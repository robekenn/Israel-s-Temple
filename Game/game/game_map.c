#include "game_internal.h"
#include <string.h>
#include <stdio.h>

bool SwitchMap(Game *game, MapType newMapType)
{
    UnloadTileMap(&game->map);

    if (!LoadMapForType(game, newMapType))
        return false;

    game->currentMapType = newMapType;

    ApplyPersistentMapState(game);

    if (newMapType == MAP_INSIDE)
        SetPlayerToTileCenter(&game->player, &game->map, INSIDE_START_X, INSIDE_START_Y);
    else
        SetPlayerToTileCenter(&game->player, &game->map, OUTSIDE_RETURN_X, OUTSIDE_RETURN_Y);

    return true;
}

bool LoadMapForType(Game *game, MapType mapType)
{
    if (mapType == MAP_OUTSIDE)
        return LoadTileMap(OUTSIDE_MAP_PATH, &game->map);

    return LoadTileMap(INSIDE_MAP_PATH, &game->map);
}

void ApplyPersistentMapState(Game *game)
{
    if (game == NULL)
        return;

    if (game->currentMapType == MAP_INSIDE && game->incenseAltarLit)
    {
        bool changed = SetLayerTileAtXY(
            &game->map,
            "TempleCol",
            INCENSE_ALTAR_TILE_X,
            INCENSE_ALTAR_TILE_Y,
            INCENSE_ALTAR_LIT_GID
        );

        printf("Reapply incense altar state: changed=%d\n", changed ? 1 : 0);
    }

    if (game->currentMapType == MAP_INSIDE)
        ApplyMenorahVisual(game);

    if (game->currentMapType == MAP_OUTSIDE)
        ApplyBronzeAltarVisual(game);
}

MapLayer *FindLayer(TileMap *map, const char *layerName)
{
    if (map == NULL || layerName == NULL)
        return NULL;

    for (int i = 0; i < map->layerCount; i++)
    {
        if (strcmp(map->layers[i].name, layerName) == 0)
            return &map->layers[i];
    }

    return NULL;
}

bool SetLayerTileAtXY(TileMap *map, const char *layerName, int tileX, int tileY, int gid)
{
    if (map == NULL || layerName == NULL)
        return false;

    MapLayer *layer = FindLayer(map, layerName);
    if (layer == NULL || layer->tiles == NULL)
        return false;

    if (tileX < 0 || tileX >= map->width || tileY < 0 || tileY >= map->height)
        return false;

    int index = tileY * map->width + tileX;
    layer->tiles[index] = gid;
    return true;
}