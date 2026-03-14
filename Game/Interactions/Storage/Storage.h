#ifndef STORAGE_H
#define STORAGE_H

#include <stdbool.h>
#include "../../src/raylib.h"
#include "../../Inventory/Inventory.h"

#define STORAGE_ROWS 4
#define STORAGE_COLS 4
#define STORAGE_SLOT_COUNT (STORAGE_ROWS * STORAGE_COLS)

typedef struct StorageSlot {
    ItemType item;
} StorageSlot;

typedef struct Storage {
    bool isOpen;
    int selectedSlot;

    Texture2D uiTexture;
    bool uiLoaded;

    Rectangle panelRect;
    Rectangle slotRects[STORAGE_SLOT_COUNT];

    StorageSlot slots[STORAGE_SLOT_COUNT];
} Storage;

void InitStorage(Storage *storage, const char *uiTexturePath);
void UnloadStorage(Storage *storage);

void OpenStorage(Storage *storage);
void CloseStorage(Storage *storage);
void ToggleStorage(Storage *storage);

void UpdateStorage(Storage *storage);
void DrawStorage(const Storage *storage, Texture2D itemSheet);

struct Game;
void HandleStorageInteraction(struct Game *game);
void UpdateStorageWithGame(struct Game *game);

#endif