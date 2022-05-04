#ifndef _H_UTILITY
#define _H_UTILITY

#include <ctype.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#include "bzip.h"

#ifdef _3DS
#include <3ds.h>
#endif

#ifdef WII
#include <ogc/lwp_watchdog.h>
#include <ogcsys.h>
#endif

#define SIGN_BIT (0x80)  /* Sign bit for a A-law byte.        */
#define QUANT_MASK (0xf) /* Quantization field mask.          */
#define SEG_SHIFT (4)    /* Left shift for segment number.    */
#define SEG_MASK (0x70)  /* Segment field mask.               */
#define BIAS (0x84)      /* Bias for linear code (for u-law). */

#define MAX_USER_LENGTH 12
#define MAX_CONFIRM_LENGTH 40

/*
#define CHAR_SET                                                               \
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01234567\
89!\"£$%^&*()-_=+[{]};:'@#~,<.>/?\\| "*/

/* disabled pound (£) */

#define CHAR_SET                                                               \
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01234567\
89!\"$%^&*()-_=+[{]};:'@#~,<.>/?\\| "

#define CHAR_SET_LENGTH 94

extern int sin_cos_512[512];
extern int sin_cos_2048[2048];
extern int BITMASK[];

void init_utility_global();
void charrev(char *s, int l);
void strtrim(char *s);

int get_unsigned_byte(int8_t i);
int get_unsigned_short(int8_t *buffer, int offset);
int get_unsigned_int(int8_t *buffer, int offset);
int64_t get_unsigned_long(int8_t *buffer, int offset);
int get_signed_short(int8_t *buffer, int offset);
int get_stack_int(int8_t *buffer, int offset);
int get_bit_mask(int8_t *buffer, int offset, int length);
void write_unsigned_int(int8_t *buffer, int index, int i);
void format_auth_string(char *raw, int max_length, char *formatted);
void ip_to_string(int32_t ip, char *ip_string);
int64_t encode_username(char *username);
void decode_username(int64_t encoded, char *decoded);
int get_data_file_offset(char *file_name, int8_t *buffer);
int get_data_file_length(char *file_name, int8_t *buffer);
int8_t *unpack_data(char *file_name, int extra_size, int8_t *archive_data,
                    int8_t *file_data);
int8_t *load_data(char *file_name, int extra_size, int8_t *archive_data);
void format_confirm_amount(int amount, char *formatted);
int get_ticks();
void delay_ticks(int ticks);
void get_level_difference_colour(int level_difference, char *colour);
void ulaw_to_linear(long size, uint8_t *u_ptr, int16_t *out_ptr);

#ifdef RENDER_GL
float translate_gl_coord(int position, int range);
float translate_gl_x(int x, int range);
float translate_gl_y(int y, int range);
#endif

#endif
