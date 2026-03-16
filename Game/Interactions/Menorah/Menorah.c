#include "Menorah.h"
#include "../../game/game.h"
#include <stdio.h>

static void SetMenorahStage(Game *game, MenorahStage stage)
{
    game->menorahStage = stage;
    printf("Menorah stage updated to %d\n", (int)stage);
}

void ApplyMenorahVisual(Game *game)
{
    if (game == NULL || game->currentMapType != MAP_INSIDE)
        return;

    int topGid;
    int middleGid;
    int bottomGid;

    if (game->menorahStage == MENORAH_STAGE_LIT)
    {
        topGid = MENORAH_LIT_TOP_GID;
        middleGid = MENORAH_LIT_MIDDLE_GID;
        bottomGid = MENORAH_LIT_BOTTOM_GID;
    }
    else
    {
        topGid = MENORAH_UNLIT_TOP_GID;
        middleGid = MENORAH_UNLIT_MIDDLE_GID;
        bottomGid = MENORAH_UNLIT_BOTTOM_GID;
    }

    SetLayerTileAtXY(&game->map, "TempleCol", MENORAH_TILE_X, MENORAH_TILE_Y, topGid);
    SetLayerTileAtXY(&game->map, "TempleCol", MENORAH_TILE_X, MENORAH_TILE_Y + 1, middleGid);
    SetLayerTileAtXY(&game->map, "TempleCol", MENORAH_TILE_X, MENORAH_TILE_Y + 2, bottomGid);
}

void UseMenorah(Game *game)
{
    if (game == NULL)
        return;

    if (game->menorahStage == MENORAH_STAGE_UNPREPARED)
    {
        if (HasHeldItem(&game->inventory, TOOL_BLADE))
        {
            SetHeldItem(&game->inventory, ITEM_NONE);
            game->menorahStage = MENORAH_STAGE_BLADE_DONE;
            printf("Menorah prepared with blade\n");
        }
        else
        {
            printf("You must use the blade first\n");
        }
        return;
    }

    if (game->menorahStage == MENORAH_STAGE_BLADE_DONE)
    {
        if (HasHeldItem(&game->inventory, ITEM_OIL))
        {
            SetHeldItem(&game->inventory, ITEM_NONE);
            game->menorahStage = MENORAH_STAGE_OIL_DONE;
            printf("Menorah filled with oil\n");
        }
        else
        {
            printf("You must add oil next\n");
        }
        return;
    }

    if (game->menorahStage == MENORAH_STAGE_OIL_DONE)
    {
        if (HasHeldItem(&game->inventory, ITEM_CENSER_COAL))
        {
            SetHeldItem(&game->inventory, ITEM_CENSER);
            game->menorahStage = MENORAH_STAGE_LIT;
            ApplyMenorahVisual(game);
            printf("Menorah lit successfully\n");
        }
        else
        {
            printf("You need the coal censer to light the Menorah\n");
        }
        return;
    }

    if (game->menorahStage == MENORAH_STAGE_LIT)
    {
        printf("Menorah is already lit\n");
    }
}