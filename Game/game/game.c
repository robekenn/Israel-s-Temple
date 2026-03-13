#include "game.h"
#include <string.h>
#include <stdio.h>



/**
 * @brief This function initalizes the game vars
 *
 * This function defines the game rules, Loads textures
 *
 * @param game a pointer to the game object
 *
 * @return Returns true when the game is initalized
 */
bool GameInit(Game *game)
{
    if (game == NULL)
        return false;

    memset(game, 0, sizeof(*game));

    game->currentMapType = MAP_OUTSIDE;
    game->playerScale = 3.5f;
    game->incenseAltarLit = false; //Inits Incense Alter

    if (!LoadTileMap(OUTSIDE_MAP_PATH, &game->map))
        return false;

    game->spriteSheet = LoadTexture(PLAYER_SPRITE_PATH);
    SetTextureFilter(game->spriteSheet, TEXTURE_FILTER_POINT);

    game->inventoryUITexture = LoadTexture("Game/sprites/UI/Inventory.png");
    game->itemSpriteSheet = LoadTexture("Game/sprites/Items/Items.png");

    SetTextureFilter(game->inventoryUITexture, TEXTURE_FILTER_POINT);
    SetTextureFilter(game->itemSpriteSheet, TEXTURE_FILTER_POINT);

    InitInventory(&game->inventory);

    AddItemToInventory(&game->inventory, ITEM_CENSER); //Adds item to inventory for launch

    InitializePlayer(game);
    SetPlayerToTileCenter(&game->player, &game->map, OUTSIDE_START_X, OUTSIDE_START_Y);

    ApplyPersistentMapState(game);

    return true;
}

/**
 * @brief Updates/Calculates the changes for each frame
 *
 * This function checks for changes each frame and updates the variables connected to them.
 *
 * @param game a pointer to the game object
 *
 * @return Void
 */
void GameUpdate(Game *game)
{
    if (game == NULL)
        return;

    UpdatePlayer(&game->player, &game->map, game->playerScale); //Updates the Players position
    HandleInteraction(game); //Handles any interactions
}

/**
 * @brief Draws the game to the screen
 *
 * This function redraws the screen every frame
 *
 * @param game a pointer to the game object
 *
 * @return Void
 */
void GameDraw(const Game *game)
{
    if (game == NULL)
        return;

    DrawTileMap(&game->map); //Draws the map
    DrawPlayer(game->player, game->spriteSheet, game->playerScale); //Draws the player
    DrawInteractionPrompt(game); //Draws the interaction prompts

    DrawInventoryUI(
        &game->inventory,
        game->inventoryUITexture,
        game->itemSpriteSheet
    ); //Draws the Inventory

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

void InitializePlayer(Game *game)
{
    game->player = (Player){0};
    game->player.characterIndex = 0;
    game->player.direction = 0;
    game->player.animFrame = 1;
    game->player.animTimer = 0.0f;
    game->player.speed = 120.0f;
}

Vector2 GetPlayerInteractionPoint(Player player, float drawScale)
{
    float spriteHeight = SPRITE_HEIGHT * drawScale;

    Vector2 point = player.position;
    point.y += spriteHeight * 0.32f;

    return point;
}

void HandleInteraction(Game *game)
{
    if (!IsKeyPressed(KEY_E))
        return;

    TileMap *map = &game->map;

    Vector2 interactPoint = GetPlayerInteractionPoint(game->player, game->playerScale);

    int playerTileX = 0;
    int playerTileY = 0;
    GetTileCoordsAtWorld(map, interactPoint, &playerTileX, &playerTileY);

    /* -----------------------------------------
       1) FRONT-ONLY INTERACTIONS
       ----------------------------------------- */
    int frontTileX = playerTileX;
    int frontTileY = playerTileY;
    GetTileInFront(playerTileX, playerTileY, game->player.direction, &frontTileX, &frontTileY);

    int frontInteractionGid = GetLayerTileAt(map, "Interactions", frontTileX, frontTileY);

    if (frontInteractionGid == INTERACT_TABERNACLE_ENTRANCE || frontInteractionGid == INTERACT_TABERNACLE_EXIT)
    {
        if (game->currentMapType == MAP_OUTSIDE)
            SwitchMap(game, MAP_INSIDE);
        else
            SwitchMap(game, MAP_OUTSIDE);

        return;
    }

    /* -----------------------------------------
       2) ANY-SIDE INTERACTIONS
       Check current tile too, so being very close
       still works.
       ----------------------------------------- */
    const int offsets[5][2] = {
        { 0,  0 },  // current tile
        { 0, -1 },  // up
        { 0,  1 },  // down
        {-1,  0 },  // left
        { 1,  0 }   // right
    };

    for (int i = 0; i < 5; i++)
    {
        int checkX = playerTileX + offsets[i][0];
        int checkY = playerTileY + offsets[i][1];

        int interactionGid = GetLayerTileAt(map, "Interactions", checkX, checkY);

        if (interactionGid == 0)
            continue;

        //Gets the Coal from Bronze
        if (interactionGid == INTERACT_BRONZE_ALTAR)
        {
            getCoalFromBronze(game);
            return;
        }

        if (interactionGid == INTERACT_INCENSE_ALTAR)
        {
            lightAltar(game);
            return;
        }

        if (interactionGid == INTERACT_WOOD_STACK)
        {
            printf("Wood stack interaction not implemented yet\n");
            return;
        }

        if (interactionGid == INTERACT_STORAGE)
        {
            printf("Storage interaction not implemented yet\n");
            return;
        }

        if (interactionGid == INTERACT_SACRIFICE_TABLE)
        {
            printf("Sacrifice table interaction not implemented yet\n");
            return;
        }

        if (interactionGid == INTERACT_LAVER)
        {
            printf("Laver interaction not implemented yet\n");
            return;
        }
        if (interactionGid == INTERACT_MENORAH)
        {
            printf("Menorah interaction not implemented yet\n");
            return;
        }
        if (interactionGid == INTERACT_SHOWBREAD_ALTER)
        {
            printf("Showbread Alter interaction not implemented yet\n");
            return;
        }
    }
}

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
    else
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
}

void DrawInteractionPrompt(const Game *game)
{
    int interactionGid = GetLayerTileAtWorld(&game->map, "Interactions", game->player.position);

    if (interactionGid != 0)
        DrawText("Press E", 20, 20, 30, WHITE);
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