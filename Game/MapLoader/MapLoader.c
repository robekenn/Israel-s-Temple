#include "MapLoader.h"
#include "../cJSON/cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *ReadFileTextSimple(const char *filePath);
static const MapLayer *FindLayer(const TileMap *map, const char *name);
static MapLayer *FindLayerMutable(TileMap *map, const char *name);
static const MapTileset *FindTilesetForGid(const TileMap *map, int gid);
static void DrawSingleLayer(const TileMap *map, const MapLayer *layer);


int GetLayerTileAt(const TileMap *map, const char *layerName, int tileX, int tileY){
    if (!map) return 0;

    if (tileX < 0 || tileX >= map->width || tileY < 0 || tileY >= map->height) return 0;

    const MapLayer *layer = FindLayer(map, layerName);
    if (!layer || !layer->tiles) return 0;

    int index = tileY * map->width + tileX;
    return layer->tiles[index];
}

int GetLayerTileAtWorld(const TileMap *map, const char *layerName, Vector2 worldPosition)
{
    if (!map) return 0;

    float tileWidth = GetMapTileWidthOnScreen(map);
    float tileHeight = GetMapTileHeightOnScreen(map);

    int tileX = (int)(worldPosition.x / tileWidth);
    int tileY = (int)(worldPosition.y / tileHeight);

    return GetLayerTileAt(map, layerName, tileX, tileY);
}

