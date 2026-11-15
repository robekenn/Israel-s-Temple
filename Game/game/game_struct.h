#ifndef GAME_STRUCT_H
#define GAME_STRUCT_H

#include "../Character_System/CharacterSystem.h"
#include "../Inventory/Inventory.h"
#include "../Interactions/Storage/Storage.h"
#include "../Campaign/campaign.h"

typedef enum MapType {
    MAP_OUTSIDE,
    MAP_INSIDE
} MapType;

typedef enum GameScreen {
    SCREEN_TITLE,
    SCREEN_PLAY,
    SCREEN_JOURNAL,
    SCREEN_PAUSE,
    SCREEN_HANDOFF,
    SCREEN_ENDING,
    SCREEN_RESTART
} GameScreen;

typedef struct Game {
    TileMap map;
    Player player;
    Texture2D spriteSheet;
    Texture2D itemSpriteSheet;
    Inventory inventory;
    Storage storage;
    Campaign campaign;
    MapType currentMapType;
    GameScreen screen;
    float playerScale;
    bool shouldCloseGame;
    bool started;
    bool hasSave;
    bool ended;
    bool saveFailed;
    char savePath[1024];
    char message[256];
} Game;

#endif
