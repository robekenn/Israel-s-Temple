#ifndef STORAGE_H
#define STORAGE_H

#include "../../Inventory/Inventory.h"

typedef struct Storage {
    bool isOpen;
    int selectedSlot;
} Storage;

struct Game;

void OpenStorage(struct Game *game);
void UpdateStorageWithGame(struct Game *game);
void DrawStorage(const struct Game *game);
bool StorageHasItem(ItemType item);

#endif
