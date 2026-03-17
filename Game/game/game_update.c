#include "game_internal.h"

static void UpdateExitMenu(Game *game)
{
    if (!game->showExitMenu && (IsKeyPressed(KEY_ESCAPE) || WindowShouldClose()))
    {
        game->showExitMenu = true;
        return;
    }

    if (!game->showExitMenu)
        return;

    Rectangle yesButton = {
        GetScreenWidth() * 0.5f - 140,
        GetScreenHeight() * 0.5f + 40,
        120,
        50
    };

    Rectangle noButton = {
        GetScreenWidth() * 0.5f + 20,
        GetScreenHeight() * 0.5f + 40,
        120,
        50
    };

    Vector2 mouse = GetMousePosition();

    if (IsKeyPressed(KEY_Y))
    {
        game->shouldCloseGame = true;
        return;
    }

    if (IsKeyPressed(KEY_N))
    {
        game->showExitMenu = false;
        return;
    }

    if (CheckCollisionPointRec(mouse, yesButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        game->shouldCloseGame = true;
        return;
    }

    if (CheckCollisionPointRec(mouse, noButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        game->showExitMenu = false;
        return;
    }
}

void GameUpdate(Game *game)
{
    if (game == NULL)
        return;

    UpdateExitMenu(game);
    if (game->showExitMenu || game->shouldCloseGame)
        return;

    if (game->storage.isOpen)
    {
        UpdateStorageWithGame(game);
        return;
    }

    UpdatePlayer(&game->player, &game->map, game->playerScale);
    HandleInteraction(game);
}