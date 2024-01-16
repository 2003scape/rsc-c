#ifndef _H_UTILITY
#define _H_UTILITY

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#ifdef WII
#include <ogc/lwp_watchdog.h>
#include <ogcsys.h>
#elif defined(_3DS)
#include <3ds.h>

#if defined(RENDER_3DS_GL)
#include <citro3d.h>
#include <tex3ds.h>
#endif
#else
#ifdef __SWITCH__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

#ifdef RENDER_GL
#ifdef __SWITCH__
#include <SDL2/SDL_image.h>
#else
#include <SDL_image.h>
#endif
#ifdef GLAD
#ifdef __SWITCH__
#include <glad/glad.h>
#else
#include "../../glad/glad.h"
#endif
#else
#include <GL/glew.h>
#include <GL/glu.h>
#endif
#endif
#endif

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
#define GL_TEXTURE_SIZE 1024.0f
#endif

#include "colours.h"
#include "lib/bzip.h"

/* for audio conversion */
#define SIGN_BIT 0x80  /* sign bit for a a-law byte */
#define QUANT_MASK 0xf /* quantization field mask */
#define SEG_SHIFT 4    /* left shift for segment number */
#define SEG_MASK 0x70  /* segment field mask */
#define BIAS 0x84      /* bias for linear code (for u-law) */

#define MAX_USER_LENGTH 12
#define MAX_CONFIRM_LENGTH 40

/*
#define CHAR_SET                                                               \
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01234567\
89!\"£$%^&*()-_=+[{]};:'@#~,<.>/?\\| "*/

/* disabled pound (£) */
#define CHAR_SET                                                               \
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01234567\
89!\"A$%^&*()-_=+[{]};:'@#~,<.>/?\\| "

#define CHAR_SET_LENGTH 95

#define IS_DIGIT_SEPARATOR(char_code)                                          \
    (char_code == ' ' || char_code == ',' || char_code == '.')

#define IS_DIGIT_SUFFIX(char_code)                                             \
    (tolower(char_code) == 'k' || tolower(char_code) == 'm')

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
/* convert sin_cos_x array index to radians */
#define TABLE_TO_RADIANS(i, length) (((i) / ((length) / 8.0f)) * (M_PI / 2))

/* convert integer vertices to floats and vice versa */
#define VERTEX_SCALE 100 // TODO try other values
#define VERTEX_TO_FLOAT(vertex) (vertex / (float)VERTEX_SCALE)
#define FLOAT_TO_VERTEX(f) (int)(f * VERTEX_SCALE)
#endif

extern int sin_cos_512[512];
extern int sin_cos_2048[2048];
extern int BITMASK[];

extern int certificate_items[][2];

#ifdef RENDER_3DS_GL
extern int _3ds_gl_framebuffer_offsets_x[];
extern int _3ds_gl_framebuffer_offsets_y[];
#endif

void init_utility_global();

void mud_log(char *format, ...);
void mud_error(char *format, ...);

char *strcat_realloc(char *s, const char *new);
char *mud_strdup(const char *s);
void strtrim(char *s);
void strtolower(char *s);

int get_signed_byte(void *, size_t, size_t);
int get_unsigned_byte(void *, size_t, size_t);
int get_unsigned_short(void *, size_t, size_t);
int get_unsigned_short_le(void *, size_t, size_t);
int get_unsigned_int(void *, size_t, size_t);
int64_t get_unsigned_long(void *, size_t, size_t);
int get_signed_short(void *, size_t, size_t);
int get_stack_int(void *, size_t, size_t);
int get_bit_mask(void *, size_t, size_t, size_t);
void write_unsigned_int(void *buffer, size_t index, int i);
void format_auth_string(char *raw, int max_length, char *formatted);
void ip_to_string(int32_t ip, char *ip_string);
int64_t encode_username(char *username);
void decode_username(int64_t encoded, char *decoded);
uint32_t get_data_file_offset(const char *file_name, void *buffer);
uint32_t get_data_file_length(const char *file_name, void *buffer);
void *unpack_data(const char *file_name, size_t extra_size, void *archive_data,
                  void *data_out, size_t *size_out);
void *load_data(const char *file_name, size_t extra_size,
                  void *archive_data, size_t *size_out);
void format_confirm_amount(int amount, char *formatted);
int get_ticks();
void delay_ticks(int ticks);
void get_level_difference_colour(int level_difference, char *colour);
void ulaw_to_linear(long size, uint8_t *u_ptr, int16_t *out_ptr);
void format_number_commas(int number, char *dest);
void format_amount_suffix(int amount, int use_colour, int convert_ten_thousands,
                          int use_commas, char *dest);
void url_encode(const char *s, char *dest);
int get_certificate_item_id(int item_id);
int is_ip_address(char *address);
int colour_str_to_colour(char *colour_str);
double distance(int x1, int y1, int x2, int y2);

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
float gl_translate_coord(int position, int range);
float gl_translate_x(int x, int range);
float gl_translate_y(int y, int range);
#endif
#ifdef RENDER_GL
void rotate_point(int centre_x, int centre_y, float angle, int *point);
void gl_create_texture(GLuint *texture_id);
void gl_load_texture(GLuint *texture_id, char *file);
#elif defined(RENDER_3DS_GL)
void _3ds_gl_load_tex(const uint8_t *t3x_data, size_t t3x_size, C3D_Tex *tex);
int _3ds_gl_translate_framebuffer_index(int index);
int _3ds_gl_morton_interleave(int x, int y);
int _3ds_gl_get_morton_offset(int x, int y, int bytes_per_pixel);
int _3ds_gl_translate_texture_index(int x, int y, int size);
uint16_t _3ds_gl_rgb32_to_rgba5551(int colour32);
int _3ds_gl_rgba5551_to_rgb32(uint16_t colour16);
#endif
#endif
