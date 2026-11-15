#ifndef GAME_H
#define GAME_H

#include "game_struct.h"

#define OUTSIDE_MAP_PATH "Game/MapLoader/Maps/Map1.json"
#define INSIDE_MAP_PATH "Game/MapLoader/Maps/Temple_Interior.json"
#define PLAYER_SPRITE_PATH "Game/sprites/Characters/character_9-16.png"

#define OUTSIDE_START_X 17
#define OUTSIDE_START_Y 18
#define INSIDE_START_X 12
#define INSIDE_START_Y 12
#define OUTSIDE_RETURN_X 17
#define OUTSIDE_RETURN_Y 5

bool GameInit(Game *game);
void GameUpdate(Game *game);
void GameDraw(const Game *game);
void GameUnload(Game *game);

#endif
