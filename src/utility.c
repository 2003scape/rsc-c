#include "utility.h"

int sin_cos_512[512];
int sin_cos_2048[2048];

int BITMASK[] = {0,          1,          3,         7,         15,
                 31,         63,         127,       255,       511,
                 1023,       2047,       4095,      8191,      16383,
                 32767,      65535,      0x1ffff,   0x3ffff,   0x7ffff,
                 0xfffff,    0x1fffff,   0x3fffff,  0x7fffff,  0xffffff,
                 0x1ffffff,  0x3ffffff,  0x7ffffff, 0xfffffff, 0x1fffffff,
                 0x3fffffff, 0x7fffffff, -1};

void init_utility_global() {
    for (int i = 0; i < 256; i++) {
        sin_cos_512[i] = (int)(sin((double)i * 0.02454369) * 32768);
        sin_cos_512[i + 256] = (int)(cos((double)i * 0.02454369) * 32768);
    }

    /* sin(i) = sin((i / 256.0f) * (M_PI / 2)) */
    for (int i = 0; i < 1024; i++) {
        sin_cos_2048[i] = (int)(sin((double)i * 0.00613592315) * 32768);
        sin_cos_2048[i + 1024] = (int)(cos((double)i * 0.00613592315) * 32768);
    }
}

void strtrim(char *s) {
    char *p = s;
    int l = strlen(p);

    while (isspace(p[l - 1])) {
        p[--l] = 0;
    }

    while (*p && isspace(*p)) {
        ++p;
        --l;
    }

    memmove(s, p, l + 1);
}

void strtolower(char *s) {
    for (int i = 0; s[i]; i++) {
        s[i] = tolower(s[i]);
    }
}

int get_unsigned_byte(int8_t i) { return i & 0xff; }

int get_unsigned_short(int8_t *buffer, int offset) {
    return ((buffer[offset] & 0xff) << 8) + (buffer[offset + 1] & 0xff);
}

int get_unsigned_int(int8_t *buffer, int offset) {
    return ((buffer[offset] & 0xff) << 24) +
           ((buffer[offset + 1] & 0xff) << 16) +
           ((buffer[offset + 2] & 0xff) << 8) + (buffer[offset + 3] & 0xff);
}

int64_t get_unsigned_long(int8_t *buffer, int offset) {
    return (((int64_t)get_unsigned_int(buffer, offset) & 0xffffffff) << 32) +
           ((int64_t)get_unsigned_int(buffer, offset + 4) & 0xffffffff);
}

int get_signed_short(int8_t *buffer, int offset) {
    int i = get_unsigned_byte(buffer[offset]) * 256 +
            get_unsigned_byte(buffer[offset + 1]);

    if (i > 32767) {
        i -= 0x10000;
    }

    return i;
}

/* used for inventory and bank items, since most of the time there's less than
 * 128 */
int get_stack_int(int8_t *buffer, int offset) {
    if ((buffer[offset] & 0xff) < 128) {
        return buffer[offset];
    }

    return (((buffer[offset] & 0xff) - 128) << 24) +
           ((buffer[offset + 1] & 0xff) << 16) +
           ((buffer[offset + 2] & 0xff) << 8) + (buffer[offset + 3] & 0xff);
}

int get_bit_mask(int8_t *buffer, int offset, int length) {
    int byte_offset = offset >> 3;
    int bit_offset = 8 - (offset & 7);
    int bits = 0;

    for (; length > bit_offset; bit_offset = 8) {
        bits += (buffer[byte_offset++] & BITMASK[bit_offset])
                << (length - bit_offset);

        length -= bit_offset;
    }

    if (length == bit_offset) {
        bits += buffer[byte_offset] & BITMASK[bit_offset];
    } else {
        bits +=
            (buffer[byte_offset] >> (bit_offset - length)) & BITMASK[length];
    }

    return bits;
}

void write_unsigned_int(int8_t *buffer, int index, int i) {
    buffer[index] = (int8_t)(i >> 24);
    buffer[index + 1] = (int8_t)(i >> 16);
    buffer[index + 2] = (int8_t)(i >> 8);
    buffer[index + 3] = (int8_t)i;
}

