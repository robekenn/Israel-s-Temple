#include "game_internal.h"
#include <string.h>

bool GameInit(Game *game)
{
    if (game == NULL)
        return false;

    memset(game, 0, sizeof(*game));

    game->currentMapType = MAP_OUTSIDE;
    game->playerScale = 3.5f;

    game->incenseAltarLit = false;
    game->menorahStage = MENORAH_STAGE_UNPREPARED;
    game->bronzeAltarLit = false;
    game->bronzeAltarReadyForWood = false;

    game->showExitMenu = false;
    game->shouldCloseGame = false;

    if (!LoadTileMap(OUTSIDE_MAP_PATH, &game->map))
        return false;

    game->spriteSheet = LoadTexture(PLAYER_SPRITE_PATH);
    SetTextureFilter(game->spriteSheet, TEXTURE_FILTER_POINT);

    game->inventoryUITexture = LoadTexture("Game/sprites/UI/Inventory.png");
    game->itemSpriteSheet = LoadTexture("Game/sprites/Items/Items.png");
    SetTextureFilter(game->inventoryUITexture, TEXTURE_FILTER_POINT);
    SetTextureFilter(game->itemSpriteSheet, TEXTURE_FILTER_POINT);

    InitStorage(&game->storage, "Game/sprites/UI/Storage_UI.png");
    InitInventory(&game->inventory);

    AddItemToInventory(&game->inventory, ITEM_CENSER);

    InitializePlayer(game);
    SetPlayerToTileCenter(&game->player, &game->map, OUTSIDE_START_X, OUTSIDE_START_Y);

    ApplyPersistentMapState(game);

    return true;
}

void GameUnload(Game *game)
{
    if (game == NULL)
        return;

    UnloadStorage(&game->storage);
    UnloadTileMap(&game->map);
    UnloadTexture(game->spriteSheet);
    UnloadTexture(game->inventoryUITexture);
    UnloadTexture(game->itemSpriteSheet);
}