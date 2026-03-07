#include "src/raylib.h"
#include <stdio.h>

#define SPRITE_WIDTH 16
#define SPRITE_HEIGHT 20
#define FRAMES_PER_ROW 3
#define DIRECTIONS 4

void DrawBackground(Texture2D texture, float rotation, float scale, Color tint)
{
    float texWidth = texture.width * scale;
    float texHeight = texture.height * scale;

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    for (float y = 0; y < screenHeight; y += texHeight)
    {
        for (float x = 0; x < screenWidth; x += texWidth)
        {
            Vector2 position = { x, y };
            DrawTextureEx(texture, position, rotation, scale, tint);
        }
    }
}

typedef struct Player {
    Vector2 position;
    int characterIndex;   // 0 through 7
    int direction;        // 0=down, 1=left, 2=right, 3=up
    int animFrame;        // 0,1,2
    float animTimer;
    float speed;
} Player;

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


int main(void)
{
    InitWindow(1920, 1080, "The Tabernacle");
    ToggleFullscreen();

    SetTargetFPS(60);

    Texture2D texture = LoadTexture("Game/sprites/Land/Dark_Sand.png");

    if (texture.id == 0) {
        printf("Failed to load texture!\n");
    }

    float rotation = 0.0f;
    float scale = 0.04f;

    Vector2 texturePosition = {
        GetScreenWidth() / 2.0f - (texture.width * scale) / 2.0f,
        GetScreenHeight() / 2.0f - (texture.height * scale) / 2.0f
    };

    Texture2D spriteSheet = LoadTexture("Game/sprites/Characters/character_9-16.png");

    Player player = {0};
    player.position = (Vector2){ 640, 360 };
    player.characterIndex = 0; // change 0-7 to pick a different character
    player.direction = 0;      // start facing down
    player.animFrame = 1;      // center frame for idle
    player.animTimer = 0.0f;
    player.speed = 120.0f;

    float drawScale = 4.0f;

    while (!WindowShouldClose())
    {
        //Update
        float dt = GetFrameTime();
        bool moving = false;

        if (IsKeyDown(KEY_RIGHT))
        {
            player.position.x += player.speed * dt;
            player.direction = 2;
            moving = true;
        }
        else if (IsKeyDown(KEY_LEFT))
        {
            player.position.x -= player.speed * dt;
            player.direction = 1;
            moving = true;
        }
        else if (IsKeyDown(KEY_UP))
        {
            player.position.y -= player.speed * dt;
            player.direction = 3;
            moving = true;
        }
        else if (IsKeyDown(KEY_DOWN))
        {
            player.position.y += player.speed * dt;
            player.direction = 0;
            moving = true;
        }

        if (moving)
        {
            player.animTimer += dt;
            if (player.animTimer >= 0.18f)
            {
                player.animTimer = 0.0f;
                player.animFrame++;

                if (player.animFrame > 2)
                    player.animFrame = 0;
            }
        }
        else
        {
            player.animFrame = 1; // idle frame
            player.animTimer = 0.0f;
        }

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

        //Draw
        BeginDrawing();

        ClearBackground(BLACK);
        DrawBackground(texture, rotation, scale, WHITE);
        DrawTexturePro(spriteSheet, source, dest, origin, 0.0f, WHITE);
        //DrawTextureEx(texture, texturePosition, rotation, scale, WHITE);

        EndDrawing();
    }

    UnloadTexture(texture);
    CloseWindow();

    return 0;
}