#ifndef _H_EXPERIENCE_DROPS
#define _H_EXPERIENCE_DROPS

#include "../mudclient.h"

void mudclient_drop_experience(mudclient *mud, int skill_index, int experience);
void mudclient_draw_experience_drops(mudclient *mud);

#endif
