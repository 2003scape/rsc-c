#include <stddef.h>
#include <string.h>
#include "rsa.h"

#ifndef WITH_RSA_OPENSSL
/* from RSC Sundae. Public domain. */

static void
pow_mod_faster(struct bn *, struct bn *, struct bn *, struct bn *);

/* O(log n) */
static void
pow_mod_faster(struct bn *a, struct bn *b, struct bn *n, struct bn *res)
{
	bignum_from_int(res, 1); /* r = 1 */

	struct bn tmpa;
	struct bn tmpb;
	struct bn tmp;

	bignum_assign(&tmpa, a);
	bignum_assign(&tmpb, b);

	for (;;) {
		if (tmpb.array[0] & 1) {     /* if (b % 2) */
			bignum_mul(res, &tmpa, &tmp);  /*   r = r * a % m */
			bignum_mod(&tmp, n, res);
		}
		bignum_rshift(&tmpb, &tmp, 1); /* b /= 2 */
		bignum_assign(&tmpb, &tmp);

		if (bignum_is_zero(&tmpb)) {
			break;
		}

		bignum_mul(&tmpa, &tmpa, &tmp);
		bignum_mod(&tmp, n, &tmpa);
	}
}

int
rsa_init(struct rsa *rsa, const char *exponent, const char *modulus)
{
	bignum_init(&rsa->exponent);
	bignum_from_string(&rsa->exponent, (char *)exponent, strlen(exponent));

	bignum_init(&rsa->modulus);
	bignum_from_string(&rsa->modulus, (char *)modulus, strlen(modulus));

	return 0;
}

int
rsa_crypt(struct rsa *rsa, void *buffer, size_t len,
	    void *out, size_t outlen)
{
	struct bn encrypted = {0};
	struct bn result = {0};
	uint8_t *inbuf = buffer;
	uint8_t *outbuf = out;

	bignum_init(&encrypted);
	for (size_t i = 0; i < len; ++i) {
		encrypted.array[i] = inbuf[len - 1 - i];
	}

	bignum_init(&result);
	pow_mod_faster(&encrypted, &rsa->exponent, &rsa->modulus, &result);

	for (int i = (BN_ARRAY_SIZE - 1); i >= 0; i--) {
		if (result.array[i] != 0) {
			if ((i + 1) > (int)outlen) {
				return -1;
			}
			outlen = i + 1;
			break;
		}
	}

	for (size_t i = 0; i < outlen; ++i) {
		outbuf[i] = result.array[outlen - 1 - i];
	}

	return (int)outlen;
}
#endif
