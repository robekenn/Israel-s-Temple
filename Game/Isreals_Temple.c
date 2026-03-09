#include "game/game.h"
#include "platformUtils/platform_utils.h"
#include "src/raylib.h"

int main(void)
{
    SetWorkingDirectoryToExecutable();

    InitWindow(1920, 1080, "Israel's Temple");
    ToggleBorderlessWindowed();
    SetTargetFPS(60);

    Game game;
    if (!GameInit(&game))
    {
        CloseWindow();
        return 1;
    }

    while (!WindowShouldClose())
    {
        GameUpdate(&game);

        BeginDrawing();
        ClearBackground(BLACK);
        GameDraw(&game);
        EndDrawing();
    }

    GameUnload(&game);
    CloseWindow();
    return 0;
}