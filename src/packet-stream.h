#ifndef _H_PACKET_STREAM
#define _H_PACKET_STREAM

#include <stdint.h>
#include <stdio.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

typedef struct PacketStream PacketStream;

#include "mudclient.h"

typedef struct PacketStream {
    int socket;
    int closed;
    int delay;
    int length;
    int max_read_tries;
    int8_t packet_data[5000];
    int packet_end;
    int packet_max_length;
    int packet_start;
    int read_tries;
    int socket_exception;
    char *socket_exception_message;
    int8_t available_buffer[5000];
    int available_length;
} PacketStream;

void packet_stream_new(PacketStream *packet_stream, mudclient *mud);
int packet_stream_available_bytes(PacketStream *packet_stream, int length);
void packet_stream_read_bytes(PacketStream *packet_stream, int length,
                              int8_t *buffer);
void packet_stream_write_bytes(PacketStream *packet_stream, int8_t *buffer,
                               int offset, int length);
int packet_stream_read_byte(PacketStream *packet_stream);
int packet_stream_has_packet(PacketStream *packet_stream);
int packet_stream_read_packet(PacketStream *packet_stream, int8_t *buffer);
void packet_stream_new_packet(PacketStream *packet_stream, int opcode);
void packet_stream_write_packet(PacketStream *packet_stream, int i);
void packet_stream_send_packet(PacketStream *packet_stream);
void packet_stream_put_bytes(PacketStream *packet_stream, int8_t *src,
                             int offset, int length);
void packet_stream_put_byte(PacketStream *packet_stream, int i);
void packet_stream_put_short(PacketStream *packet_stream, int i);
void packet_stream_put_int(PacketStream *packet_stream, int i);
void packet_stream_put_long(PacketStream *packet_stream, int64_t i);
void packet_stream_put_string(PacketStream *packet_stream, char *s);
int packet_stream_get_byte(PacketStream *packet_stream);
int packet_stream_get_short(PacketStream *packet_stream);
int64_t packet_stream_get_long(PacketStream *packet_stream);
void packet_stream_flush_packet(PacketStream *packet_stream);

#endif
