#include "packet-stream.h"

#if 0
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
    /*packet_stream->decode_key = 3141592;
    packet_stream->encode_key = 3141592;*/
#endif

#ifndef NO_RSA
    packet_stream->rsa_exponent = mud->options->rsa_exponent;
    packet_stream->rsa_modulus = mud->options->rsa_modulus;
#endif

    int ret = 0;

#ifdef WII
    char local_ip[16] = {0};
    char gateway[16] = {0};
    char netmask[16] = {0};

    ret = if_config(local_ip, netmask, gateway, TRUE, 20);

    if (ret < 0) {
        fprintf(stderr, "if_config(): %d\n", ret);
        exit(1);
    }
#endif

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(mud->options->port);

    char server_ip[16] = {0};

    if (is_ip_address(mud->options->server)) {
        strcpy(server_ip, mud->options->server);
    } else {
#ifdef WII
        struct hostent *host_addr = net_gethostbyname(mud->options->server);

        struct in_addr addr = {0};
        memcpy(&addr, host_addr->h_addr_list[0], sizeof(struct in_addr));
        strcpy(server_ip, inet_ntoa(addr));
#else
        struct addrinfo hints = {0};
        struct addrinfo *result = {0};

        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        int status = getaddrinfo(mud->options->server, NULL, &hints, &result);

        if (status != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
            exit(1);
        }

        for (struct addrinfo *rp = result; rp != NULL; rp = rp->ai_next) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)rp->ai_addr;

            inet_ntop(rp->ai_family, &(ipv4->sin_addr), server_ip,
                      sizeof(server_ip));

            if (strlen(server_ip)) {
                break;
            }
        }

        freeaddrinfo(result);
#endif
    }

#ifdef WIN32
    WSADATA wsa_data = {0};
    ret = WSAStartup(MAKEWORD(2, 2), &wsa_data);

    if (ret != 0) {
        fprintf(stderr, "WSAStartup: %d\n", ret);
        exit(1);
    }

    ret = InetPton(AF_INET, server_ip, &server_addr.sin_addr);
#else
    ret = inet_aton(server_ip, &server_addr.sin_addr);
#endif

    if (ret == 0) {
        fprintf(stderr, "inet_aton(%s) error: %d\n", mud->options->server, ret);
        exit(1);
    }

#ifdef WII
    packet_stream->socket = net_socket(AF_INET, SOCK_STREAM, 0);
