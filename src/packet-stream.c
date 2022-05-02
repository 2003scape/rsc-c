#include "packet-stream.h"

#ifdef REVISION_177
char *SPOOKY_THREAT =
    "All RuneScape code and data, including this message, are copyright 2003 "
    "Jagex Ltd. Unauthorised reproduction in any form is strictly prohibited.  "
    "The RuneScape network protocol is copyright 2003 Jagex Ltd and is "
    "protected by international copyright laws. The RuneScape network protocol "
    "also incorporates a copy protection mechanism to prevent unauthorised "
    "access or use of our servers. Attempting to break, bypass or duplicate "
    "this mechanism is an infringement of the Digital Millienium Copyright Act "
    "and may lead to prosecution. Decompiling, or reverse-engineering the "
    "RuneScape code in any way is strictly prohibited. RuneScape and Jagex are "
    "registered trademarks of Jagex Ltd. You should not be reading this "
    "message, you have been warned...";

int THREAT_LENGTH = 0;

int OPCODE_ENCRYPTION[] = {
    124, 345, 953, 124, 634, 636, 661, 127, 177, 295, 559, 384, 321, 679, 871,
    592, 679, 347, 926, 585, 681, 195, 785, 679, 818, 115, 226, 799, 925, 852,
    194, 966, 32,  3,   4,   5,   6,   7,   8,   9,   40,  1,   2,   3,   4,
    5,   6,   7,   8,   9,   50,  444, 52,  3,   4,   5,   6,   7,   8,   9,
    60,  1,   2,   3,   4,   5,   6,   7,   8,   9,   70,  1,   2,   3,   4,
    5,   6,   7,   8,   9,   80,  1,   2,   3,   4,   5,   6,   7,   8,   9,
    90,  1,   2,   3,   4,   5,   6,   7,   8,   9,   100, 1,   2,   3,   4,
    5,   6,   7,   8,   9,   110, 1,   2,   3,   4,   5,   6,   7,   8,   9,
    120, 1,   2,   3,   4,   5,   6,   7,   8,   9,   130, 1,   2,   3,   4,
    5,   6,   7,   8,   9,   140, 1,   2,   3,   4,   5,   6,   7,   8,   9,
    150, 1,   2,   3,   4,   5,   6,   7,   8,   9,   160, 1,   2,   3,   4,
    5,   6,   7,   8,   9,   170, 1,   2,   3,   4,   5,   6,   7,   8,   9,
    180, 1,   2,   3,   4,   5,   6,   7,   8,   9,   694, 235, 846, 834, 300,
    200, 298, 278, 247, 286, 346, 144, 23,  913, 812, 765, 432, 176, 935, 452,
    542, 45,  346, 65,  637, 62,  354, 123, 34,  912, 812, 834, 698, 324, 872,
    912, 438, 765, 344, 731, 625, 783, 176, 658, 128, 854, 489, 85,  6,   865,
    43,  573, 132, 527, 235, 434, 658, 912, 825, 298, 753, 282, 652, 439, 629,
    945};

char *rsa_exponent = "00010001";

char *rsa_modulus =
    "87cef754966ecb19806238d9fecf0f421e816976f74f365c86a584e51049794d41fefbdc5f"
    "ed3a3ed3b7495ba24262bb7d1dd5d2ff9e306b5bbf5522a2e85b25";

int get_client_opcode_friend(int opcode) {
    switch (opcode) {
    case CLIENT_LOGIN:
        return CLIENT_LOGIN_FRIEND;
    case CLIENT_RECONNECT:
        return CLIENT_RECONNECT_FRIEND;
    }

    return -1;
}

void init_packet_stream_global() { THREAT_LENGTH = strlen(SPOOKY_THREAT); }
#endif

