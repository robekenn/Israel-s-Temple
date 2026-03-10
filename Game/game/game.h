#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdlib.h>
#include "../src/raylib.h"
#include "../Character_System/CharacterSystem.h"
#include "../MapLoader/MapLoader.h"
#include "../Inventory/Inventory.h"

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

void SetHeldItem(Inventory *inventory, ItemType type);
bool GameInit(Game *game);
void GameUpdate(Game *game);
void GameDraw(const Game *game);
void GameUnload(Game *game);

#endif