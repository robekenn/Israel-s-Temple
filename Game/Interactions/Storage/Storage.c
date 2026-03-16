#include "Storage.h"
#include "../../game/game.h"
#include <stdio.h>
#include <string.h>

#define FALLBACK_SLOT_SIZE 64
#define FALLBACK_SLOT_SPACING 35
#define FALLBACK_PANEL_PADDING 48
#define FALLBACK_TITLE_HEIGHT 0
#define FALLBACK_FOOTER_HEIGHT 0

#define STORAGE_BG_OVERLAY_ALPHA 0.45f
#define STORAGE_MAX_UI_SCALE 6.0f
#define STORAGE_SCREEN_FILL 0.80f

#define SLOT_BORDER_THICKNESS 2.0f
#define SLOT_SELECTED_THICKNESS 4.0f

#define ITEM_INSET_RATIO 0.12f
#define ITEM_DRAW_RATIO 0.76f

#define LABEL_FONT_SIZE 24
#define HINT_FONT_SIZE 20

/*
    These slot rectangles are defined in the ORIGINAL PIXEL SPACE
    of your storage UI image.
*/
static const Rectangle gStorageSlotTemplate[STORAGE_SLOT_COUNT] = {
    {  8,  7, 14, 14 }, { 25,  7, 14, 14 }, { 42,  7, 14, 14 }, { 59,  7, 14, 14 },
    {  8, 24, 14, 14 }, { 25, 24, 14, 14 }, { 42, 24, 14, 14 }, { 59, 24, 14, 14 },
    {  8, 41, 14, 14 }, { 25, 41, 14, 14 }, { 42, 41, 14, 14 }, { 59, 41, 14, 14 },
    {  8, 58, 14, 14 }, { 25, 58, 14, 14 }, { 42, 58, 14, 14 }, { 59, 58, 14, 14 }
};

static float GetStorageUIScale(const Storage *storage)
{
    if (!storage || !storage->uiLoaded || storage->uiTexture.width <= 0 || storage->uiTexture.height <= 0)
        return 1.0f;

    float maxWidth = GetScreenWidth() * STORAGE_SCREEN_FILL;
    float maxHeight = GetScreenHeight() * STORAGE_SCREEN_FILL;

    float scaleX = maxWidth / (float)storage->uiTexture.width;
    float scaleY = maxHeight / (float)storage->uiTexture.height;
    float uniformScale = (scaleX < scaleY) ? scaleX : scaleY;

    if (uniformScale < 1.0f)
        return uniformScale;

    if (uniformScale > STORAGE_MAX_UI_SCALE)
        uniformScale = STORAGE_MAX_UI_SCALE;

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

static void EquipSelectedStorageItem(Game *game, int slotIndex)
{
    if (!game)
        return;

    if (slotIndex < 0 || slotIndex >= STORAGE_SLOT_COUNT)
        return;

    StorageSlot *slot = &game->storage.slots[slotIndex];
    if (slot->item == ITEM_NONE)
        return;

    SetHeldItem(&game->inventory, slot->item);
    printf("Equipped %s from storage\n", GetItemName(slot->item));
}

static int GetHoveredStorageSlot(const Storage *storage, Vector2 mousePos)
{
    if (!storage)
        return -1;

    for (int i = 0; i < STORAGE_SLOT_COUNT; i++)
    {
        if (CheckCollisionPointRec(mousePos, storage->slotRects[i]))
            return i;
    }

    return -1;
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
    storage->slots[7].item = ITEM_BUCKET;
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

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, STORAGE_BG_OVERLAY_ALPHA));

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

    Vector2 mousePos = GetMousePosition();
    int hoveredSlot = GetHoveredStorageSlot(storage, mousePos);

    for (int i = 0; i < STORAGE_SLOT_COUNT; i++)
    {
        Rectangle slot = storage->slotRects[i];
        StorageSlot storageSlot = storage->slots[i];

        bool isSelected = (i == storage->selectedSlot);
        bool isHovered = (i == hoveredSlot);

        if (isHovered)
        {
            DrawRectangleRounded(
                (Rectangle){
                    slot.x - 2.0f,
                    slot.y - 2.0f,
                    slot.width + 4.0f,
                    slot.height + 4.0f
                },
                0.18f,
                6,
                Fade(GOLD, 0.18f)
            );
        }

        if (isSelected)
        {
            DrawRectangleLinesEx(slot, SLOT_SELECTED_THICKNESS, GOLD);
        }
        else if (isHovered)
        {
            DrawRectangleLinesEx(slot, SLOT_BORDER_THICKNESS, (Color){255, 220, 120, 255});
        }
        else
        {
            DrawRectangleLinesEx(slot, SLOT_BORDER_THICKNESS, (Color){125, 88, 58, 255});
        }

        if (storageSlot.item != ITEM_NONE)
        {
            Rectangle src = GetItemSourceRect(storageSlot.item);

            Rectangle dst = {
                slot.x + slot.width * ITEM_INSET_RATIO,
                slot.y + slot.height * ITEM_INSET_RATIO,
                slot.width * ITEM_DRAW_RATIO,
                slot.height * ITEM_DRAW_RATIO
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
            LABEL_FONT_SIZE,
            GOLD
        );
    }
    else
    {
        DrawText(
            "Empty Slot",
            (int)storage->panelRect.x + 24,
            (int)(storage->panelRect.y + storage->panelRect.height + 8),
            LABEL_FONT_SIZE,
            LIGHTGRAY
        );
    }

    DrawText(
        "Mouse: Hover / Click   Arrow Keys: Move   E: Equip   Q: Close",
        (int)storage->panelRect.x,
        (int)(storage->panelRect.y + storage->panelRect.height + 38),
        HINT_FONT_SIZE,
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

    Vector2 mousePos = GetMousePosition();
    int hoveredSlot = GetHoveredStorageSlot(storage, mousePos);

    if (hoveredSlot >= 0)
    {
        storage->selectedSlot = hoveredSlot;

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            EquipSelectedStorageItem(game, hoveredSlot);
        }
    }

    if (IsKeyPressed(KEY_E))
    {
        EquipSelectedStorageItem(game, storage->selectedSlot);
    }
}