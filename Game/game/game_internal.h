#ifndef GAME_INTERNAL_H
#define GAME_INTERNAL_H

#include "game.h"
#include "../Interactions/InteractionHandler.h"

/* player helpers */
void InitializePlayer(Game *game);
Vector2 GetPlayerInteractionPoint(Player player, float drawScale);

/* map helpers */
bool SwitchMap(Game *game, MapType newMapType);
bool LoadMapForType(Game *game, MapType mapType);
void ApplyPersistentMapState(Game *game);

MapLayer *FindLayer(TileMap *map, const char *layerName);
bool SetLayerTileAtXY(TileMap *map, const char *layerName, int tileX, int tileY, int gid);

/* ui helpers */
void DrawInteractionPrompt(const Game *game);

#endif