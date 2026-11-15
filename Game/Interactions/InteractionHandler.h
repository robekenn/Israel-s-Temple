#ifndef INTERACTION_HANDLER_H
#define INTERACTION_HANDLER_H

#include "../game/game_struct.h"

int GetReachableStation(const Game *game);
int GetObjectiveStation(const Game *game);
const char *StationName(const Game *game, int station);
void HandleInteraction(Game *game);

#endif
