#ifndef MENORAH_H
#define MENORAH_H

#include "../../game/game_struct.h"

#define MENORAH_TILE_X 9
#define MENORAH_TILE_Y 9

// Replace these with your actual tile gids
#define MENORAH_UNLIT_TOP_GID 75 //75 90 105
#define MENORAH_UNLIT_MIDDLE_GID 90 //75 90 105
#define MENORAH_UNLIT_BOTTOM_GID 105 //75 90 105
//#define MENORAH_BLADE_DONE_GID 501
//#define MENORAH_OIL_DONE_GID 502
#define MENORAH_LIT_TOP_GID 63 //63 78 93
#define MENORAH_LIT_MIDDLE_GID 78 //63 78 93
#define MENORAH_LIT_BOTTOM_GID 93 //63 78 93


void UseMenorah(Game *game);
void ApplyMenorahVisual(Game *game);

#endif