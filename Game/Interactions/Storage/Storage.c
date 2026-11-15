#include "Storage.h"
#include "../../game/game_internal.h"

enum { COLS = 4, COUNT = 9 };

static const ItemType stock[COUNT] = {
    ITEM_CENSER, ITEM_INCENSE, ITEM_OIL, ITEM_WINE,
    ITEM_FLOUR, TOOL_BLADE, TOOL_ASH_BROOM, ITEM_BUCKET, ITEM_NONE
};

static const Rectangle panel = {192, 139, 896, 506};
static const Rectangle closeButton = {965, 167, 92, 36};
static const Color gold = {231, 189, 115, 255};
static const Color paper = {249, 232, 198, 255};

bool StorageHasItem(ItemType item)
{
    for (int i = 0; i < COUNT; i++)
        if (stock[i] == item)
            return true;
    return false;
}

void OpenStorage(Game *game)
{
    ItemType held = game->inventory.heldItem;
    if (held == ITEM_CENSER_COAL || held == ITEM_BREAD || held == ITEM_PREPARED_OFFERING)
    {
        GameMessage(game, "Finish the current duty before exchanging what you are carrying.");
        return;
    }
    const RiteStep *step = CampaignStep(&game->campaign);
    ItemType wanted = step ? step->required : held;
    game->storage = (Storage){.isOpen = true};
    for (int i = 0; i < COUNT; i++)
        if (stock[i] == wanted)
            game->storage.selectedSlot = i;
}

static Rectangle Slot(int index)
{
    return (Rectangle){224 + (index % COLS)*210, 263 + (index / COLS)*108, 198, 96};
}

void UpdateStorageWithGame(Game *game)
{
    Storage *storage = &game->storage;
    bool click = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    Vector2 mouse = GetMousePosition();

    if (IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_ESCAPE) ||
        (click && CheckCollisionPointRec(mouse, closeButton)))
    {
        storage->isOpen = false;
        return;
    }

    int next = storage->selectedSlot;
    if (IsKeyPressed(KEY_RIGHT)) next++;
    if (IsKeyPressed(KEY_LEFT)) next--;
    if (IsKeyPressed(KEY_DOWN)) next += COLS;
    if (IsKeyPressed(KEY_UP)) next -= COLS;
    storage->selectedSlot = (next + COUNT) % COUNT;

    if (click)
    {
        for (int i = 0; i < COUNT; i++)
        {
            if (!CheckCollisionPointRec(mouse, Slot(i)))
                continue;
            storage->selectedSlot = i;
            break;
        }
        click = CheckCollisionPointRec(mouse, Slot(storage->selectedSlot));
    }
    if (!click && !IsKeyPressed(KEY_E) && !IsKeyPressed(KEY_ENTER))
        return;

    ItemType item = stock[storage->selectedSlot];
    SetHeldItem(&game->inventory, item);
    GameMessage(game, item == ITEM_NONE ? "You put your item away." :
                TextFormat("You take %s from the supplies.", GetItemName(item)));
    storage->isOpen = false;
}

void DrawStorage(const Game *game)
{
    if (!game->storage.isOpen)
        return;

    DrawRectangle(0, 0, VIEW_WIDTH, VIEW_HEIGHT, Fade(BLACK, 0.75f));
    DrawRectangleRec(panel, (Color){37, 29, 39, 255});
    DrawRectangleLinesEx(panel, 2, gold);
    DrawText("Supply chest", 224, 167, 37, paper);
    DrawText("Choose what to carry. You have room for one item.", 224, 217, 21, paper);
    DrawRectangleLinesEx(closeButton, 1, gold);
    DrawText("Close", 980, 175, 21, paper);

    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < COUNT; i++)
    {
        Rectangle slot = Slot(i);
        bool selected = i == game->storage.selectedSlot;
        bool hover = CheckCollisionPointRec(mouse, slot);
        DrawRectangleRec(slot, selected || hover ? (Color){84, 58, 56, 255} : (Color){54, 42, 51, 255});
        DrawRectangleLinesEx(slot, selected || hover ? 2 : 1, selected || hover ? gold : Fade(gold, 0.4f));
        DrawItemIcon(stock[i], game->itemSpriteSheet, (Rectangle){slot.x + 8, slot.y + 16, 55, 55});
        DrawText(stock[i] == ITEM_NONE ? "Put item away" : GetItemName(stock[i]),
                 (int)slot.x + (stock[i] == ITEM_NONE ? 13 : 66), (int)slot.y + 36, 17, paper);
    }

    DrawText("Arrows to choose   /   E or click to take   /   Q to close", 448, 607, 19, gold);
}
