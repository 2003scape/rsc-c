#include "packet-stream.h"

void packet_stream_new(PacketStream *packet_stream, mudclient *mud) {
    int ret;

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(mud->port);

    ret = inet_aton(mud->server, &server_addr.sin_addr);

    if (ret == 0) {
        fprintf(stderr, "inet_aton(%s) error: %d\n", mud->server, ret);
        exit(1);
    }

    packet_stream->socket = socket(AF_INET, SOCK_STREAM, 0);

    if (packet_stream->socket < 0) {
        fprintf(stderr, "socket() error: %d\n", packet_stream->socket);
        exit(1);
    }

    int set = 1;

    setsockopt(packet_stream->socket, IPPROTO_TCP, TCP_NODELAY, &set,
               sizeof(set));

    ret = connect(packet_stream->socket, (struct sockaddr *)&server_addr,
                  sizeof(server_addr));

    if (ret < 0) {
        fprintf(stderr, "connect() error: %d\n", ret);
        packet_stream->closed = 1;
    } else {
        packet_stream->closed = 0;

        ret = ioctl(packet_stream->socket, FIONBIO, &set);

        if (ret < 0) {
            fprintf(stderr, "net_ioctl() error: %d\n", ret);
        }
    }

    memset(packet_stream->packet_data, 0, 5000);
    memset(packet_stream->available_buffer, 0, 5000);

    packet_stream->delay = 0;
    packet_stream->length = 0;
    packet_stream->max_read_tries = 0;
    packet_stream->packet_end = 3;
    packet_stream->packet_start = 0;
    packet_stream->packet_max_length = 5000;
    packet_stream->socket_exception = 0;
    packet_stream->available_length = 0;
}

int packet_stream_available_bytes(PacketStream *packet_stream, int length) {
    int bytes =
        recv(packet_stream->socket, packet_stream->available_buffer, length, 0);

    if (bytes < 0) {
        bytes = 0;
    }

    packet_stream->available_length = bytes;

    if (bytes < length) {
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

        memcpy(buffer, packet_stream->available_buffer, copy_length);
        length -= copy_length;
        packet_stream->available_length -= copy_length;
    }

    int offset = 0;

    while (length > 0) {
        int bytes = recv(packet_stream->socket, buffer + offset, length, 0);

        if (bytes > 0) {
            length -= bytes;
            offset += bytes;
        } else {
            SDL_Delay(100);
        }
    }
}

void packet_stream_write_bytes(PacketStream *packet_stream, int8_t *buffer,
                               int offset, int length) {
    if (!packet_stream->closed) {
        write(packet_stream->socket, buffer + offset, length);
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
    if (packet_stream->packet_start > ((packet_stream->packet_max_length * 4) / 5)) {
        packet_stream_write_packet(packet_stream, 0);
    }

    packet_stream->packet_data[packet_stream->packet_start + 2] = opcode & 0xff;
    packet_stream->packet_data[packet_stream->packet_start + 3] = 0;
    packet_stream->packet_end = packet_stream->packet_start + 3;
}

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
    /*
    if (this.isaacOutgoing !== null) {
        let i = this.packetData[this.packetStart + 2] & 0xff;

        this.packetData[this.packetStart + 2] =
            (i + this.isaacOutgoing.getNextValue()) & 0xff;
    }*/

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
    memcpy(packet_stream->packet_data + packet_stream->packet_end, src + offset, length);
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
    packet_stream_put_int(packet_stream, (int32_t) (i >> 32));
    packet_stream_put_int(packet_stream, (int32_t) i);
}

void packet_stream_put_string(PacketStream *packet_stream, char *s) {
    packet_stream_put_bytes(packet_stream, (int8_t*) s, 0, strlen(s));
}

int packet_stream_get_byte(PacketStream *packet_stream) {
    return packet_stream_read_byte(packet_stream);
}

int packet_stream_get_short(PacketStream *packet_stream) {
    int i = packet_stream_get_byte(packet_stream);
    int j = packet_stream_get_byte(packet_stream);

    return i * 256 + j;
}

int64_t packet_stream_get_long(PacketStream *packet_stream) {
    int i = packet_stream_get_short(packet_stream);
    int j = packet_stream_get_short(packet_stream);
    int k = packet_stream_get_short(packet_stream);
    int l = packet_stream_get_short(packet_stream);

    return ((int64_t) i << 48) + ((int64_t) j << 32) + ((int64_t) k << 16) + l;
}
