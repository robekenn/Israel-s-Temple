#include "game_internal.h"

void InitializePlayer(Game *game)
{
    game->player = (Player){0};
    game->player.characterIndex = 0;
    game->player.direction = 0;
    game->player.animFrame = 1;
    game->player.animTimer = 0.0f;
    game->player.speed = 120.0f;
}

Vector2 GetPlayerInteractionPoint(Player player, float drawScale)
{
    float spriteHeight = SPRITE_HEIGHT * drawScale;

    Vector2 point = player.position;
    point.y += spriteHeight * 0.32f;

    return point;
}