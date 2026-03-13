#ifndef GAME_STRUCT_H
#define GAME_STRUCT_H

#include <stdbool.h>
#include "../src/raylib.h"
#include "../Character_System/CharacterSystem.h"
#include "../MapLoader/MapLoader.h"
#include "../Inventory/Inventory.h"


#define OUTSIDE_MAP_PATH "Game/MapLoader/Maps/Map1.json"
#define INSIDE_MAP_PATH  "Game/MapLoader/Maps/Temple_Interior.json"

#define OUTSIDE_START_X 17
#define OUTSIDE_START_Y 18

#define INSIDE_START_X  12
#define INSIDE_START_Y  12

#define OUTSIDE_RETURN_X 17
#define OUTSIDE_RETURN_Y 5

#define PLAYER_SPRITE_PATH "Game/sprites/Characters/character_9-16.png"

// Confirmed interaction IDs
#define INTERACT_INCENSE_ALTAR  109
#define INTERACT_BRONZE_ALTAR   338

// These may still need adjusting
#define INCENSE_ALTAR_TILE_X 12
#define INCENSE_ALTAR_TILE_Y 5
#define INCENSE_ALTAR_LIT_GID 98

typedef enum MapType {
    MAP_OUTSIDE = 0,
    MAP_INSIDE
} MapType;

typedef struct Game {
    TileMap map;
    Player player;
    Texture2D spriteSheet;
    Texture2D inventoryUITexture;
    Texture2D itemSpriteSheet;
    Inventory inventory;
    MapType currentMapType;
    float playerScale;
    bool incenseAltarLit;
} Game;

#endif