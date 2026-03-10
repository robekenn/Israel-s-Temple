#include "game.h"
#include <string.h>
#include <stdio.h>

#define OUTSIDE_MAP_PATH "Game/MapLoader/Maps/Map1.json"
#define INSIDE_MAP_PATH  "Game/MapLoader/Maps/Temple_Interior.json"

#define OUTSIDE_START_X 17
#define OUTSIDE_START_Y 18

#define INSIDE_START_X  12
#define INSIDE_START_Y  12

#define OUTSIDE_RETURN_X 17
#define OUTSIDE_RETURN_Y 5

#define PLAYER_SPRITE_PATH "Game/sprites/Characters/character_9-16.png"

// Confirmed interaction IDs
#define INTERACT_INCENSE_ALTAR  109
#define INTERACT_BRONZE_ALTAR   338

// These may still need adjusting
#define INCENSE_ALTAR_TILE_X 12
#define INCENSE_ALTAR_TILE_Y 5
#define INCENSE_ALTAR_LIT_GID 98

static bool LoadMapForType(Game *game, MapType mapType);
static void InitializePlayer(Game *game);
static void HandleInteraction(Game *game);
static bool SwitchMap(Game *game, MapType newMapType);
static void DrawInteractionPrompt(const Game *game);
static void ApplyPersistentMapState(Game *game);

static MapLayer *FindLayer(TileMap *map, const char *layerName);
static bool SetLayerTileAtXY(TileMap *map, const char *layerName, int tileX, int tileY, int gid);

bool GameInit(Game *game)
{
    if (game == NULL)
        return false;

    memset(game, 0, sizeof(*game));

    game->currentMapType = MAP_OUTSIDE;
    game->playerScale = 3.5f;
    game->incenseAltarLit = false;

    if (!LoadTileMap(OUTSIDE_MAP_PATH, &game->map))
        return false;

    game->spriteSheet = LoadTexture(PLAYER_SPRITE_PATH);
    SetTextureFilter(game->spriteSheet, TEXTURE_FILTER_POINT);

    game->inventoryUITexture = LoadTexture("Game/sprites/UI/Inventory.png");
    game->itemSpriteSheet = LoadTexture("Game/sprites/Items/Items.png");

    SetTextureFilter(game->inventoryUITexture, TEXTURE_FILTER_POINT);
    SetTextureFilter(game->itemSpriteSheet, TEXTURE_FILTER_POINT);

    InitInventory(&game->inventory);

    AddItemToInventory(&game->inventory, ITEM_CENSER);

    InitializePlayer(game);
    SetPlayerToTileCenter(&game->player, &game->map, OUTSIDE_START_X, OUTSIDE_START_Y);

    ApplyPersistentMapState(game);

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

    DrawInventoryUI(
        &game->inventory,
        game->inventoryUITexture,
        game->itemSpriteSheet
    );

    DrawText(TextFormat("Held: %s", GetItemName(game->inventory.heldItem)), 20, 60, 20, WHITE);
    DrawText(TextFormat("Incense lit: %s", game->incenseAltarLit ? "true" : "false"), 20, 85, 20, WHITE);
    DrawText(TextFormat("Map: %s", game->currentMapType == MAP_OUTSIDE ? "OUTSIDE" : "INSIDE"), 20, 110, 20, WHITE);
}

void GameUnload(Game *game)
{
    if (game == NULL)
        return;

    UnloadTileMap(&game->map);
    UnloadTexture(game->spriteSheet);
    UnloadTexture(game->inventoryUITexture);
    UnloadTexture(game->itemSpriteSheet);
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

    if (!IsKeyPressed(KEY_E))
        return;

    if (interactionGid == INTERACT_BRONZE_ALTAR)
    {
        if (HasHeldItem(&game->inventory, ITEM_CENSER))
        {
            SetHeldItem(&game->inventory, ITEM_CENSER_COAL);
            printf("Bronze altar used: censer -> censer coal\n");
        }
        return;
    }

    if (interactionGid == INTERACT_INCENSE_ALTAR)
    {
        if (HasHeldItem(&game->inventory, ITEM_CENSER_COAL))
        {
            bool changed = SetLayerTileAtXY(
                &game->map,
                "TempleCol",
                INCENSE_ALTAR_TILE_X,
                INCENSE_ALTAR_TILE_Y,
                INCENSE_ALTAR_LIT_GID
            );

            printf("Incense altar used. changed=%d\n", changed ? 1 : 0);

            if (changed)
            {
                game->incenseAltarLit = true;
                SetHeldItem(&game->inventory, ITEM_CENSER);
                printf("Incense altar lit and saved in memory\n");
            }
        }
        return;
    }

    if (interactionGid == 0)
        return;

    if (game->currentMapType == MAP_OUTSIDE)
        SwitchMap(game, MAP_INSIDE);
    else
        SwitchMap(game, MAP_OUTSIDE);
}

static bool SwitchMap(Game *game, MapType newMapType)
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

static bool LoadMapForType(Game *game, MapType mapType)
{
    if (mapType == MAP_OUTSIDE)
        return LoadTileMap(OUTSIDE_MAP_PATH, &game->map);
    else
        return LoadTileMap(INSIDE_MAP_PATH, &game->map);
}

static void ApplyPersistentMapState(Game *game)
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
}

static void DrawInteractionPrompt(const Game *game)
{
    int interactionGid = GetLayerTileAtWorld(&game->map, "Interactions", game->player.position);

    if (interactionGid != 0)
        DrawText("Press E", 20, 20, 30, WHITE);
}

static MapLayer *FindLayer(TileMap *map, const char *layerName)
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

static bool SetLayerTileAtXY(TileMap *map, const char *layerName, int tileX, int tileY, int gid)
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