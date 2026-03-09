#include "game.h"

#define OUTSIDE_MAP_PATH "Game/MapLoader/Maps/Map1.json"
#define INSIDE_MAP_PATH  "Game/MapLoader/Maps/Temple_Interior.json"

#define OUTSIDE_START_X 17
#define OUTSIDE_START_Y 18

#define INSIDE_START_X  12
#define INSIDE_START_Y  12

#define OUTSIDE_RETURN_X 17
#define OUTSIDE_RETURN_Y 5

#define PLAYER_SPRITE_PATH "Game/sprites/Characters/character_9-16.png"

static bool LoadMapForType(Game *game, MapType mapType);
static void InitializePlayer(Game *game);
static void HandleInteraction(Game *game);
static bool SwitchMap(Game *game, MapType newMapType);
static void DrawInteractionPrompt(const Game *game);

bool GameInit(Game *game)
{
    if (game == NULL)
        return false;

    game->currentMapType = MAP_OUTSIDE;
    game->playerScale = 3.5f;

    if (!LoadTileMap(OUTSIDE_MAP_PATH, &game->map))
        return false;

    game->spriteSheet = LoadTexture(PLAYER_SPRITE_PATH);
    SetTextureFilter(game->spriteSheet, TEXTURE_FILTER_POINT);

    InitializePlayer(game);
    SetPlayerToTileCenter(&game->player, &game->map, OUTSIDE_START_X, OUTSIDE_START_Y);

    return true;
}

void GameUpdate(Game *game)
{
    if (game == NULL)
        return;

    UpdatePlayer(&game->player, &game->map, game->playerScale);
    HandleInteraction(game);
}

void GameDraw(const Game *game)
{
    if (game == NULL)
        return;

    DrawTileMap(&game->map);
    DrawPlayer(game->player, game->spriteSheet, game->playerScale);
    DrawInteractionPrompt(game);
}

void GameUnload(Game *game)
{
    if (game == NULL)
        return;

    UnloadTileMap(&game->map);
    UnloadTexture(game->spriteSheet);
}

static void InitializePlayer(Game *game)
{
    game->player = (Player){0};
    game->player.characterIndex = 0;
    game->player.direction = 0;
    game->player.animFrame = 1;
    game->player.animTimer = 0.0f;
    game->player.speed = 120.0f;
}

static void HandleInteraction(Game *game)
{
    int interactionGid = GetLayerTileAtWorld(&game->map, "Interactions", game->player.position);

    if (interactionGid == 0 || !IsKeyPressed(KEY_E))
        return;

    if (game->currentMapType == MAP_OUTSIDE)
    {
        SwitchMap(game, MAP_INSIDE);
    }
    else if (game->currentMapType == MAP_INSIDE)
    {
        SwitchMap(game, MAP_OUTSIDE);
    }
}

static bool SwitchMap(Game *game, MapType newMapType)
{
    UnloadTileMap(&game->map);

    if (!LoadMapForType(game, newMapType))
        return false;

    game->currentMapType = newMapType;

    if (newMapType == MAP_INSIDE)
    {
        SetPlayerToTileCenter(&game->player, &game->map, INSIDE_START_X, INSIDE_START_Y);
    }
    else
    {
        SetPlayerToTileCenter(&game->player, &game->map, OUTSIDE_RETURN_X, OUTSIDE_RETURN_Y);
    }

    return true;
}

static bool LoadMapForType(Game *game, MapType mapType)
{
    if (mapType == MAP_OUTSIDE)
        return LoadTileMap(OUTSIDE_MAP_PATH, &game->map);
    else
        return LoadTileMap(INSIDE_MAP_PATH, &game->map);
}

static void DrawInteractionPrompt(const Game *game)
{
    int interactionGid = GetLayerTileAtWorld(&game->map, "Interactions", game->player.position);

    if (interactionGid != 0)
    {
        DrawText("Press E", 20, 20, 30, WHITE);
    }
}