#include "game_internal.h"

#include <stdio.h>

void GameMessage(Game *game, const char *message)
{
    snprintf(game->message, sizeof(game->message), "%s", message);
}

void BeginCampaign(Game *game)
{
    CampaignRestore(&game->campaign, 0, 0, 0, -1);
    InitializePlayer(game);
    PlacePlayer(game, OUTSIDE_START_X, OUTSIDE_START_Y, DIR_UP);
    SetHeldItem(&game->inventory, ITEM_NONE);
    game->started = true;
    game->ended = false;
    game->screen = SCREEN_PLAY;
    ApplyPersistentMapState(game);
    GameMessage(game, CampaignShift(&game->campaign)->opening);
    SaveGame(game);
}

bool GameInit(Game *game)
{
    *game = (Game){.screen = SCREEN_TITLE, .playerScale = 2.35f};
    InitializePlayer(game);
    InitInventory(&game->inventory);
    CampaignRestore(&game->campaign, 0, 0, 0, -1);
    if (!SwitchMap(game, MAP_OUTSIDE))
        return false;
    PlacePlayer(game, OUTSIDE_START_X, OUTSIDE_START_Y, DIR_UP);

    game->spriteSheet = LoadTexture(PLAYER_SPRITE_PATH);
    game->itemSpriteSheet = LoadTexture("Game/sprites/Items/Items.png");
    if (!game->spriteSheet.id || !game->itemSpriteSheet.id)
    {
        GameUnload(game);
        return false;
    }
    SetTextureFilter(game->spriteSheet, TEXTURE_FILTER_POINT);
    SetTextureFilter(game->itemSpriteSheet, TEXTURE_FILTER_POINT);
    SetSavePath(game);
    LoadGame(game);
    return true;
}

void GameUnload(Game *game)
{
    UnloadTileMap(&game->map);
    UnloadTexture(game->spriteSheet);
    UnloadTexture(game->itemSpriteSheet);
}
