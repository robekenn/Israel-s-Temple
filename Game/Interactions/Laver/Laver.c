#include "Laver.h"
#include "../../game/game.h"
#include <stdio.h>

void getWaterFromLaver(Game* game){
    if (HasHeldItem(&game->inventory, ITEM_BUCKET)) {
        SetHeldItem(&game->inventory, ITEM_WATER_BUCKET);
        printf("Laver used: bucket -> water bucket\n");
    }
}
