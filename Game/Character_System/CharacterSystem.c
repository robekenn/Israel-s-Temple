#include "CharacterSystem.h"
#include <math.h>

void SetPlayerToTileCenter(Player *player, TileMap *map, int tileX, int tileY)
{
    float tileWidth = GetMapTileWidthOnScreen(map);
    float tileHeight = GetMapTileHeightOnScreen(map);

    player->position.x = tileX * tileWidth + tileWidth / 2.0f;
    player->position.y = tileY * tileHeight + tileHeight / 2.0f;
}


static bool PlayerCollidesAt(TileMap *map, Vector2 centerPos, float drawScale)
{
    float spriteWidth = SPRITE_WIDTH * drawScale;
    float spriteHeight = SPRITE_HEIGHT * drawScale;

    // Narrow collision box around the player's feet
    float hitboxWidth = spriteWidth * 0.35f;
    float hitboxHeight = spriteHeight * 0.18f;

    // Put the hitbox near the bottom of the sprite
    float hitboxCenterX = centerPos.x;
    float hitboxCenterY = centerPos.y + (spriteHeight * 0.32f);

    Vector2 topLeft = {
        hitboxCenterX - hitboxWidth / 2.0f,
        hitboxCenterY - hitboxHeight / 2.0f
    };

    Vector2 topRight = {
        hitboxCenterX + hitboxWidth / 2.0f,
        hitboxCenterY - hitboxHeight / 2.0f
    };

    Vector2 bottomLeft = {
        hitboxCenterX - hitboxWidth / 2.0f,
        hitboxCenterY + hitboxHeight / 2.0f
    };

    Vector2 bottomRight = {
        hitboxCenterX + hitboxWidth / 2.0f,
        hitboxCenterY + hitboxHeight / 2.0f
    };

    Vector2 midLeft = {
        hitboxCenterX - hitboxWidth / 2.0f,
        hitboxCenterY
    };

    Vector2 midRight = {
        hitboxCenterX + hitboxWidth / 2.0f,
        hitboxCenterY
    };

    return IsMapCollisionAt(map, topLeft) ||
           IsMapCollisionAt(map, topRight) ||
           IsMapCollisionAt(map, bottomLeft) ||
           IsMapCollisionAt(map, bottomRight) ||
           IsMapCollisionAt(map, midLeft) ||
           IsMapCollisionAt(map, midRight);
}

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

void UpdatePlayer(Player *player, TileMap *map, float drawScale)
{
    float dt = GetFrameTime();
    bool moving = false;
    float sprint = 1.0f;

    Vector2 movement = {0.0f, 0.0f};

    if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
    {
        sprint = 1.5f;
    }

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
    {
        movement.x += 1.0f;
        player->direction = 2;
        moving = true;
    }

    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
    {
        movement.x -= 1.0f;
        player->direction = 1;
        moving = true;
    }

    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
    {
        movement.y -= 1.0f;
        player->direction = 3;
        moving = true;
    }

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

        float moveAmount = player->speed * sprint * dt;

        Vector2 tryPos = player->position;

        // Move on X first
        tryPos.x += movement.x * moveAmount;
        if (!PlayerCollidesAt(map, tryPos, drawScale))
        {
            player->position.x = tryPos.x;
        }

        // Then Y
        tryPos = player->position;
        tryPos.y += movement.y * moveAmount;
        if (!PlayerCollidesAt(map, tryPos, drawScale))
        {
            player->position.y = tryPos.y;
        }

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

    // Debug collision box if needed:
    /*
    float spriteWidth = SPRITE_WIDTH * drawScale;
    float spriteHeight = SPRITE_HEIGHT * drawScale;
    float hitboxWidth = spriteWidth * 0.35f;
    float hitboxHeight = spriteHeight * 0.18f;
    float hitboxCenterX = player.position.x;
    float hitboxCenterY = player.position.y + (spriteHeight * 0.32f);

    DrawRectangleLines(
        (int)(hitboxCenterX - hitboxWidth / 2.0f),
        (int)(hitboxCenterY - hitboxHeight / 2.0f),
        (int)hitboxWidth,
        (int)hitboxHeight,
        RED
    );
    */
}