void packet_stream_new(PacketStream *packet_stream, mudclient *mud) {
    memset(packet_stream, 0, sizeof(PacketStream));

#ifdef REVISION_177
    packet_stream->decode_key = 3141592;
    packet_stream->encode_key = 3141592;
#endif

    int ret;

#ifdef WII
    char localip[16] = {0};
    char gateway[16] = {0};
    char netmask[16] = {0};

    ret = if_config(localip, netmask, gateway, TRUE, 20);

    if (ret < 0) {
        fprintf(stderr, "if_config(): %d\n", ret);
        exit(1);
    }
#endif

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(mud->port);

#ifdef WIN32
    WSADATA wsa_data;
    ret = WSAStartup(MAKEWORD(2, 2), &wsa_data);

    if (ret != 0) {
        fprintf(stderr, "WSAStartup: %d\n", ret);
        exit(1);
    }

    ret = InetPton(AF_INET, mud->server, &server_addr.sin_addr);
#else
    ret = inet_aton(mud->server, &server_addr.sin_addr);
#endif

    if (ret == 0) {
        fprintf(stderr, "inet_aton(%s) error: %d\n", mud->server, ret);
        exit(1);
    }

#ifdef WII
    packet_stream->socket = net_socket(AF_INET, SOCK_STREAM, 0);
#else
    packet_stream->socket = socket(AF_INET, SOCK_STREAM, 0);
#endif

    if (packet_stream->socket < 0) {
        fprintf(stderr, "socket() error: %d\n", packet_stream->socket);
        exit(1);
    }

    int set = 1;

#ifdef WII
    net_setsockopt(packet_stream->socket, IPPROTO_TCP, TCP_NODELAY, &set,
                   sizeof(set));

    ret = net_connect(packet_stream->socket, (struct sockaddr *)&server_addr,
                      sizeof(server_addr));
#else
#ifdef WIN32
    setsockopt(packet_stream->socket, IPPROTO_TCP, TCP_NODELAY, (char *) &set,
               sizeof(set));
#else
    setsockopt(packet_stream->socket, IPPROTO_TCP, TCP_NODELAY, &set,
               sizeof(set));
#endif

#ifdef EMSCRIPTEN
    int attempts_ms = 0;

    do {
        if (attempts_ms >= 5000) {
            break;
        }

        ret = connect(packet_stream->socket, (struct sockaddr *)&server_addr,
                      sizeof(server_addr));

        //printf("%d %d\n", ret, errno);

        if (errno == 30) {
            ret = 0;
            break;
        } else if (errno != EINPROGRESS && errno != 7) {
            /* not sure what 7 is, but i'm not worried about portability
             * since this is explicitly for emscripten */
            break;
        }

        delay_ticks(100);
        attempts_ms += 100;
    } while (ret == -1);
#else
    ret = connect(packet_stream->socket, (struct sockaddr *)&server_addr,
                  sizeof(server_addr));
#endif
#endif

    if (ret < 0) {
        fprintf(stderr, "connect() error: %d\n", ret);
        packet_stream->closed = 1;
    } else {
        packet_stream->closed = 0;

#ifdef WII
        ret = net_ioctl(packet_stream->socket, FIONBIO, &set);
#else
#ifdef WIN32
        u_long mode = 1;
        ioctlsocket(packet_stream->socket, FIONBIO, &mode);
#endif

#if !defined(EMSCRIPTEN) && !defined(WIN32)
        ret = ioctl(packet_stream->socket, FIONBIO, &set);
#endif
#endif

#if !defined(EMSCRIPTEN) && !defined(WIN32)
        if (ret < 0) {
            fprintf(stderr, "ioctl() error: %d\n", ret);
            exit(1);
        }
#endif
    }

    packet_stream->packet_end = 3;
    packet_stream->packet_max_length = 5000;
}

