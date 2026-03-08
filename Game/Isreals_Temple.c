#include <stdio.h>
#include "src/raylib.h"
#include "Character_System/CharacterSystem.h"
#include <limits.h>
#include <unistd.h>
#include <libgen.h>
#include "MapLoader/MapLoader.h"


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



int main(void)
{
    SetWorkingDirectoryToExecutable();
    InitWindow(1920, 1080, "Israel's Temple");
    ToggleFullscreen();

    TileMap map;
    
    if (!LoadTileMap("Game/MapLoader/Maps/Map1.json", &map)) {
        CloseWindow();
        return 1;
    };

    
    SetTargetFPS(60);

    float rotation = 0.0f;
    float scale = 0.04f;

    Texture2D spriteSheet = LoadTexture("Game/sprites/Characters/character_9-16.png");

    Player player = {0};
    player.position = (Vector2){ 640, 360 };
    player.characterIndex = 0; // change 0-7 to pick a different character
    player.direction = 0;      // start facing down
    player.animFrame = 1;      // center frame for idle
    player.animTimer = 0.0f;
    player.speed = 120.0f;

    float PlayerScale = 4.0f;

    while (!WindowShouldClose())
    {
        //Update
        UpdatePlayer(&player,&map, PlayerScale);

        
        //Draw
        BeginDrawing();

        ClearBackground(BLACK);
        DrawTileMap(&map);
        DrawPlayer(player, spriteSheet, PlayerScale);
        //DrawTextureEx(texture, texturePosition, rotation, scale, WHITE);

        EndDrawing();
    }
    UnloadTileMap(&map);
    UnloadTexture(spriteSheet);
    CloseWindow();

    return 0;
}