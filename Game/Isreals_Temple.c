#include "game/game.h"
#include "game/view.h"

#include <math.h>

int main(void)
{
    unsigned int windowFlags = FLAG_WINDOW_RESIZABLE;
#if defined(__APPLE__)
    // Match the Cocoa OpenGL drawable to GLFW's Retina framebuffer size.
    windowFlags |= FLAG_WINDOW_HIGHDPI;
#endif
    SetConfigFlags(windowFlags);
    InitWindow(VIEW_WIDTH, VIEW_HEIGHT, "Israel's Temple");
    if (!IsWindowReady())
        return 1;

    SetWindowMinSize(800, 500);
    MaximizeWindow();
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    if (!FileExists(OUTSIDE_MAP_PATH))
        ChangeDirectory(GetApplicationDirectory());

    RenderTexture2D canvas = LoadRenderTexture(VIEW_WIDTH, VIEW_HEIGHT);
    if (!canvas.id)
    {
        CloseWindow();
        return 1;
    }
    SetTextureFilter(canvas.texture, TEXTURE_FILTER_BILINEAR);

    Game game;
    if (!GameInit(&game))
    {
        UnloadRenderTexture(canvas);
        CloseWindow();
        return 1;
    }

    while (!game.shouldCloseGame)
    {
        float scale = fminf((float)GetScreenWidth() / VIEW_WIDTH, (float)GetScreenHeight() / VIEW_HEIGHT);
        if (scale <= 0)
            scale = 1;
        int left = (int)((GetScreenWidth() - VIEW_WIDTH*scale) / 2);
        int top = (int)((GetScreenHeight() - VIEW_HEIGHT*scale) / 2);
        SetMouseOffset(-left, -top);
        SetMouseScale(1 / scale, 1 / scale);
        GameUpdate(&game);

        BeginTextureMode(canvas);
        ClearBackground(BLACK);
        GameDraw(&game);
        EndTextureMode();

        BeginDrawing();
        ClearBackground((Color){18, 15, 24, 255});
        DrawTexturePro(canvas.texture, (Rectangle){0, 0, VIEW_WIDTH, -VIEW_HEIGHT},
                       (Rectangle){left, top, VIEW_WIDTH*scale, VIEW_HEIGHT*scale},
                       (Vector2){0, 0}, 0, WHITE);
        EndDrawing();
    }

    GameUnload(&game);
    UnloadRenderTexture(canvas);
    CloseWindow();
    return 0;
}