#else
    #ifdef __SWITCH__
        socketInitializeDefault();              // Initialize sockets
    #endif
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
    setsockopt(packet_stream->socket, IPPROTO_TCP, TCP_NODELAY, (char *)&set,
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
#elif defined(WIN32) || defined(__SWITCH__)
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

void packet_stream_new_packet(PacketStream *packet_stream, CLIENT_OPCODE opcode) {
#if 0
    packet_stream->opcode_friend = get_client_opcode_friend(opcode);
#endif

    if (packet_stream->packet_start >
        ((packet_stream->packet_max_length * 4) / 5)) {
        packet_stream_write_packet(packet_stream, 0);
    }

#ifndef NO_ISAAC
    if (packet_stream->isaac_ready) {
        opcode = opcode + isaac_next(&packet_stream->isaac_out);
    }
#endif

    packet_stream->packet_data[packet_stream->packet_start + 2] = opcode & 0xff;
    packet_stream->packet_data[packet_stream->packet_start + 3] = 0;
    packet_stream->packet_end = packet_stream->packet_start + 3;
}

/*int packet_stream_decode_opcode(PacketStream *packet_stream, int opcode) {
    int index = (opcode - packet_stream->decode_key) & 255;
    int decoded_opcode = OPCODE_ENCRYPTION[index];

    packet_stream->decode_threat_index =
        (packet_stream->decode_threat_index + decoded_opcode) % THREAT_LENGTH;

    char threat_character = SPOOKY_THREAT[packet_stream->decode_threat_index];

    packet_stream->decode_key = (packet_stream->decode_key * 3 +
                                 (int)threat_character + decoded_opcode) &
                                0xffff;

    return decoded_opcode;
}*/

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
#if 0
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

void packet_stream_put_bytes(PacketStream *packet_stream, void *src,
                             int offset, int length) {
    uint8_t *p = src;

    memcpy(packet_stream->packet_data + packet_stream->packet_end, p + offset,
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

#ifndef NO_RSA
static void packet_stream_put_rsa(PacketStream *packet_stream,
                                  const void *input, size_t input_len) {
    const uint8_t *unencoded = input;

    struct bn exponent = {0};
    bignum_init(&exponent);

    bignum_from_string(&exponent, packet_stream->rsa_exponent,
                       strlen(packet_stream->rsa_exponent));

    struct bn modulus = {0};
    bignum_init(&modulus);

    bignum_from_string(&modulus, packet_stream->rsa_modulus,
                       strlen(packet_stream->rsa_modulus));

    struct bn bn = {0};
    bignum_init(&bn);
    for (size_t i = 0; i < input_len; i++) {
        bn.array[i] = unencoded[input_len - 1 - i];
    }

    struct bn result = {0};
    bignum_init(&result);
    bignum_pow_mod(&bn, &exponent, &modulus, &result);

    int result_length = 0;

    for (int i = (BN_ARRAY_SIZE - 1); i >= 0; i--) {
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
#endif

#ifndef REVISION_177
void packet_stream_put_login_block(PacketStream *packet_stream,
                const char *username, const char *password,
                uint32_t *isaac_keys, uint32_t uuid)
{
    uint8_t input_block[16 + (sizeof(uint32_t) * 4) + 4 +
                        USERNAME_LENGTH + PASSWORD_LENGTH];
    size_t username_len = strlen(username);
    size_t password_len = strlen(password);
    uint8_t *p = input_block;

#if !defined(NO_ISAAC) || !defined(NO_RSA)
    *(p++) = '\n'; /* Magic for sanity checks by the server. */
#endif

#ifndef NO_ISAAC
    memset(packet_stream->isaac_in.randrsl, 0,
           sizeof(packet_stream->isaac_in.randrsl));
    memset(packet_stream->isaac_out.randrsl, 0,
           sizeof(packet_stream->isaac_out.randrsl));
#endif

    for (unsigned int i = 0; i < 4; ++i) {
#ifndef NO_ISAAC
        packet_stream->isaac_in.randrsl[i] = isaac_keys[i];
        packet_stream->isaac_out.randrsl[i] = isaac_keys[i];
#endif
        write_unsigned_int(p, 0, isaac_keys[i]);
        p += 4;
    }

    write_unsigned_int(p, 0, uuid);
    p += 4;

    memcpy(p, username, username_len);
    p += username_len;
    *(p++) = '\n';

    memcpy(p, password, password_len);
    p += password_len;
    *(p++) = '\n';

#ifndef NO_ISAAC
    isaac_init(&packet_stream->isaac_in, 1);
    isaac_init(&packet_stream->isaac_out, 1);
    packet_stream->isaac_ready = 1;
#endif

#ifndef NO_RSA
    packet_stream_put_rsa(packet_stream, input_block, p - input_block);
#else
    packet_stream_put_byte(packet_stream, p - input_block);
    packet_stream_put_bytes(packet_stream, input_block, 0, p - input_block);
#endif
}
#endif

#ifdef REVISION_177
void packet_stream_put_password(PacketStream *packet_stream, int session_id,
                                char *password) {
    int8_t encoded[15] = {0};

    int password_length = strlen(password);
    int password_index = 0;

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
#ifndef NO_RSA
        packet_stream_put_rsa(packet_stream, encoded, sizeof(encoded));
#else
        packet_stream_put_byte(packet_stream, sizeof(encoded));
        packet_stream_put_bytes(packet_stream, encoded, 0, sizeof(encoded));
#endif
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
