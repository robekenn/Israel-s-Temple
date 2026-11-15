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
    { TOOL_ASH_BROOM,       "Ash Broom",       { 128, 0, 16, 16 } },
    { ITEM_BUCKET,             "Bucket",       { 144, 0, 16, 16 } },
    { ITEM_WATER_BUCKET,       "Water Bucket", { 160, 0, 16, 16 } },
    { ITEM_PREPARED_OFFERING,  "Prepared offering", { 0, 0, 0, 0 } },
    { ITEM_BREAD,              "Twelve loaves", { 0, 0, 0, 0 } }
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

void DrawItemIcon(ItemType type, Texture2D itemSheet, Rectangle dest)
{
    if (type == ITEM_NONE)
        return;
    if (type == ITEM_PREPARED_OFFERING || type == ITEM_BREAD)
    {
        const Color bronze = {154, 87, 50, 255};
        const Color grain = {239, 189, 112, 255};
        const Color dark = {67, 37, 34, 255};
        float p = dest.width / 16.0f;
        DrawRectangleRec((Rectangle){dest.x + 2*p, dest.y + 10*p, 12*p, 4*p}, dark);
        DrawRectangleRec((Rectangle){dest.x + 3*p, dest.y + 9*p, 10*p, 3*p}, bronze);
        if (type == ITEM_BREAD)
        {
            for (int i = 0; i < 3; i++)
            {
                float x = dest.x + (3 + 4*i)*p;
                DrawRectangleRec((Rectangle){x, dest.y + 5*p, 3*p, 5*p}, grain);
                DrawRectangleRec((Rectangle){x + p, dest.y + 6*p, p, 2*p}, bronze);
            }
        }
        else
        {
            DrawRectangleRec((Rectangle){dest.x + 4*p, dest.y + 6*p, 5*p, 3*p}, grain);
            DrawRectangleRec((Rectangle){dest.x + 10*p, dest.y + 5*p, 3*p, 5*p},
                             (Color){160, 62, 55, 255});
        }
        return;
    }
    DrawTexturePro(itemSheet, GetItemSourceRect(type), dest, (Vector2){0}, 0, WHITE);
}

Rectangle GetItemSourceRect(ItemType type)
{
    return GetItemDefinition(type)->sourceRect;
}
