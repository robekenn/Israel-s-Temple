#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdlib.h>
#include "../src/raylib.h"
#include "../Character_System/CharacterSystem.h"
#include "../MapLoader/MapLoader.h"
#include "../Inventory/Inventory.h"
#include "game_struct.h"
#include "../Interactions/Bronze_Altar/Bronze_Altar.h"
#include "../Interactions/Incense_Altar/Incense_Altar.h"
#include "../Interactions/Wood_Stack/Wood_Stack.h"
#include "../Interactions/Storage/Storage.h"

bool LoadMapForType(Game *game, MapType mapType);
void InitializePlayer(Game *game);
void HandleInteraction(Game *game);
bool SwitchMap(Game *game, MapType newMapType);
void DrawInteractionPrompt(const Game *game);
void ApplyPersistentMapState(Game *game);

MapLayer *FindLayer(TileMap *map, const char *layerName);
bool SetLayerTileAtXY(TileMap *map, const char *layerName, int tileX, int tileY, int gid);

void SetHeldItem(Inventory *inventory, ItemType type);
bool GameInit(Game *game);
void GameUpdate(Game *game);
void GameDraw(const Game *game);
void GameUnload(Game *game);

#endif