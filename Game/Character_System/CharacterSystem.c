#include "CharacterSystem.h"
#include <math.h>

Rectangle GetPlayerSourceRect(Player player)
{
    int characterColBlock = (player.characterIndex % 4) * FRAMES_PER_ROW;
    int characterRowBlock = (player.characterIndex / 4) * DIRECTIONS;

    int sourceX = (characterColBlock + player.animFrame) * SPRITE_WIDTH;
    int sourceY = (characterRowBlock + player.direction) * SPRITE_HEIGHT;

    Rectangle source = {
        (float)sourceX,
        (float)sourceY,
        (float)SPRITE_WIDTH,
        (float)SPRITE_HEIGHT
    };

    return source;
}


#include <math.h>

void UpdatePlayer(Player *player)
{
    float dt = GetFrameTime();
    bool moving = false;

    Vector2 movement = {0.0f, 0.0f};

    // RIGHT (D or Right Arrow)
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
    {
        movement.x += 1.0f;
        player->direction = 2;
        moving = true;
    }

    // LEFT (A or Left Arrow)
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
    {
        movement.x -= 1.0f;
        player->direction = 1;
        moving = true;
    }

    // UP (W or Up Arrow)
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
    {
        movement.y -= 1.0f;
        player->direction = 3;
        moving = true;
    }

    // DOWN (S or Down Arrow)
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
    {
        movement.y += 1.0f;
        player->direction = 0;
        moving = true;
    }

    if (moving)
    {
        float length = sqrtf(movement.x * movement.x + movement.y * movement.y);

        if (length > 0.0f)
        {
            movement.x /= length;
            movement.y /= length;
        }

        player->position.x += movement.x * player->speed * dt;
        player->position.y += movement.y * player->speed * dt;

        player->animTimer += dt;

        if (player->animTimer >= 0.18f)
        {
            player->animTimer = 0.0f;
            player->animFrame++;

            if (player->animFrame > 2)
                player->animFrame = 0;
        }
    }
    else
    {
        player->animFrame = 1;
        player->animTimer = 0.0f;
    }
}

void DrawPlayer(Player player, Texture2D spriteSheet, float drawScale)
{
    Rectangle source = GetPlayerSourceRect(player);

    Rectangle dest = {
        player.position.x,
        player.position.y,
        SPRITE_WIDTH * drawScale,
        SPRITE_HEIGHT * drawScale
    };

    Vector2 origin = {
        dest.width / 2.0f,
        dest.height / 2.0f
    };

    DrawTexturePro(spriteSheet, source, dest, origin, 0.0f, WHITE);
}