int packet_stream_available_bytes(PacketStream *packet_stream, int length) {
    if (packet_stream->available_length >= length) {
        return 1;
    }

#ifdef WII
    int bytes = net_recv(packet_stream->socket,
                         packet_stream->available_buffer +
                             packet_stream->available_offset +
                             packet_stream->available_length,
                         length - packet_stream->available_length, 0);
#else
    int bytes =
        recv(packet_stream->socket,
             packet_stream->available_buffer + packet_stream->available_offset +
                 packet_stream->available_length,
             length - packet_stream->available_length, 0);
#endif

    if (bytes < 0) {
        bytes = 0;
    }

    packet_stream->available_length += bytes;

    if (packet_stream->available_length < length) {
        return 0;
    }

    return 1;
}

void packet_stream_read_bytes(PacketStream *packet_stream, int length,
                              int8_t *buffer) {
    if (packet_stream->closed) {
        return;
    }

    if (packet_stream->available_length > 0) {
        int copy_length;

        if (length > packet_stream->available_length) {
            copy_length = packet_stream->available_length;
        } else {
            copy_length = length;
        }

        memcpy(buffer,
               packet_stream->available_buffer +
                   packet_stream->available_offset,
               copy_length);

        length -= copy_length;

        packet_stream->available_length -= copy_length;

        if (packet_stream->available_length == 0) {
            packet_stream->available_offset = 0;
        } else {
            packet_stream->available_offset += copy_length;
        }
    }

    int offset = 0;

    while (length > 0) {
#ifdef WII
        int bytes = net_recv(packet_stream->socket, buffer + offset, length, 0);
#else
        int bytes = recv(packet_stream->socket, buffer + offset, length, 0);
#endif

        if (bytes > 0) {
            length -= bytes;
            offset += bytes;
        } else {
            // TODO up this?
            delay_ticks(1);
        }
    }
}

void packet_stream_write_bytes(PacketStream *packet_stream, int8_t *buffer,
                               int offset, int length) {
    if (!packet_stream->closed) {
#ifdef WII
        net_write(packet_stream->socket, buffer + offset, length);
#elif WIN32
        send(packet_stream->socket, buffer + offset, length, 0);
#else
        write(packet_stream->socket, buffer + offset, length);
#endif
    }
}

int packet_stream_read_byte(PacketStream *packet_stream) {
    if (packet_stream->closed) {
        return -1;
    }

    int8_t byte;
    packet_stream_read_bytes(packet_stream, 1, &byte);
    return (uint8_t)byte & 0xff;
}

int packet_stream_has_packet(PacketStream *packet_stream) {
    return packet_stream->packet_start > 0;
}

int packet_stream_read_packet(PacketStream *packet_stream, int8_t *buffer) {
    packet_stream->read_tries++;

    if (packet_stream->max_read_tries > 0 &&
        packet_stream->read_tries > packet_stream->max_read_tries) {
        packet_stream->socket_exception = 1;
        packet_stream->socket_exception_message = "time-out";
        packet_stream->max_read_tries += packet_stream->max_read_tries;

        return 0;
    }

    if (packet_stream->length == 0 &&
        packet_stream_available_bytes(packet_stream, 2)) {
        packet_stream->length = packet_stream_read_byte(packet_stream);

        if (packet_stream->length >= 160) {
            packet_stream->length = (packet_stream->length - 160) * 256 +
                                    (packet_stream_read_byte(packet_stream));
        }
    }

    if (packet_stream->length > 0 &&
        packet_stream_available_bytes(packet_stream, packet_stream->length)) {
        if (packet_stream->length >= 160) {
            packet_stream_read_bytes(packet_stream, packet_stream->length,
                                     buffer);
        } else {
            buffer[packet_stream->length - 1] =
                packet_stream_read_byte(packet_stream);

            if (packet_stream->length > 1) {
                packet_stream_read_bytes(packet_stream,
                                         packet_stream->length - 1, buffer);
            }
        }

        int i = packet_stream->length;

        packet_stream->length = 0;
        packet_stream->read_tries = 0;

        return i;
    }

    return 0;
}

