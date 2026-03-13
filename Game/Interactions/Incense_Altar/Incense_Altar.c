#include "Incense_Altar.h"
#include "../../game/game.h"
#include <stdio.h>

void changeIncenseAltarStatus(Game* game){
    game->incenseAltarLit = true;
    printf("Incense altar lit and saved in memory\n");
}

void lightAltar(Game* game)
{
    // STEP 1: Player brings coal censer
    if (!game->incenseCoalPlaced && HasHeldItem(&game->inventory, ITEM_CENSER_COAL))
    {
        printf("Coal placed on the incense altar.\n");

        game->incenseCoalPlaced = true;

        // Player now holds empty censer
        SetHeldItem(&game->inventory, ITEM_CENSER);

        return;
    }

    // STEP 2: Coal already placed, now player brings incense
    if (game->incenseCoalPlaced && !game->incenseAltarLit && HasHeldItem(&game->inventory, ITEM_INCENSE))
    {
        bool changed = SetLayerTileAtXY(
            &game->map,
            "TempleCol",
            INCENSE_ALTAR_TILE_X,
            INCENSE_ALTAR_TILE_Y,
            INCENSE_ALTAR_LIT_GID
        );

        printf("Incense burned on altar. changed=%d\n", changed ? 1 : 0);

        if (changed)
        {
            SetHeldItem(&game->inventory, ITEM_NONE);
            changeIncenseAltarStatus(game);
        }

        return;
    }

    // Feedback if wrong item
    printf("You need coal first, then incense.\n");
}