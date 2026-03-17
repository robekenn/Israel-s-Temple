#include "game_internal.h"

static void DrawExitMenu(const Game *game)
{
    if (!game->showExitMenu)
        return;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    Rectangle panel = {
        screenW * 0.5f - 220,
        screenH * 0.5f - 120,
        440,
        240
    };

    Rectangle yesButton = {
        screenW * 0.5f - 140,
        screenH * 0.5f + 40,
        120,
        50
    };

    Rectangle noButton = {
        screenW * 0.5f + 20,
        screenH * 0.5f + 40,
        120,
        50
    };

    Vector2 mouse = GetMousePosition();
    bool hoverYes = CheckCollisionPointRec(mouse, yesButton);
    bool hoverNo = CheckCollisionPointRec(mouse, noButton);

    DrawRectangle(0, 0, screenW, screenH, Fade(BLACK, 0.65f));
    DrawRectangleRounded(panel, 0.15f, 12, DARKGRAY);
    DrawRectangleLinesEx(panel, 3, WHITE);

    DrawText("Exit Game?", (int)panel.x + 115, (int)panel.y + 28, 36, WHITE);
    DrawText("Are you sure you want to quit?", (int)panel.x + 52, (int)panel.y + 85, 24, LIGHTGRAY);

    DrawRectangleRec(yesButton, hoverYes ? RED : MAROON);
    DrawRectangleLinesEx(yesButton, 2, WHITE);
    DrawText("Yes", (int)yesButton.x + 34, (int)yesButton.y + 12, 24, WHITE);

    DrawRectangleRec(noButton, hoverNo ? DARKGREEN : GREEN);
    DrawRectangleLinesEx(noButton, 2, WHITE);
    DrawText("No", (int)noButton.x + 40, (int)noButton.y + 12, 24, WHITE);
}

void GameDraw(const Game *game)
{
    if (game == NULL)
        return;

    DrawTileMap(&game->map);
    DrawPlayer(game->player, game->spriteSheet, game->playerScale);

    if (game->storage.isOpen)
    {
        DrawStorage(&game->storage, game->itemSpriteSheet);
    }

    DrawInteractionPrompt(game);

    DrawInventoryUI(
        &game->inventory,
        game->inventoryUITexture,
        game->itemSpriteSheet
    );

    DrawText(TextFormat("Held: %s", GetItemName(game->inventory.heldItem)), 20, 60, 20, WHITE);
    DrawText(TextFormat("Incense lit: %s", game->incenseAltarLit ? "true" : "false"), 20, 85, 20, WHITE);
    DrawText(TextFormat("Map: %s", game->currentMapType == MAP_OUTSIDE ? "OUTSIDE" : "INSIDE"), 20, 110, 20, WHITE);

    DrawExitMenu(game);
}