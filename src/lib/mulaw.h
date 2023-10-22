#ifndef MULAW_H
#define MULAW_H

#include <stdint.h>

/*
 * Public domain.
 * - Stormy
 */

void
audio_mulaw_to_linear16(int16_t *, uint8_t *, unsigned int);

#endif
