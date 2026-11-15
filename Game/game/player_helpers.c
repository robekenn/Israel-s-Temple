#include "game_internal.h"

void InitializePlayer(Game *game)
{
    game->player = (Player){0};
    game->player.characterIndex = 0;
    game->player.direction = 0;
    game->player.animFrame = 1;
    game->player.animTimer = 0.0f;
    game->player.speed = 205.0f;
}

Vector2 GetPlayerInteractionPoint(Player player, float drawScale)
{
    float spriteHeight = SPRITE_HEIGHT * drawScale;

    Vector2 point = player.position;
    point.y += spriteHeight * 0.32f;

    return point;
}
void PlacePlayer(Game *game, int tileX, int tileY, int direction)
{
    SetPlayerToTileCenter(&game->player, &game->map, tileX, tileY);
    game->player.position.y -= SPRITE_HEIGHT * game->playerScale * 0.32f;
    game->player.direction = direction;
}
