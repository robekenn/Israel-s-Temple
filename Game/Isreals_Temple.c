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

// On macOS, make the working directory match the executable location
// so relative file paths still work when launching the app bundle.
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

// Track which map is currently loaded
typedef enum MapType {
    MAP_OUTSIDE,
    MAP_INSIDE
} MapType;


int main(void)
{
    SetWorkingDirectoryToExecutable();

    InitWindow(1920, 1080, "Israel's Temple");
    ToggleFullscreen();
    SetTargetFPS(60);

    // -----------------------------
    // Load first map (outside map)
    // -----------------------------
    TileMap map;
    MapType currentMapType = MAP_OUTSIDE;

    if (!LoadTileMap("Game/MapLoader/Maps/Map1.json", &map)) {
        CloseWindow();
        return 1;
    }

    // -----------------------------
    // Load player sprite sheet
    // -----------------------------
    Texture2D spriteSheet = LoadTexture("Game/sprites/Characters/character_9-16.png");
    SetTextureFilter(spriteSheet, TEXTURE_FILTER_POINT);

    // -----------------------------
    // Set up player
    // -----------------------------
    Player player = {0};
    player.characterIndex = 0;   // choose character row block
    player.direction = 0;        // 0 = down
    player.animFrame = 1;        // center frame for idle
    player.animTimer = 0.0f;
    player.speed = 120.0f;

    float PlayerScale = 3.5f;

    // Start player near the outdoor temple entrance
    // Adjust these tile coordinates if you want a different start point.
    SetPlayerToTileCenter(&player, &map, 17, 18);

    while (!WindowShouldClose())
    {
        // -----------------------------
        // Update player movement/collision
        // -----------------------------
        UpdatePlayer(&player, &map, PlayerScale);

        // -----------------------------
        // Check interaction tile under player
        // If the player is standing on a non-zero tile in the
        // "Interactions" layer, they can press E to interact.
        // -----------------------------
        int interactionGid = GetLayerTileAtWorld(&map, "Interactions", player.position);

        // Press E to switch maps
        if (interactionGid != 0 && IsKeyPressed(KEY_E))
        {
            // Leaving outside map -> entering inside map
            if (currentMapType == MAP_OUTSIDE)
            {
                UnloadTileMap(&map);

                if (LoadTileMap("Game/MapLoader/Maps/Temple_Interior.json", &map))
                {
                    currentMapType = MAP_INSIDE;

                    // Spawn player just inside the temple entrance
                    // Change these tile coordinates if needed.
                    SetPlayerToTileCenter(&player, &map, 12, 12);
                }
                else
                {
                    CloseWindow();
                    return 1;
                }
            }
            // Leaving inside map -> going back outside
            else if (currentMapType == MAP_INSIDE)
            {
                UnloadTileMap(&map);

                if (LoadTileMap("Game/MapLoader/Maps/Map1.json", &map))
                {
                    currentMapType = MAP_OUTSIDE;

                    // Spawn player just south of the temple entrance
                    SetPlayerToTileCenter(&player, &map, 17, 5);
                }
                else
                {
                    CloseWindow();
                    return 1;
                }
            }
        }

        // -----------------------------
        // Draw
        // -----------------------------
        BeginDrawing();
        ClearBackground(BLACK);

        DrawTileMap(&map);
        DrawPlayer(player, spriteSheet, PlayerScale);

        // Show interaction prompt if standing on an interaction tile
        if (interactionGid != 0)
        {
            DrawText("Press E", 20, 20, 30, WHITE);
        }

        EndDrawing();
    }

    // -----------------------------
    // Cleanup
    // -----------------------------
    UnloadTileMap(&map);
    UnloadTexture(spriteSheet);
    CloseWindow();

    return 0;
}