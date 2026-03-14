#ifndef INVENTORY_H
#define INVENTORY_H

#include "raylib.h"
#include <stdbool.h>

typedef enum ItemType {
    ITEM_NONE = 0,
    ITEM_CENSER,
    ITEM_CENSER_COAL,
    ITEM_INCENSE,
    ITEM_OIL,
    ITEM_WINE,
    ITEM_WOOD,
    ITEM_FLOUR,
    TOOL_BLADE,
    TOOL_ASH_BROOM
} ItemType;

typedef struct ItemDefinition {
    ItemType type;
    const char *name;
    Rectangle sourceRect;
} ItemDefinition;

typedef struct Inventory {
    ItemType heldItem;
    bool occupied;
} Inventory;

void InitInventory(Inventory *inventory);
bool AddItemToInventory(Inventory *inventory, ItemType type);
bool RemoveHeldItem(Inventory *inventory);
bool HasHeldItem(const Inventory *inventory, ItemType type);
void SetHeldItem(Inventory *inventory, ItemType type);

void DrawInventoryUI(const Inventory *inventory, Texture2D uiTexture, Texture2D itemSheet);

const ItemDefinition *GetItemDefinition(ItemType type);
const char *GetItemName(ItemType type);
Rectangle GetItemSourceRect(ItemType type);

#endif