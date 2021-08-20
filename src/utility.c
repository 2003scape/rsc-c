#include "utility.h"

void charrev(char *s, int l) {
    int end = l - 1;

    for (int i = 0; i < l / 2; i++) {
        char t = s[i];
        s[i] = s[end];
        s[end] = t;
        end--;
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

    return ((buffer[offset] & 0xff) - 128 << 24) +
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

void format_auth_string(char *raw, int max_length, char *formatted) {
    int raw_length = strlen(raw);

    for (int i = 0; i < max_length; i++) {
        char char_code = raw[i];

        if (i >= raw_length) {
            formatted[i] = ' ';
        } else if ((char_code >= 'a' && char_code <= 'z') ||
                   (char_code >= 'A' && char_code <= 'Z') ||
                   (char_code >= '0' && char_code <= '9')) {
            formatted[i] = raw[i];
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
    char cleaned[MAX_USER_LENGTH + 1];

    int username_length = strlen(username);

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

    int i = 0;

    while (encoded != 0) {
        char char_code = (encoded % 37);
        encoded /= 37;

        if (char_code == 0) {
            decoded[i++] = ' ';
        } else if (char_code < 27) {
            if (encoded % 37 == 0) {
                decoded[i++] = (char_code + 65 - 1);
            } else {
                decoded[i++] = (char_code + 97 - 1);
            }
        } else {
            decoded[i++] = (char_code + 48 - 27);
        }
    }

    charrev(decoded, i);

    decoded[i++] = '\0';
}

static int32_t hash_file_name(char *file_name) {
    int i = 0;

    while (file_name[i]) {
        file_name[i] = toupper(file_name[i]);
        i++;
    }

    int file_name_length = i;
    int32_t hash = 0;

    for (int i = 0; i < file_name_length; i++) {
        hash = hash * 61 + fileName[i] - 32;
    }

    return hash;
}

static int32_t get_file_hash(int32_t *buffer, int entry) {
    return (buffer[entry * 10 + 2] & 0xff) * 0x1000000 +
           (buffer[entry * 10 + 3] & 0xff) * 0x10000 +
           (buffer[entry * 10 + 4] & 0xff) * 0x100 +
           (buffer[entry * 10 + 5] & 0xff);
}

static int32_t get_file_size(int32_t *buffer, int entry) {
    return (buffer[entry * 10 + 6] & 0xff) * 0x10000 +
           (buffer[entry * 10 + 7] & 0xff) * 0x100 +
           (buffer[entry * 10 + 8] & 0xff);
}

static int32_t get_archive_size(int32_t *buffer, int entry) {
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
    int num_entries = get_unsigned_short(buffer, 0);
    int32_t wanted_hash = hash_file_name(file_name);
    int offset = 2 + num_entries * 10;

    for (int entry = 0; entry < num_entries; entry++) {
        int32_t file_hash = get_file_hash(buffer, entry);
        int file_size = get_file_size(buffer, entry);
        int archive_size = get_archive_size(buffer, entry);

        if (file_hash == wanted_hash) {
            if (file_data == NULL) {
                file_data = malloc(file_size + extra_size);
            }

            if (file_size != archive_size) {
                bzip_decompress(file_data, file_size, archive_data,
                                archive_size, offset);
            } else {
                for (int i = 0; i < file_size; i++) {
                    fileData[i] = archiveData[offset + i];
                }
            }

            return file_data;
        }

        offset += archive_size;
    }
}