void format_auth_string(char *raw, int max_length, char *formatted) {
    int raw_length = strlen(raw);

    for (int i = 0; i < max_length; i++) {
        char char_code = raw[i];

        if (i >= raw_length) {
            formatted[i] = ' ';
        } else if ((char_code >= 'a' && char_code <= 'z') ||
                   (char_code >= 'A' && char_code <= 'Z') ||
                   (char_code >= '0' && char_code <= '9')) {
            formatted[i] = char_code;
        } else {
            formatted[i] = '_';
        }
    }

    formatted[max_length] = '\0';
}

void ip_to_string(int32_t ip, char *ip_string) {
    sprintf(ip_string, "%d.%d.%d.%d", (ip >> 24) & 0xff, (ip >> 16) & 0xff,
            (ip >> 8) & 0xff, ip & 0xff);
}

int64_t encode_username(char *username) {
    int username_length = strlen(username);
    char cleaned[username_length + 1];

    for (int i = 0; i < username_length; i++) {
        char char_code = username[i];

        if ((char_code >= 'a' && char_code <= 'z') ||
            (char_code >= '0' && char_code <= '9')) {
            cleaned[i] = char_code;
        } else if (char_code >= 'A' && char_code <= 'Z') {
            cleaned[i] = char_code + 97 - 65;
        } else {
            cleaned[i] = ' ';
        }
    }

    cleaned[username_length] = '\0';

    strtrim(cleaned);

    int cleaned_length = strlen(cleaned);

    if (cleaned_length > MAX_USER_LENGTH) {
        cleaned[MAX_USER_LENGTH + 1] = '\0';
        cleaned_length = MAX_USER_LENGTH;
    }

    int64_t encoded = 0;

    for (int i = 0; i < cleaned_length; i++) {
        char char_code = cleaned[i];

        encoded *= 37;

        if (char_code >= 'a' && char_code <= 'z') {
            encoded += 1 + char_code - 97;
        } else if (char_code >= '0' && char_code <= '9') {
            encoded += 27 + char_code - 48;
        }
    }

    return encoded;
}

void decode_username(int64_t encoded, char *decoded) {
    if (encoded < 0) {
        strcpy(decoded, "invalidName");
    }

    int length = 0;

    while (encoded != 0) {
        char char_code = (encoded % 37);
        encoded /= 37;

        if (char_code == 0) {
            decoded[length++] = ' ';
        } else if (char_code < 27) {
            if (encoded % 37 == 0) {
                decoded[length++] = (char_code + 65 - 1);
            } else {
                decoded[length++] = (char_code + 97 - 1);
            }
        } else {
            decoded[length++] = (char_code + 48 - 27);
        }
    }

    int end = length - 1;

    for (int i = 0; i < length / 2; i++) {
        char temp = decoded[i];
        decoded[i] = decoded[end];
        decoded[end] = temp;
        end--;
    }

    decoded[length++] = '\0';
}

static int32_t hash_file_name(char *file_name) {
    int i = 0;

    char upper_file_name[strlen(file_name) + 1];

    while (file_name[i]) {
        upper_file_name[i] = toupper(file_name[i]);
        i++;
    }

    file_name = upper_file_name;

    int file_name_length = i;
    int32_t hash = 0;

    for (int i = 0; i < file_name_length; i++) {
        hash = hash * 61 + file_name[i] - 32;
    }

    return hash;
}

static int32_t get_file_hash(int8_t *buffer, int entry) {
    return (buffer[entry * 10 + 2] & 0xff) * 0x1000000 +
           (buffer[entry * 10 + 3] & 0xff) * 0x10000 +
           (buffer[entry * 10 + 4] & 0xff) * 0x100 +
           (buffer[entry * 10 + 5] & 0xff);
}

static int get_file_size(int8_t *buffer, int entry) {
    return (buffer[entry * 10 + 6] & 0xff) * 0x10000 +
           (buffer[entry * 10 + 7] & 0xff) * 0x100 +
           (buffer[entry * 10 + 8] & 0xff);
}

