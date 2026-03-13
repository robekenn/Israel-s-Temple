#include "Inventory.h"
#include <stdlib.h>

static ItemDefinition gItemDefs[] = {
    { ITEM_NONE,        "None",        { 0,  0, 16, 16 } },
    { ITEM_CENSER,      "Censer",      { 0,  0, 16, 16 } },
    { ITEM_CENSER_COAL, "Coal Censer", { 16, 0, 16, 16 } },
    { ITEM_INCENSE,       "Incense",       { 32, 0, 16, 16 } },
    { ITEM_OIL,       "Oil",       { 48, 0, 16, 16 } },
    { ITEM_WINE,       "Wine",       { 64, 0, 16, 16 } },
    { ITEM_WOOD,       "Wood",       { 80, 0, 16, 16 } },
    { ITEM_FLOUR,       "Flour",       { 96, 0, 16, 16 } },
    { TOOL_BLADE,       "Blade",       { 112, 0, 16, 16 } },
    { TOOL_ASH_BROOM,       "Ash Broom",       { 128, 0, 16, 16 } }
};

static const int gItemDefCount = sizeof(gItemDefs) / sizeof(gItemDefs[0]);

const ItemDefinition *GetItemDefinition(ItemType type)
{
    for (int i = 0; i < gItemDefCount; i++)
    {
        if (gItemDefs[i].type == type)
            return &gItemDefs[i];
    }

    return &gItemDefs[0];
}

const char *GetItemName(ItemType type)
{
    return GetItemDefinition(type)->name;
}

void InitInventory(Inventory *inventory)
{
    if (inventory == NULL)
        return;

    inventory->heldItem = ITEM_NONE;
    inventory->occupied = false;
}

bool AddItemToInventory(Inventory *inventory, ItemType type)
{
    if (inventory == NULL)
        return false;

    if (type == ITEM_NONE)
        return false;

    if (inventory->occupied)
        return false; // player can only hold one item

    inventory->heldItem = type;
    inventory->occupied = true;
    return true;
}

bool RemoveHeldItem(Inventory *inventory)
{
    if (inventory == NULL)
        return false;

    if (!inventory->occupied)
        return false;

    inventory->heldItem = ITEM_NONE;
    inventory->occupied = false;
    return true;
}

bool HasHeldItem(const Inventory *inventory, ItemType type)
{
    if (inventory == NULL)
        return false;

    return inventory->occupied && inventory->heldItem == type;
}
void SetHeldItem(Inventory *inventory, ItemType type)
{
    if (inventory == NULL)
        return;

    if (type == ITEM_NONE)
    {
        inventory->heldItem = ITEM_NONE;
        inventory->occupied = false;
    }
    else
    {
        inventory->heldItem = type;
        inventory->occupied = true;
    }
}

void DrawInventoryUI(const Inventory *inventory, Texture2D uiTexture, Texture2D itemSheet)
{
    const float margin = 20.0f;
    const float scale = 6.0f;

    const float uiWidth = (float)uiTexture.width * scale;
    const float uiHeight = (float)uiTexture.height * scale;

    const float uiX = margin;
    const float uiY = (float)GetScreenHeight() - uiHeight - margin;

    Rectangle srcRect = {
        0.0f, 0.0f,
        (float)uiTexture.width,
        (float)uiTexture.height
    };

    Rectangle destRect = {
        uiX, uiY,
        uiWidth, uiHeight
    };

    DrawTexturePro(uiTexture, srcRect, destRect, (Vector2){0, 0}, 0.0f, WHITE);

    if (inventory == NULL || !inventory->occupied)
        return;

    const ItemDefinition *def = GetItemDefinition(inventory->heldItem);

    float iconSize = uiHeight * 0.55f;
    if (uiWidth < uiHeight)
        iconSize = uiWidth * 0.55f;

    Rectangle iconDest = {
        uiX + (uiWidth - iconSize) * 0.5f,
        uiY + (uiHeight - iconSize) * 0.5f,
        iconSize,
        iconSize
    };

    DrawTexturePro(itemSheet, def->sourceRect, iconDest, (Vector2){0, 0}, 0.0f, WHITE);
}