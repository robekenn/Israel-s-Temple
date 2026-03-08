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

bool LoadTileMap(const char *jsonFilePath, TileMap *map);
void UnloadTileMap(TileMap *map);
void DrawTileMap(TileMap *map);

bool IsMapCollisionAt(TileMap *map, Vector2 worldPosition);
bool IsMapCollisionTile(TileMap *map, int tileX, int tileY);

float GetMapTileWidthOnScreen(TileMap *map);
float GetMapTileHeightOnScreen(TileMap *map);

#endif