static int get_archive_size(int8_t *buffer, int entry) {
    return (buffer[entry * 10 + 9] & 0xff) * 0x10000 +
           (buffer[entry * 10 + 10] & 0xff) * 0x100 +
           (buffer[entry * 10 + 11] & 0xff);
}

int get_data_file_offset(char *file_name, int8_t *buffer) {
    int num_entries = get_unsigned_short(buffer, 0);
    int32_t wanted_hash = hash_file_name(file_name);
    int offset = 2 + num_entries * 10;

    for (int entry = 0; entry < num_entries; entry++) {
        int32_t file_hash = get_file_hash(buffer, entry);
        int archive_size = get_archive_size(buffer, entry);

        if (file_hash == wanted_hash) {
            return offset;
        }

        offset += archive_size;
    }

    return 0;
}

int get_data_file_length(char *file_name, int8_t *buffer) {
    int num_entries = get_unsigned_short(buffer, 0);
    int32_t wanted_hash = hash_file_name(file_name);

    for (int entry = 0; entry < num_entries; entry++) {
        int32_t file_hash = get_file_hash(buffer, entry);
        int file_size = get_file_size(buffer, entry);

        if (file_hash == wanted_hash) {
            return file_size;
        }
    }

    return 0;
}

int8_t *unpack_data(char *file_name, int extra_size, int8_t *archive_data,
                    int8_t *file_data) {
    int num_entries = get_unsigned_short(archive_data, 0);
    int32_t wanted_hash = hash_file_name(file_name);
    int offset = 2 + num_entries * 10;

    for (int entry = 0; entry < num_entries; entry++) {
        int32_t file_hash = get_file_hash(archive_data, entry);
        int file_size = get_file_size(archive_data, entry);
        int archive_size = get_archive_size(archive_data, entry);

        if (file_hash == wanted_hash) {
            if (file_data == NULL) {
                file_data = malloc(file_size + extra_size);
            }

            if (file_size != archive_size) {
                bzip_decompress(file_data, file_size, archive_data,
                                archive_size, offset);
            } else {
                for (int i = 0; i < file_size; i++) {
                    file_data[i] = archive_data[offset + i];
                }
            }

            return file_data;
        }

        offset += archive_size;
    }

    return NULL;
}

int8_t *load_data(char *file_name, int extra_size, int8_t *archive_data) {
    if (archive_data == NULL) {
        return NULL;
    }

    return unpack_data(file_name, extra_size, archive_data, NULL);
}

void format_confirm_amount(int amount, char *formatted) {
    sprintf(formatted, "%d", amount);

    int formatted_length = strlen(formatted);

    for (int i = formatted_length - 3; i > 0; i -= 3) {
        int begin_length = i;
        char begin[begin_length + 1];
        begin[begin_length] = '\0';

        int end_length = formatted_length - i;
        char end[end_length + 1];
        end[end_length] = '\0';

        strncpy(begin, formatted, begin_length);
        strncpy(end, formatted + i, end_length);

        sprintf(formatted, "%s,%s", begin, end);

        formatted_length = strlen(formatted);
    }

    if (formatted_length > 8) {
        int short_length = formatted_length - 8;
        char short_num[short_length + 1];
        short_num[short_length] = '\0';

        strncpy(short_num, formatted, short_length);

        char formatted_copy[formatted_length];
        strcpy(formatted_copy, formatted);

        sprintf(formatted, "@gre@%s million @whi@(%s)", short_num,
                formatted_copy);
    } else if (formatted_length > 4) {
        int short_length = formatted_length - 4;
        char short_num[short_length + 1];
        short_num[short_length] = '\0';

        strncpy(short_num, formatted, short_length);

        char formatted_copy[formatted_length];
        strcpy(formatted_copy, formatted);

        sprintf(formatted, "@cya@%s K @whi@(%s)", short_num, formatted_copy);
    }
}

int get_ticks() {
#if !defined(WII) && !defined(_3DS)
    return SDL_GetTicks();
#endif

#ifdef _3DS
    return (int)(svcGetSystemTick() / CPU_TICKS_PER_MSEC);
#endif

#ifdef WII
    uint64_t ticks = gettime();
    return (int)(ticks / TB_TIMER_CLOCK);
#endif
}

