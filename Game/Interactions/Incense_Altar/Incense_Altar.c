#include "Incense_Altar.h"
#include "../../game/game.h"
#include <stdio.h>

void lightAltar(Game* game) {
    if (HasHeldItem(&game->inventory, ITEM_CENSER_COAL)){
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
}


void changeIncenseAltarStatus(Game* game){
    game->incenseAltarLit = true;
    SetHeldItem(&game->inventory, ITEM_CENSER);
    printf("Incense altar lit and saved in memory\n");
}