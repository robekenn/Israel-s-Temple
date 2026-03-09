#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdlib.h>
#include "../src/raylib.h"
#include "../Character_System/CharacterSystem.h"
#include "../MapLoader/MapLoader.h"

typedef enum MapType
{
    MAP_OUTSIDE,
    MAP_INSIDE
} MapType;

typedef struct Game
{
    TileMap map;
    Player player;
    Texture2D spriteSheet;
    MapType currentMapType;
    float playerScale;
} Game;

bool GameInit(Game *game);
void GameUpdate(Game *game);
void GameDraw(const Game *game);
void GameUnload(Game *game);

#endif