#include "Storage.h"
#include "../../game/game.h"
#include <stdio.h>
#include <string.h>

#define FALLBACK_SLOT_SIZE 64
#define FALLBACK_SLOT_SPACING 35
#define FALLBACK_PANEL_PADDING 48
#define FALLBACK_TITLE_HEIGHT 0
#define FALLBACK_FOOTER_HEIGHT 0

/*
    These slot rectangles are defined in the ORIGINAL PIXEL SPACE
    of your storage UI image.

    Example:
    If your storage UI png is 128x128 and each slot hole is 16x16,
    put the slot positions here exactly where they appear in the art.

    You will probably need to tweak these values slightly to match
    your exact UI image.
*/
static const Rectangle gStorageSlotTemplate[STORAGE_SLOT_COUNT] = {
    { 8, 7, 14, 14 }, { 25, 7, 14, 14 }, { 42, 7, 14, 14 }, { 59, 7, 14, 14 },
    { 8, 24, 14, 14 }, { 25, 24, 14, 14 }, { 42, 24, 14, 14 }, { 59, 24, 14, 14 },
    { 8, 41, 14, 14 }, { 25, 41, 14, 14 }, { 42, 41, 14, 14 }, { 59, 41, 14, 14 },
    { 8, 58, 14, 14 }, { 25, 58, 14, 14 }, { 42, 58, 14, 14 }, { 59, 58, 14, 14 }
};

static float GetStorageUIScale(const Storage *storage)
{
    if (!storage || !storage->uiLoaded || storage->uiTexture.width <= 0 || storage->uiTexture.height <= 0)
        return 1.0f;

    float maxWidth = GetScreenWidth() * 0.80f;
    float maxHeight = GetScreenHeight() * 0.80f;

    float scaleX = maxWidth / (float)storage->uiTexture.width;
    float scaleY = maxHeight / (float)storage->uiTexture.height;

    float uniformScale = (scaleX < scaleY) ? scaleX : scaleY;

    if (uniformScale < 1.0f)
        return uniformScale;

    /* Optional: clamp upper size so it doesn't become absurdly large */
    if (uniformScale > 6.0f)
        uniformScale = 6.0f;

    return uniformScale;
}

static void UpdateStorageLayout(Storage *storage)
{
    if (!storage)
        return;

    if (storage->uiLoaded)
    {
        float uniformScale = GetStorageUIScale(storage);

        float panelWidth = storage->uiTexture.width * uniformScale;
        float panelHeight = storage->uiTexture.height * uniformScale;

        storage->panelRect.x = (GetScreenWidth() - panelWidth) / 2.0f;
        storage->panelRect.y = (GetScreenHeight() - panelHeight) / 2.0f;
        storage->panelRect.width = panelWidth;
        storage->panelRect.height = panelHeight;

        for (int i = 0; i < STORAGE_SLOT_COUNT; i++)
        {
            Rectangle src = gStorageSlotTemplate[i];

            storage->slotRects[i] = (Rectangle){
                storage->panelRect.x + src.x * uniformScale,
                storage->panelRect.y + src.y * uniformScale,
                src.width * uniformScale,
                src.height * uniformScale
            };
        }
    }
    else
    {
        int gridWidth = STORAGE_COLS * FALLBACK_SLOT_SIZE + (STORAGE_COLS - 1) * FALLBACK_SLOT_SPACING;
        int gridHeight = STORAGE_ROWS * FALLBACK_SLOT_SIZE + (STORAGE_ROWS - 1) * FALLBACK_SLOT_SPACING;

        int panelWidth = gridWidth + FALLBACK_PANEL_PADDING * 2;
        int panelHeight = FALLBACK_TITLE_HEIGHT + gridHeight + FALLBACK_FOOTER_HEIGHT + FALLBACK_PANEL_PADDING * 2;

        storage->panelRect.x = (GetScreenWidth() - panelWidth) / 2.0f;
        storage->panelRect.y = (GetScreenHeight() - panelHeight) / 2.0f;
        storage->panelRect.width = panelWidth;
        storage->panelRect.height = panelHeight;

        float startX = storage->panelRect.x + FALLBACK_PANEL_PADDING;
        float startY = storage->panelRect.y + FALLBACK_PANEL_PADDING + FALLBACK_TITLE_HEIGHT;

        for (int row = 0; row < STORAGE_ROWS; row++)
        {
            for (int col = 0; col < STORAGE_COLS; col++)
            {
                int index = row * STORAGE_COLS + col;

                storage->slotRects[index] = (Rectangle){
                    startX + col * (FALLBACK_SLOT_SIZE + FALLBACK_SLOT_SPACING),
                    startY + row * (FALLBACK_SLOT_SIZE + FALLBACK_SLOT_SPACING),
                    FALLBACK_SLOT_SIZE,
                    FALLBACK_SLOT_SIZE
                };
            }
        }
    }
}

void InitStorage(Storage *storage, const char *uiTexturePath)
{
    if (!storage)
        return;

    memset(storage, 0, sizeof(Storage));

    storage->isOpen = false;
    storage->selectedSlot = 0;
    storage->uiLoaded = false;

    for (int i = 0; i < STORAGE_SLOT_COUNT; i++)
    {
        storage->slots[i].item = ITEM_NONE;
    }

    if (uiTexturePath && FileExists(uiTexturePath))
    {
        storage->uiTexture = LoadTexture(uiTexturePath);
        storage->uiLoaded = true;
        SetTextureFilter(storage->uiTexture, TEXTURE_FILTER_POINT);
    }
    else
    {
        printf("Storage UI texture not found: %s\n", uiTexturePath ? uiTexturePath : "(null)");
    }

    UpdateStorageLayout(storage);

    storage->slots[0].item = ITEM_CENSER;
    storage->slots[1].item = ITEM_INCENSE;
    storage->slots[2].item = ITEM_OIL;
    storage->slots[3].item = ITEM_WINE;
    storage->slots[4].item = ITEM_FLOUR;
    storage->slots[5].item = TOOL_BLADE;
    storage->slots[6].item = TOOL_ASH_BROOM;
}

