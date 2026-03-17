#ifndef GAME_STRUCT_H
#define GAME_STRUCT_H

#include <stdbool.h>
#include "../src/raylib.h"
#include "../Character_System/CharacterSystem.h"
#include "../MapLoader/MapLoader.h"
#include "../Inventory/Inventory.h"
#include "../Interactions/Storage/Storage.h"

typedef enum MapType {
    MAP_OUTSIDE = 0,
    MAP_INSIDE
} MapType;

typedef enum MenorahStage {
    MENORAH_STAGE_UNPREPARED = 0,
    MENORAH_STAGE_BLADE_DONE = 1,
    MENORAH_STAGE_OIL_DONE = 2,
    MENORAH_STAGE_LIT = 3
} MenorahStage;

typedef struct Game {
    TileMap map;
    Player player;

    Texture2D spriteSheet;
    Texture2D inventoryUITexture;
    Texture2D itemSpriteSheet;

    Inventory inventory;
    Storage storage;

    MapType currentMapType;
    float playerScale;

    bool incenseCoalPlaced;
    bool incenseAltarLit;

    MenorahStage menorahStage;

    bool bronzeAltarLit;
    bool bronzeAltarReadyForWood;

    bool showExitMenu;
    bool shouldCloseGame;
} Game;

#endif