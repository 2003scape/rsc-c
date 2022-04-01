#ifndef _H_PACKET_STREAM
#define _H_PACKET_STREAM

#include <stdint.h>
#include <stdio.h>

#include <errno.h>
#include <fcntl.h>

#ifdef WII
#include <network.h>
#else
#ifdef WIN32
#include <ws2tcpip.h>
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#endif
#endif

#ifdef REVISION_177
#include "bn.h"
#endif

#define PACKET_BUFFER_LENGTH 5000

#ifdef REVISION_177
extern char *SPOOKY_THREAT;
extern int THREAT_LENGTH;

extern int OPCODE_ENCRYPTION[];

extern char *rsa_exponent;
extern char *rsa_modulus;

int get_client_opcode_friend(int opcode);
#endif

typedef struct PacketStream PacketStream;

#include "mudclient.h"
#include "utility.h"

#ifdef REVISION_177
void init_packet_stream_global();
#endif

typedef struct PacketStream {
    int socket;
    int closed;
    int delay;
    int length;
    int max_read_tries;
    int8_t packet_data[PACKET_BUFFER_LENGTH];
    int packet_end;
    int packet_max_length;
    int packet_start;
    int read_tries;
    int socket_exception;
    char *socket_exception_message;
    int8_t available_buffer[PACKET_BUFFER_LENGTH];
    int available_length;
    int available_offset;

#ifdef REVISION_177
    int decode_key;
    int decode_threat_index;

    int encode_key;
    int encode_threat_index;

    int opcode_friend;

    int field_597;
#endif
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

#ifdef REVISION_177
int packet_stream_decode_opcode(PacketStream *packet_stream, int opcode);
#endif

void packet_stream_write_packet(PacketStream *packet_stream, int i);
void packet_stream_send_packet(PacketStream *packet_stream);
void packet_stream_put_bytes(PacketStream *packet_stream, int8_t *src,
                             int offset, int length);
void packet_stream_put_byte(PacketStream *packet_stream, int i);
void packet_stream_put_short(PacketStream *packet_stream, int i);
void packet_stream_put_int(PacketStream *packet_stream, int i);
void packet_stream_put_long(PacketStream *packet_stream, int64_t i);
void packet_stream_put_string(PacketStream *packet_stream, char *s);

#ifdef REVISION_177
void packet_stream_put_password(PacketStream *packet_stream, int session_id,
                                char *password);
#endif

int packet_stream_get_byte(PacketStream *packet_stream);
int packet_stream_get_short(PacketStream *packet_stream);
int packet_stream_get_int(PacketStream *packet_stream);
int64_t packet_stream_get_long(PacketStream *packet_stream);
void packet_stream_flush_packet(PacketStream *packet_stream);
void packet_stream_close(PacketStream *packet_stream);

#endif
