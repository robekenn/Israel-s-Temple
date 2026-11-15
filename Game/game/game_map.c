#include "game_internal.h"

#include <math.h>

enum { MENTOR_X = 16, VISITOR_X = 18, GATE_Y = 16, CABINET_X = 14, CABINET_Y = 12 };

bool SwitchMap(Game *game, MapType type)
{
    if (game->map.layerCount == 0 || type != game->currentMapType)
    {
        TileMap next;
        if (!LoadTileMap(type == MAP_OUTSIDE ? OUTSIDE_MAP_PATH : INSIDE_MAP_PATH, &next))
            return false;

        UnloadTileMap(&game->map);
        game->map = next;
    }
    game->currentMapType = type;
    ApplyPersistentMapState(game);
    if (type == MAP_INSIDE)
        PlacePlayer(game, INSIDE_START_X, INSIDE_START_Y, DIR_UP);
    else
        PlacePlayer(game, OUTSIDE_RETURN_X, OUTSIDE_RETURN_Y, DIR_DOWN);
    return true;
}

void ApplyPersistentMapState(Game *game)
{
    if (game->currentMapType == MAP_OUTSIDE)
    {
        SetLayerTileAt(&game->map, "TempleCol", 17, 10, game->campaign.fireTended ? 240 : 255);
        return;
    }
    SetLayerTileAt(&game->map, "TempleCol", 12, 5, game->campaign.incenseLit ? 98 : 83);

    const int lit[] = {63, 78, 93};
    const int unlit[] = {75, 90, 105};
    for (int i = 0; i < 3; i++)
        SetLayerTileAt(&game->map, "TempleCol", 9, 9 + i, game->campaign.lampsLit ? lit[i] : unlit[i]);
}

int StationAt(const Game *game, int x, int y)
{
    if (game->currentMapType == MAP_OUTSIDE && y == GATE_Y)
    {
        if (x == MENTOR_X)
            return INTERACT_MENTOR;
        if (x == VISITOR_X && CampaignShift(&game->campaign)->visitor)
            return INTERACT_VISITOR;
    }
    if (game->currentMapType == MAP_INSIDE && x == CABINET_X && y == CABINET_Y)
        return INTERACT_STORAGE;
    return GetLayerTileAt(&game->map, "Interactions", x, y);
}

bool FindStation(const Game *game, int station, Vector2 *center)
{
    float best = INFINITY;
    float w = GetMapTileWidthOnScreen(&game->map);
    float h = GetMapTileHeightOnScreen(&game->map);
    Vector2 feet = GetPlayerInteractionPoint(game->player, game->playerScale);
    for (int y = 0; y < game->map.height; y++)
    {
        for (int x = 0; x < game->map.width; x++)
        {
            if (StationAt(game, x, y) != station)
                continue;
            Vector2 tile = {(x + 0.5f)*w, (y + 0.5f)*h};
            float dx = tile.x - feet.x;
            float dy = tile.y - feet.y;
            float distance = dx*dx + dy*dy;
            if (distance < best)
            {
                best = distance;
                *center = tile;
            }
        }
    }
    return best < INFINITY;
}
