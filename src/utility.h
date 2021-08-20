#ifndef _H_UTILITY
#define _H_UTILITY

#include <ctype.h>
#include <stdint.h>
#include <string.h>

#include "bzip.h"

#define MAX_USER_LENGTH 12

static const int BITMASK[] = {
    0,          1,          3,         7,         15,        31,
    63,         127,        255,       511,       1023,      2047,
    4095,       8191,       16383,     32767,     65535,     0x1ffff,
    0x3ffff,    0x7ffff,    0xfffff,   0x1fffff,  0x3fffff,  0x7fffff,
    0xffffff,   0x1ffffff,  0x3ffffff, 0x7ffffff, 0xfffffff, 0x1fffffff,
    0x3fffffff, 0x7fffffff, -1};

void charrev(char *s, int l);
void strtrim(char *s);

int get_unsigned_byte(int8_t i);
int get_unsigned_short(int8_t *buffer, int offset);
int get_unsigned_int(int8_t *buffer, int offset);
int64_t get_unsigned_long(int8_t *buffer, int offset);
int get_signed_short(int8_t *buffer, int offset);
int get_stack_int(int8_t *buffer, int offset);
int get_bit_mask(int8_t *buffer, int offset, int length);
void format_auth_string(char *raw, int max_length, char *formatted);
void ip_to_string(int32_t ip, char *ip_string);
int64_t encode_username(char *username);
void decode_username(int64_t encoded, char *decoded);
int get_data_file_offset(char *file_name, int8_t *buffer);
int get_data_file_length(char *file_name, int8_t *buffer);
int8_t *unpack_data(char *file_name, int extra_size, int8_t *archive_data,
                    int8_t *file_data);
int8_t load_data(char *file_name, int extra_size, int8_t archive_data);
void format_confirm_amount(int amount, char *formatted);

#endif