void packet_stream_new_packet(PacketStream *packet_stream, int opcode) {
#ifdef REVISION_177
    packet_stream->opcode_friend = get_client_opcode_friend(opcode);
#endif

    if (packet_stream->packet_start >
        ((packet_stream->packet_max_length * 4) / 5)) {
        packet_stream_write_packet(packet_stream, 0);
    }

    packet_stream->packet_data[packet_stream->packet_start + 2] = opcode & 0xff;
    packet_stream->packet_data[packet_stream->packet_start + 3] = 0;
    packet_stream->packet_end = packet_stream->packet_start + 3;
}

#ifdef REVISION_177
int packet_stream_decode_opcode(PacketStream *packet_stream, int opcode) {
    int index = (opcode - packet_stream->decode_key) & 255;
    int decoded_opcode = OPCODE_ENCRYPTION[index];

    packet_stream->decode_threat_index =
        (packet_stream->decode_threat_index + decoded_opcode) % THREAT_LENGTH;

    char threat_character = SPOOKY_THREAT[packet_stream->decode_threat_index];

    packet_stream->decode_key = (packet_stream->decode_key * 3 +
                                 (int)threat_character + decoded_opcode) &
                                0xffff;

    return decoded_opcode;
}
#endif

void packet_stream_write_packet(PacketStream *packet_stream, int i) {
    if (packet_stream->socket_exception) {
        packet_stream->packet_start = 0;
        packet_stream->packet_end = 3;
        packet_stream->socket_exception = 0;

        fprintf(stderr, "socket exception: %s\n",
                packet_stream->socket_exception_message);

        exit(1);
    }

    packet_stream->delay++;

    if (packet_stream->delay < i) {
        return;
    }

    if (packet_stream->packet_start > 0) {
        packet_stream->delay = 0;

        packet_stream_write_bytes(packet_stream, packet_stream->packet_data, 0,
                                  packet_stream->packet_start);
    }

    packet_stream->packet_start = 0;
    packet_stream->packet_end = 3;
}

void packet_stream_send_packet(PacketStream *packet_stream) {
#ifdef REVISION_177
/*
    int i = packet_stream->packet_data[packet_stream->packet_start + 2] & 0xff;

    packet_stream->packet_data[packet_stream->packet_start + 2] =
        (int8_t)(i + packet_stream->decode_key);

    int opcode_friend = packet_stream->opcode_friend;

    packet_stream->encode_threat_index =
        (packet_stream->encode_threat_index + opcode_friend) % THREAT_LENGTH;

    char threat_character = SPOOKY_THREAT[packet_stream->encode_threat_index];

    packet_stream->encode_key =
        packet_stream->encode_key * 3 + (int)threat_character + opcode_friend &
        0xffff;
*/
#endif

    int length = packet_stream->packet_end - packet_stream->packet_start - 2;

    if (length >= 160) {
        packet_stream->packet_data[packet_stream->packet_start] =
            (160 + (length / 256)) & 0xff;

        packet_stream->packet_data[packet_stream->packet_start + 1] =
            length & 0xff;
    } else {
        packet_stream->packet_data[packet_stream->packet_start] = length & 0xff;
        packet_stream->packet_end--;

        packet_stream->packet_data[packet_stream->packet_start + 1] =
            packet_stream->packet_data[packet_stream->packet_end];
    }

    packet_stream->packet_start = packet_stream->packet_end;
}

void packet_stream_flush_packet(PacketStream *packet_stream) {
    packet_stream_send_packet(packet_stream);
    packet_stream_write_packet(packet_stream, 0);
}

void packet_stream_put_bytes(PacketStream *packet_stream, int8_t *src,
                             int offset, int length) {
    memcpy(packet_stream->packet_data + packet_stream->packet_end, src + offset,
           length);

    packet_stream->packet_end += length;
}

void packet_stream_put_byte(PacketStream *packet_stream, int i) {
    packet_stream->packet_data[packet_stream->packet_end++] = i & 0xff;
}

void packet_stream_put_short(PacketStream *packet_stream, int i) {
    packet_stream->packet_data[packet_stream->packet_end++] = (i >> 8) & 0xff;
    packet_stream->packet_data[packet_stream->packet_end++] = i & 0xff;
}

