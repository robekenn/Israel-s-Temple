#include "Wood_Stack.h"
#include "../../game/game.h"
#include <stdio.h>

void getWood(Game* game){
    SetHeldItem(&game->inventory, ITEM_WOOD);
}