void UnloadStorage(Storage *storage)
{
    if (!storage)
        return;

    if (storage->uiLoaded)
    {
        UnloadTexture(storage->uiTexture);
        storage->uiLoaded = false;
    }
}

void OpenStorage(Storage *storage)
{
    if (!storage)
        return;

    storage->isOpen = true;
    UpdateStorageLayout(storage);
}

void CloseStorage(Storage *storage)
{
    if (!storage)
        return;

    storage->isOpen = false;
}

void ToggleStorage(Storage *storage)
{
    if (!storage)
        return;

    storage->isOpen = !storage->isOpen;

    if (storage->isOpen)
        UpdateStorageLayout(storage);
}

void UpdateStorage(Storage *storage)
{
    if (!storage || !storage->isOpen)
        return;

    UpdateStorageLayout(storage);

    if (IsKeyPressed(KEY_Q))
    {
        CloseStorage(storage);
        return;
    }

    if (IsKeyPressed(KEY_RIGHT))
        storage->selectedSlot = (storage->selectedSlot + 1) % STORAGE_SLOT_COUNT;

    if (IsKeyPressed(KEY_LEFT))
        storage->selectedSlot = (storage->selectedSlot - 1 + STORAGE_SLOT_COUNT) % STORAGE_SLOT_COUNT;

    if (IsKeyPressed(KEY_DOWN))
        storage->selectedSlot = (storage->selectedSlot + STORAGE_COLS) % STORAGE_SLOT_COUNT;

    if (IsKeyPressed(KEY_UP))
        storage->selectedSlot = (storage->selectedSlot - STORAGE_COLS + STORAGE_SLOT_COUNT) % STORAGE_SLOT_COUNT;
}

void DrawStorage(const Storage *storage, Texture2D itemSheet)
{
    if (!storage || !storage->isOpen)
        return;

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.55f));

    if (storage->uiLoaded)
    {
        DrawTexturePro(
            storage->uiTexture,
            (Rectangle){ 0, 0, (float)storage->uiTexture.width, (float)storage->uiTexture.height },
            storage->panelRect,
            (Vector2){ 0, 0 },
            0.0f,
            WHITE
        );
    }
    else
    {
        DrawRectangleRounded(storage->panelRect, 0.06f, 10, (Color){70, 40, 25, 245});
        DrawRectangleLinesEx(storage->panelRect, 3, (Color){140, 95, 55, 255});
    }

    for (int i = 0; i < STORAGE_SLOT_COUNT; i++)
    {
        Rectangle slot = storage->slotRects[i];
        StorageSlot storageSlot = storage->slots[i];

        /* Debug outlines so you can confirm alignment with the art */
        Color borderColor = (i == storage->selectedSlot) ? GOLD : (Color){125, 88, 58, 255};
        DrawRectangleLinesEx(slot, 3, borderColor);

        if (storageSlot.item != ITEM_NONE)
        {
            Rectangle src = GetItemSourceRect(storageSlot.item);

            Rectangle dst = {
                slot.x + slot.width * 0.12f,
                slot.y + slot.height * 0.12f,
                slot.width * 0.76f,
                slot.height * 0.76f
            };

            DrawTexturePro(
                itemSheet,
                src,
                dst,
                (Vector2){ 0, 0 },
                0.0f,
                WHITE
            );
        }
    }

    StorageSlot selected = storage->slots[storage->selectedSlot];
    if (selected.item != ITEM_NONE)
    {
        DrawText(
            GetItemName(selected.item),
            (int)storage->panelRect.x + 24,
            (int)(storage->panelRect.y + storage->panelRect.height + 8),
            24,
            GOLD
        );
    }

    DrawText(
        "Arrow Keys = Move   E = Equip   Q = Close",
        (int)storage->panelRect.x,
        (int)(storage->panelRect.y + storage->panelRect.height + 36),
        20,
        WHITE
    );
}

void HandleStorageInteraction(Game *game)
{
    if (!game)
        return;

    OpenStorage(&game->storage);
}

void UpdateStorageWithGame(Game *game)
{
    if (!game || !game->storage.isOpen)
        return;

    Storage *storage = &game->storage;
    UpdateStorageLayout(storage);

    if (IsKeyPressed(KEY_Q))
    {
        CloseStorage(storage);
        return;
    }

    if (IsKeyPressed(KEY_RIGHT))
        storage->selectedSlot = (storage->selectedSlot + 1) % STORAGE_SLOT_COUNT;

    if (IsKeyPressed(KEY_LEFT))
        storage->selectedSlot = (storage->selectedSlot - 1 + STORAGE_SLOT_COUNT) % STORAGE_SLOT_COUNT;

    if (IsKeyPressed(KEY_DOWN))
        storage->selectedSlot = (storage->selectedSlot + STORAGE_COLS) % STORAGE_SLOT_COUNT;

    if (IsKeyPressed(KEY_UP))
        storage->selectedSlot = (storage->selectedSlot - STORAGE_COLS + STORAGE_SLOT_COUNT) % STORAGE_SLOT_COUNT;

    if (IsKeyPressed(KEY_E))
    {
        StorageSlot *slot = &storage->slots[storage->selectedSlot];

        if (slot->item != ITEM_NONE)
        {
            SetHeldItem(&game->inventory, slot->item);
            printf("Equipped %s from storage\n", GetItemName(slot->item));
        }
    }
}