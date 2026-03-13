#include "Storage.h"
#include "../../game/game.h"
#include <stdio.h>

void getItemFromStorage(Game* game){
    SetHeldItem(&game->inventory, ITEM_INCENSE);
}