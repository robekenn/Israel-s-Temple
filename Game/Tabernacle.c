#include <stdio.h>
#include "src/raylib.h"
#include "Character_System/CharacterSystem.h"
#include <limits.h>
#include <unistd.h>
#include <libgen.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

void SetWorkingDirectoryToExecutable(void)
{
#ifdef __APPLE__
    char path[PATH_MAX];
    uint32_t size = sizeof(path);

    if (_NSGetExecutablePath(path, &size) == 0)
    {
        char *dir = dirname(path);
        chdir(dir);
    }
#endif
}

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


int main(void)
{
    SetWorkingDirectoryToExecutable();
    int monitor = GetCurrentMonitor();
    int screenWidth = GetMonitorWidth(monitor);
    int screenHeight = GetMonitorHeight(monitor);

    InitWindow(screenWidth, screenHeight, "The Tabernacle");
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
        UpdatePlayer(&player);

        //Draw
        BeginDrawing();

        ClearBackground(BLACK);
        DrawBackground(texture, rotation, scale, WHITE);
        DrawPlayer(player, spriteSheet, drawScale);
        //DrawTextureEx(texture, texturePosition, rotation, scale, WHITE);

        EndDrawing();
    }

    UnloadTexture(texture);
    CloseWindow();

    return 0;
}