static char *ReadFileTextSimple(const char *filePath)
{
    FILE *fp = fopen(filePath, "rb");
    if (!fp) {
        printf("Failed to open file: %s\n", filePath);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    rewind(fp);

    char *buffer = (char *)malloc(length + 1);
    if (!buffer) {
        fclose(fp);
        printf("Failed to allocate memory for file: %s\n", filePath);
        return NULL;
    }

    size_t readSize = fread(buffer, 1, length, fp);
    buffer[readSize] = '\0';

    fclose(fp);
    return buffer;
}

static const MapLayer *FindLayer(const TileMap *map, const char *name)
{
    if (!map || !name) return NULL;

    for (int i = 0; i < map->layerCount; i++) {
        if (strcmp(map->layers[i].name, name) == 0) {
            return &map->layers[i];
        }
    }

    return NULL;
}

static MapLayer *FindLayerMutable(TileMap *map, const char *name)
{
    if (!map || !name) return NULL;

    for (int i = 0; i < map->layerCount; i++) {
        if (strcmp(map->layers[i].name, name) == 0) {
            return &map->layers[i];
        }
    }

    return NULL;
}

static const MapTileset *FindTilesetForGid(const TileMap *map, int gid)
{
    if (!map) return NULL;

    const MapTileset *best = NULL;

    for (int i = 0; i < map->tilesetCount; i++) {
        if (gid >= map->tilesets[i].firstgid) {
            if (best == NULL || map->tilesets[i].firstgid > best->firstgid) {
                best = &map->tilesets[i];
            }
        }
    }

    return best;
}

float GetMapTileWidthOnScreen(const TileMap *map)
{
    if (!map || map->width == 0) return 0.0f;
    return (float)GetScreenWidth() / map->width;
}

float GetMapTileHeightOnScreen(const TileMap *map)
{
    if (!map || map->height == 0) return 0.0f;
    return (float)GetScreenHeight() / map->height;
}

bool LoadTileMap(const char *jsonFilePath, TileMap *map)
{
    if (!map) return false;
    memset(map, 0, sizeof(TileMap));

    char *jsonText = ReadFileTextSimple(jsonFilePath);
    if (!jsonText) return false;

    cJSON *root = cJSON_Parse(jsonText);
    free(jsonText);

    if (!root) {
        printf("Failed to parse JSON map file: %s\n", jsonFilePath);
        return false;
    }

    cJSON *width = cJSON_GetObjectItem(root, "width");
    cJSON *height = cJSON_GetObjectItem(root, "height");
    cJSON *tileWidth = cJSON_GetObjectItem(root, "tilewidth");
    cJSON *tileHeight = cJSON_GetObjectItem(root, "tileheight");
    cJSON *layers = cJSON_GetObjectItem(root, "layers");
    cJSON *tilesets = cJSON_GetObjectItem(root, "tilesets");

    if (!cJSON_IsNumber(width) || !cJSON_IsNumber(height) ||
        !cJSON_IsNumber(tileWidth) || !cJSON_IsNumber(tileHeight) ||
        !cJSON_IsArray(layers) || !cJSON_IsArray(tilesets))
    {
        printf("Map JSON missing required fields.\n");
        cJSON_Delete(root);
        return false;
    }

    map->width = width->valueint;
    map->height = height->valueint;
    map->tileWidth = tileWidth->valueint;
    map->tileHeight = tileHeight->valueint;

    int tilesetCount = cJSON_GetArraySize(tilesets);
    for (int i = 0; i < tilesetCount && map->tilesetCount < MAX_TILESETS; i++) {
        cJSON *tileset = cJSON_GetArrayItem(tilesets, i);

        cJSON *image = cJSON_GetObjectItem(tileset, "image");
        if (!cJSON_IsString(image)) {
            continue;
        }

        cJSON *firstgid = cJSON_GetObjectItem(tileset, "firstgid");
        cJSON *columns = cJSON_GetObjectItem(tileset, "columns");
        cJSON *tsTileWidth = cJSON_GetObjectItem(tileset, "tilewidth");
        cJSON *tsTileHeight = cJSON_GetObjectItem(tileset, "tileheight");

        if (!cJSON_IsNumber(firstgid) || !cJSON_IsNumber(columns) ||
            !cJSON_IsNumber(tsTileWidth) || !cJSON_IsNumber(tsTileHeight))
        {
            continue;
        }

        MapTileset *dst = &map->tilesets[map->tilesetCount];

        dst->firstgid = firstgid->valueint;
        dst->columns = columns->valueint;
        dst->tileWidth = tsTileWidth->valueint;
        dst->tileHeight = tsTileHeight->valueint;
        dst->texture = LoadTexture(image->valuestring);

        if (dst->texture.id == 0) {
            printf("Failed to load tileset image: %s\n", image->valuestring);
            cJSON_Delete(root);
            UnloadTileMap(map);
            return false;
        }

        SetTextureFilter(dst->texture, TEXTURE_FILTER_POINT);
        map->tilesetCount++;
    }

    int layerCount = cJSON_GetArraySize(layers);
    for (int i = 0; i < layerCount && map->layerCount < MAX_MAP_LAYERS; i++) {
        cJSON *layer = cJSON_GetArrayItem(layers, i);

        cJSON *type = cJSON_GetObjectItem(layer, "type");
        cJSON *name = cJSON_GetObjectItem(layer, "name");
        cJSON *visible = cJSON_GetObjectItem(layer, "visible");
        cJSON *data = cJSON_GetObjectItem(layer, "data");

        if (!cJSON_IsString(type) || strcmp(type->valuestring, "tilelayer") != 0) {
            continue;
        }

        if (!cJSON_IsString(name) || !cJSON_IsArray(data)) {
            continue;
        }

        int expectedCount = map->width * map->height;
        int actualCount = cJSON_GetArraySize(data);

        if (actualCount != expectedCount) {
            printf("Layer '%s' tile count mismatch.\n", name->valuestring);
            cJSON_Delete(root);
            UnloadTileMap(map);
            return false;
        }

        MapLayer *dst = &map->layers[map->layerCount];
        strncpy(dst->name, name->valuestring, sizeof(dst->name) - 1);
        dst->name[sizeof(dst->name) - 1] = '\0';
        dst->visible = !visible || cJSON_IsTrue(visible);

        dst->tiles = (int *)malloc(sizeof(int) * expectedCount);
        if (!dst->tiles) {
            printf("Failed to allocate layer memory.\n");
            cJSON_Delete(root);
            UnloadTileMap(map);
            return false;
        }

        for (int j = 0; j < expectedCount; j++) {
            cJSON *tile = cJSON_GetArrayItem(data, j);
            dst->tiles[j] = cJSON_IsNumber(tile) ? tile->valueint : 0;
        }

        map->layerCount++;
    }

    cJSON_Delete(root);
    return true;
}

void UnloadTileMap(TileMap *map)
{
    if (!map) return;

    for (int i = 0; i < map->layerCount; i++) {
        if (map->layers[i].tiles) {
            free(map->layers[i].tiles);
            map->layers[i].tiles = NULL;
        }
    }

    for (int i = 0; i < map->tilesetCount; i++) {
        if (map->tilesets[i].texture.id != 0) {
            UnloadTexture(map->tilesets[i].texture);
            map->tilesets[i].texture.id = 0;
        }
    }

    map->layerCount = 0;
    map->tilesetCount = 0;
}

static void DrawSingleLayer(const TileMap *map, const MapLayer *layer)
{
    if (!map || !layer || !layer->tiles || !layer->visible) return;

    float tileWidthOnScreen = GetMapTileWidthOnScreen(map);
    float tileHeightOnScreen = GetMapTileHeightOnScreen(map);

    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            int index = y * map->width + x;
            int gid = layer->tiles[index];

            if (gid == 0) continue;

            const MapTileset *tileset = FindTilesetForGid(map, gid);
            if (!tileset) continue;

            int localTile = gid - tileset->firstgid;
            if (localTile < 0) continue;

            int tileX = localTile % tileset->columns;
            int tileY = localTile / tileset->columns;

            Rectangle source = {
                (float)(tileX * tileset->tileWidth),
                (float)(tileY * tileset->tileHeight),
                (float)tileset->tileWidth,
                (float)tileset->tileHeight
            };

            Rectangle dest = {
                x * tileWidthOnScreen,
                y * tileHeightOnScreen,
                tileWidthOnScreen,
                tileHeightOnScreen
            };

            DrawTexturePro(
                tileset->texture,
                source,
                dest,
                (Vector2){0.0f, 0.0f},
                0.0f,
                WHITE
            );
        }
    }
}

void DrawTileMap(const TileMap *map)
{
    if (!map) return;

    const MapLayer *ground = FindLayer(map, "Ground");
    const MapLayer *noCol = FindLayer(map, "Temple_NoCol");
    const MapLayer *col = FindLayer(map, "TempleCol");

    if (ground) DrawSingleLayer(map, ground);
    if (noCol) DrawSingleLayer(map, noCol);
    if (col) DrawSingleLayer(map, col);
}

bool IsMapCollisionTile(TileMap *map, int tileX, int tileY)
{
    if (!map) return false;

    if (tileX < 0 || tileX >= map->width || tileY < 0 || tileY >= map->height) {
        return true;
    }

    MapLayer *col = FindLayerMutable(map, "TempleCol");
    if (!col || !col->tiles) return false;

    int index = tileY * map->width + tileX;
    return col->tiles[index] != 0;
}

bool IsMapCollisionAt(TileMap *map, Vector2 worldPosition)
{
    if (!map) return false;

    float tileWidthOnScreen = GetMapTileWidthOnScreen(map);
    float tileHeightOnScreen = GetMapTileHeightOnScreen(map);

    int tileX = (int)(worldPosition.x / tileWidthOnScreen);
    int tileY = (int)(worldPosition.y / tileHeightOnScreen);

    return IsMapCollisionTile(map, tileX, tileY);
}