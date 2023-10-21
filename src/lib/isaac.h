/*
 * Public domain.
 */
#ifndef ISAAC_H
#define ISAAC_H
#include <stdint.h>

struct isaac {
	/* external results */
	uint32_t randrsl[256], randcnt;

	/* internal state */
	uint32_t mm[256];
	uint32_t aa, bb, cc;
};

uint32_t isaac_next(struct isaac *);
void isaac_init(struct isaac *, int);
#endif
