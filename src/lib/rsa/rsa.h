#ifndef RSA_H
#define RSA_H

/* from RSC Sundae. Public domain. */

#ifdef WITH_RSA_OPENSSL
#include <openssl/bn.h>

struct rsa {
	BN_CTX *ctx;
	BIGNUM *exponent;
	BIGNUM *modulus;
};
#else
#include "../bn.h"

struct rsa {
	struct bn exponent;
	struct bn modulus;
};
#endif

int rsa_init(struct rsa *, const char *, const char *);
int rsa_crypt(struct rsa *, void *, size_t, void *, size_t);

#endif
