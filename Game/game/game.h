#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdlib.h>

#include "../src/raylib.h"
#include "../Character_System/CharacterSystem.h"
#include "../MapLoader/MapLoader.h"
#include "../Inventory/Inventory.h"
#include "../Interactions/Bronze_Altar/Bronze_Altar.h"
#include "../Interactions/Incense_Altar/Incense_Altar.h"
#include "../Interactions/Wood_Stack/Wood_Stack.h"
#include "../Interactions/Storage/Storage.h"
#include "../Interactions/Menorah/Menorah.h"
#include "../Interactions/Laver/Laver.h"
#include "game_struct.h"

/* -------------------------------------------------
   MAP PATHS
   ------------------------------------------------- */
#define OUTSIDE_MAP_PATH "Game/MapLoader/Maps/Map1.json"
#define INSIDE_MAP_PATH  "Game/MapLoader/Maps/Temple_Interior.json"

/* -------------------------------------------------
   PLAYER / SPRITES
   ------------------------------------------------- */
#define PLAYER_SPRITE_PATH "Game/sprites/Characters/character_9-16.png"

/* -------------------------------------------------
   MAP SPAWN POINTS
   ------------------------------------------------- */
#define OUTSIDE_START_X 17
#define OUTSIDE_START_Y 18

#define INSIDE_START_X  12
#define INSIDE_START_Y  12

#define OUTSIDE_RETURN_X 17
#define OUTSIDE_RETURN_Y 5

/* -------------------------------------------------
   PERSISTENT VISUAL TILE POSITIONS
   ------------------------------------------------- */
#define INCENSE_ALTAR_TILE_X 12
#define INCENSE_ALTAR_TILE_Y 5
#define INCENSE_ALTAR_LIT_GID 98

/* -------------------------------------------------
   PUBLIC GAME API
   ------------------------------------------------- */
bool GameInit(Game *game);
void GameUpdate(Game *game);
void GameDraw(const Game *game);
void GameUnload(Game *game);

#endif