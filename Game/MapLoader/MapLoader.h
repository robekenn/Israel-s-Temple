#ifndef MAPLOADER_H
#define MAPLOADER_H

#include "raylib.h"
#include <stdbool.h>

#define MAX_MAP_LAYERS 16
#define MAX_TILESETS 16

typedef struct MapLayer {
    char name[64];
    int *tiles;
    bool visible;
} MapLayer;

typedef struct MapTileset {
    int firstgid;
    int columns;
    int tileWidth;
    int tileHeight;
    Texture2D texture;
} MapTileset;

typedef struct TileMap {
    int width;
    int height;
    int tileWidth;
    int tileHeight;

    int layerCount;
    MapLayer layers[MAX_MAP_LAYERS];

    int tilesetCount;
    MapTileset tilesets[MAX_TILESETS];
} TileMap;

/* Existing */
bool LoadTileMap(const char *jsonFilePath, TileMap *map);
void UnloadTileMap(TileMap *map);
void DrawTileMap(const TileMap *map);
int GetLayerTileAtWorld(const TileMap *map, const char *layerName, Vector2 worldPosition);
int GetLayerTileAt(const TileMap *map, const char *layerName, int tileX, int tileY);
bool IsMapCollisionAt(TileMap *map, Vector2 worldPosition);
bool IsMapCollisionTile(TileMap *map, int tileX, int tileY);
float GetMapTileWidthOnScreen(const TileMap *map);
float GetMapTileHeightOnScreen(const TileMap *map);

/* Directions must match your player->direction values */
#define DIR_DOWN  0
#define DIR_LEFT  1
#define DIR_RIGHT 2
#define DIR_UP    3

/* Interaction ids */
#define INTERACT_WOOD_STACK      341
#define INTERACT_STORAGE         340
#define INTERACT_SACRIFICE_TABLE 343
#define INTERACT_LAVER           342
#define INTERACT_BRONZE_ALTAR    338
#define INTERACT_MENORAH 121
#define INTERACT_SHOWBREAD_ALTER 122
#define INTERACT_TABERNACLE_ENTRANCE 336
#define INTERACT_TABERNACLE_EXIT 107
/* keep your other ids here too, like incense altar, entrance, etc. */

/* Interaction helpers */
void GetTileCoordsAtWorld(const TileMap *map, Vector2 worldPosition, int *tileX, int *tileY);
void GetTileInFront(int playerTileX, int playerTileY, int direction, int *tileX, int *tileY);
bool IsAdjacentTile(int ax, int ay, int bx, int by);
bool IsInteractionReachableFromAnySide(const TileMap *map, Vector2 playerWorldPos, int targetTileX, int targetTileY);
bool IsInteractionReachableFromFrontOnly(const TileMap *map, Vector2 playerWorldPos, int playerDirection, int targetTileX, int targetTileY);
int GetReachableInteractionAnySide(const TileMap *map, Vector2 playerWorldPos, const char *layerName, int *outTileX, int *outTileY);
int GetInteractionInFrontOnly(const TileMap *map, Vector2 playerWorldPos, int playerDirection, const char *layerName, int *outTileX, int *outTileY);

#endif