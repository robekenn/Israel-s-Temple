#ifndef GAME_INTERNAL_H
#define GAME_INTERNAL_H

#include "game.h"
#include "view.h"
#include "../Interactions/InteractionHandler.h"

void InitializePlayer(Game *game);
void PlacePlayer(Game *game, int tileX, int tileY, int direction);
Vector2 GetPlayerInteractionPoint(Player player, float drawScale);

bool SwitchMap(Game *game, MapType newMapType);
void ApplyPersistentMapState(Game *game);
int StationAt(const Game *game, int x, int y);
bool FindStation(const Game *game, int station, Vector2 *center);

void GameMessage(Game *game, const char *message);
void SetSavePath(Game *game);
bool LoadGame(Game *game);
bool SaveGame(Game *game);
void BeginCampaign(Game *game);

bool GameMenuPressed(const Game *game, int index);
void DrawWorld(const Game *game);
void DrawTitleScene(void);

#endif
