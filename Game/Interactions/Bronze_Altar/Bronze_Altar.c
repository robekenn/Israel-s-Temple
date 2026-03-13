#include "Bronze_Altar.h"
#include "../../game/game.h"
#include <stdio.h>

void getCoalFromBronze(Game* game) {
    if (HasHeldItem(&game->inventory, ITEM_CENSER)) {
        SetHeldItem(&game->inventory, ITEM_CENSER_COAL);
        printf("Bronze altar used: censer -> censer coal\n");
    }
}