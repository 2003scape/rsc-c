#ifndef _H_MUDCLIENT
#define _H_MUDCLIENT

#include <stdio.h>

typedef struct mudclient mudclient;

#include "game-model.h"
#include "scene.h"
#include "surface.h"
#include "utility.h"
#include "world.h"

typedef struct mudclient {
} mudclient;

void mudclient_draw_teleport_bubble(mudclient *mud, int x, int y, int width,
                                    int height, int id);
void mudclient_draw_item(mudclient *mud, int x, int y, int width, int height,
                         int id);
void mudclient_draw_player(mudclient *mud, int x, int y, int width, int height,
                           int id, int tx, int ty);
void mudclient_draw_npc(mudclient *mud, int x, int y, int width, int height,
                        int id, int tx, int ty);

int main(int argc, char **argv);

#endif