void packet_stream_put_int(PacketStream *packet_stream, int i) {
    packet_stream->packet_data[packet_stream->packet_end++] = (i >> 24) & 0xff;
    packet_stream->packet_data[packet_stream->packet_end++] = (i >> 16) & 0xff;
    packet_stream->packet_data[packet_stream->packet_end++] = (i >> 8) & 0xff;
    packet_stream->packet_data[packet_stream->packet_end++] = i & 0xff;
}

void packet_stream_put_long(PacketStream *packet_stream, int64_t i) {
    packet_stream_put_int(packet_stream, (int32_t)(i >> 32));
    packet_stream_put_int(packet_stream, (int32_t)i);
}

void packet_stream_put_string(PacketStream *packet_stream, char *s) {
    packet_stream_put_bytes(packet_stream, (int8_t *)s, 0, strlen(s));
}

#ifdef REVISION_177
void packet_stream_put_password(PacketStream *packet_stream, int session_id,
                                char *password) {
    int8_t encoded[15];

    int password_length = strlen(password);
    int password_index = 0;

    struct bn exponent;
    bignum_init(&exponent);
    bignum_from_string(&exponent, rsa_exponent, strlen(rsa_exponent));

    struct bn modulus;
    bignum_init(&modulus);
    bignum_from_string(&modulus, rsa_modulus, strlen(rsa_modulus));

    while (password_index < password_length) {
        encoded[0] = (int8_t)(1 + ((float)rand() / (float)RAND_MAX) * 127);
        encoded[1] = (int8_t)(((float)rand() / (float)RAND_MAX) * 256);
        encoded[2] = (int8_t)(((float)rand() / (float)RAND_MAX) * 256);
        encoded[3] = (int8_t)(((float)rand() / (float)RAND_MAX) * 256);

        write_unsigned_int(encoded, 4, session_id);

        for (int i = 0; i < 7; i++) {
            if (password_index + i >= password_length) {
                encoded[8 + i] = 32;
            } else {
                encoded[8 + i] = (int8_t)password[password_index + i];
            }
        }

        password_index += 7;

        struct bn input;
        bignum_init(&input);

        for (int i = 0; i < 15; i++) {
            input.array[i] = (uint8_t)encoded[14 - i];
        }

        struct bn result;
        bignum_init(&result);

        bignum_pow_mod(&input, &exponent, &modulus, &result);

        int result_length = 0;

        for (int i = 127; i >= 0; i--) {
            if (result.array[i] != 0) {
                result_length = i + 1;
                break;
            }
        }

        packet_stream_put_byte(packet_stream, result_length);

        for (int i = result_length - 1; i >= 0; i--) {
            packet_stream_put_byte(packet_stream, result.array[i]);
        }
    }
}
#endif

int packet_stream_get_byte(PacketStream *packet_stream) {
    return packet_stream_read_byte(packet_stream);
}

int packet_stream_get_short(PacketStream *packet_stream) {
    int i = packet_stream_get_byte(packet_stream);
    int j = packet_stream_get_byte(packet_stream);

    return i * 256 + j;
}

int packet_stream_get_int(PacketStream *packet_stream) {
    int i = packet_stream_get_short(packet_stream);
    int j = packet_stream_get_short(packet_stream);

    return i * 65536 + j;
}

int64_t packet_stream_get_long(PacketStream *packet_stream) {
    int i = packet_stream_get_short(packet_stream);
    int j = packet_stream_get_short(packet_stream);
    int k = packet_stream_get_short(packet_stream);
    int l = packet_stream_get_short(packet_stream);

    return ((int64_t)i << 48) + ((int64_t)j << 32) + ((int64_t)k << 16) + l;
}

void packet_stream_close(PacketStream *packet_stream) {
#ifdef WII
    net_close(packet_stream->socket);
#else
    close(packet_stream->socket);
#endif
}