void delay_ticks(int ticks) {
#if !defined(WII) && !defined(_3DS)
#ifdef EMSCRIPTEN
    emscripten_sleep(ticks);
#else
    SDL_Delay(ticks);
#endif
#endif

#ifdef _3DS
    svcSleepThread(ticks * 1000);
#endif

#ifdef WII
    int end = get_ticks() + ticks;

    while (get_ticks() != end)
        ;
#endif
}

void get_level_difference_colour(int level_difference, char *colour) {
    if (level_difference < 0) {
        strcpy(colour, "@or1@");
    }

    if (level_difference < -3) {
        strcpy(colour, "@or2@");
    }

    if (level_difference < -6) {
        strcpy(colour, "@or3@");
    }

    if (level_difference < -9) {
        strcpy(colour, "@red@");
    }

    if (level_difference > 0) {
        strcpy(colour, "@gr1@");
    }

    if (level_difference > 3) {
        strcpy(colour, "@gr2@");
    }

    if (level_difference > 6) {
        strcpy(colour, "@gr3@");
    }

    if (level_difference > 9) {
        strcpy(colour, "@gre@");
    }
}

void ulaw_to_linear(long size, uint8_t *u_ptr, int16_t *out_ptr) {
    for (long i = 0; i < size; i++) {
        uint16_t u_val = ~(*u_ptr++);

        short t = ((u_val & QUANT_MASK) << 3) + BIAS;
        t <<= ((unsigned)u_val & SEG_MASK) >> SEG_SHIFT;

        *out_ptr++ = ((u_val & SIGN_BIT) ? (BIAS - t) : (t - BIAS));
    }
}

void format_number_commas(int number, char *dest) {
    if (number < 1000) {
        sprintf(dest, "%d", number);
    } else {
        format_number_commas(number / 1000, dest);
        sprintf(dest + strlen(dest), ",%03d", number % 1000);
    }
}

void format_amount_suffix(int amount, int use_colour, int convert_ten_thousands,
                          int use_commas, char *dest) {
    float scale = 1.0f;
    char colour[6] = {0};
    char suffix = '\0';

    if (amount >= 10000000) {
        scale = 1000000.0f;
        strcpy(colour, "@gre@");
        suffix = 'M';
    } else if (amount >= (convert_ten_thousands ? 10000 : 100000)) {
        scale = 1000.0f;
        strcpy(colour, "@whi@");
        suffix = 'K';
    }

    amount = floorf((float)amount / scale);

    char formatted_amount[15] = {0};

    if (use_commas) {
        format_number_commas(amount, formatted_amount);
    } else {
        sprintf(formatted_amount, "%d", amount);
    }

    sprintf(dest, "%s%s%c", use_colour ? colour : "", formatted_amount, suffix);
}

#ifdef RENDER_GL
float gl_translate_coord(int position, int range) {
    float half = range / 2.0f;
    return (position - half) / half;
}

float gl_translate_x(int x, int range) { return gl_translate_coord(x, range); }

float gl_translate_y(int y, int range) { return -gl_translate_coord(y, range); }

void gl_update_texture_array(GLuint texture_array_id, int index, int width,
                             int height, int32_t *pixels, int convert_bgra) {
    /* webgl only supports RGBA */
    if (convert_bgra) {
        uint8_t *byte_pixels = (uint8_t *)pixels;

        for (int i = 0; i < width * height * 4; i += 4) {
            int b = byte_pixels[i];
            int g = byte_pixels[i + 1];
            int r = byte_pixels[i + 2];
            int a = byte_pixels[i + 3];

            pixels[i / 4] = (a << 24) | (b << 16) | (g << 8) | r;
        }
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY, texture_array_id);

    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, index, width, height, 1,
                    GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

void rotate_point(int centre_x, int centre_y, float angle, int *point) {
    point[0] -= centre_x;
    point[1] -= centre_y;

    float sine = sin(angle);
    float cosine = cos(angle);

    int x_new = point[0] * cosine - point[1] * sine;
    int y_new = point[0] * sine + point[1] * cosine;

    point[0] = x_new + centre_x;
    point[1] = y_new + centre_y;
}
#endif
