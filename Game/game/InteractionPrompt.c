#include "game_internal.h"

void DrawInteractionPrompt(const Game *game)
{
    int interactionGid = GetLayerTileAtWorld(&game->map, "Interactions", game->player.position);

    if (interactionGid != 0)
        DrawText("Press E", 20, 20, 30, WHITE);
}