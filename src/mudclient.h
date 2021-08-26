#ifndef _H_MUDCLIENT
#define _H_MUDCLIENT

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct mudclient mudclient;

#include "colours.h"
#include "game-model.h"
#include "scene.h"
#include "surface.h"
#include "utility.h"
#include "world.h"

extern char *short_skill_names[];
extern char *skill_names[];
extern char *equipment_stat_names[];
extern int experience_array[100];

void init_mudclient_global();

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
