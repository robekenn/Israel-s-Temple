#include "Bronze_Altar.h"
#include "../../game/game.h"
#include <stdio.h>

#define BRONZE_ALTAR_TILE_X 17
#define BRONZE_ALTAR_TILE_Y 10
#define BRONZE_ALTAR_LIT_GID 240
#define BRONZE_ALTAR_LAYER_NAME "TempleCol"

void ApplyBronzeAltarVisual(Game *game)
{
    if (!game)
        return;

    bool changed = SetLayerTileAtXY(
        &game->map,
        BRONZE_ALTAR_LAYER_NAME,
        BRONZE_ALTAR_TILE_X,
        BRONZE_ALTAR_TILE_Y,
        game->bronzeAltarLit ? BRONZE_ALTAR_LIT_GID : 255
    );

    printf("Bronze altar visual applied. lit=%d changed=%d\n",
           game->bronzeAltarLit ? 1 : 0,
           changed ? 1 : 0);
}

void prepareBronzeWithAshBroom(Game *game)
{
    if (!game)
        return;

    if (game->bronzeAltarLit)
    {
        printf("Bronze altar is already lit.\n");
        return;
    }

    if (HasHeldItem(&game->inventory, TOOL_ASH_BROOM))
    {
        game->bronzeAltarReadyForWood = true;
        printf("Bronze altar prepared with ash broom.\n");
    }
    else
    {
        printf("You need the ash broom first.\n");
    }
}

void lightBronzeWithWood(Game *game)
{
    if (!game)
        return;

    if (game->bronzeAltarLit)
    {
        printf("Bronze altar is already lit.\n");
        return;
    }

    if (!game->bronzeAltarReadyForWood)
    {
        printf("You must use the ash broom first.\n");
        return;
    }

    if (HasHeldItem(&game->inventory, ITEM_WOOD))
    {
        game->bronzeAltarLit = true;
        game->bronzeAltarReadyForWood = false;

        SetHeldItem(&game->inventory, ITEM_NONE);

        ApplyBronzeAltarVisual(game);
        printf("Bronze altar lit with wood.\n");
    }
    else
    {
        printf("You need wood to light the bronze altar.\n");
    }
}

void getCoalFromBronze(Game *game)
{
    if (!game)
        return;

    if (!game->bronzeAltarLit)
    {
        printf("Bronze altar is not lit yet.\n");
        return;
    }

    if (HasHeldItem(&game->inventory, ITEM_CENSER))
    {
        SetHeldItem(&game->inventory, ITEM_CENSER_COAL);
        printf("Bronze altar used: censer -> censer coal\n");
    }
    else
    {
        printf("You need to be holding the censer.\n");
    }
}

void ReapplyBronzeAltarState(Game *game)
{
    if (!game)
        return;

    ApplyBronzeAltarVisual(game);
}