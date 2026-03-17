#include "InteractionHandler.h"
#include "../game/game_internal.h"
#include <stdio.h>

void HandleInteraction(Game *game)
{
    if (!IsKeyPressed(KEY_E))
        return;

    TileMap *map = &game->map;

    Vector2 interactPoint = GetPlayerInteractionPoint(game->player, game->playerScale);

    int playerTileX = 0;
    int playerTileY = 0;
    GetTileCoordsAtWorld(map, interactPoint, &playerTileX, &playerTileY);

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

    const int offsets[5][2] = {
        { 0,  0 },
        { 0, -1 },
        { 0,  1 },
        {-1,  0 },
        { 1,  0 }
    };

    for (int i = 0; i < 5; i++)
    {
        int checkX = playerTileX + offsets[i][0];
        int checkY = playerTileY + offsets[i][1];

        int interactionGid = GetLayerTileAt(map, "Interactions", checkX, checkY);

        if (interactionGid == 0)
            continue;

        if (interactionGid == INTERACT_BRONZE_ALTAR)
        {
            if (HasHeldItem(&game->inventory, TOOL_ASH_BROOM))
            {
                prepareBronzeWithAshBroom(game);
            }
            else if (HasHeldItem(&game->inventory, ITEM_WOOD))
            {
                lightBronzeWithWood(game);
            }
            else if (HasHeldItem(&game->inventory, ITEM_CENSER))
            {
                getCoalFromBronze(game);
            }
            else
            {
                printf("Nothing happens.\n");
            }
            return;
        }

        if (interactionGid == INTERACT_INCENSE_ALTAR)
        {
            lightAltar(game);
            return;
        }

        if (interactionGid == INTERACT_WOOD_STACK)
        {
            getWood(game);
            return;
        }

        if (interactionGid == INTERACT_STORAGE)
        {
            HandleStorageInteraction(game);
            return;
        }

        if (interactionGid == INTERACT_SACRIFICE_TABLE)
        {
            printf("Sacrifice table interaction not implemented yet\n");
            return;
        }

        if (interactionGid == INTERACT_LAVER)
        {
            getWaterFromLaver(game);
            return;
        }

        if (interactionGid == INTERACT_MENORAH)
        {
            UseMenorah(game);
            return;
        }

        if (interactionGid == INTERACT_SHOWBREAD_ALTER)
        {
            printf("Showbread Alter interaction not implemented yet\n");
            return;
        }
    }
}