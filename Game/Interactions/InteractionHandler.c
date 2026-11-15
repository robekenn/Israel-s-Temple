#include "InteractionHandler.h"
#include "../game/game_internal.h"

static bool IsDoor(int station)
{
    return station == INTERACT_TABERNACLE_ENTRANCE || station == INTERACT_TABERNACLE_EXIT;
}

int GetReachableStation(const Game *game)
{
    int x, y, frontX, frontY;
    GetTileCoordsAtWorld(&game->map, GetPlayerInteractionPoint(game->player, game->playerScale), &x, &y);
    GetTileInFront(x, y, game->player.direction, &frontX, &frontY);
    int front = StationAt(game, frontX, frontY);
    const RiteStep *step = CampaignStep(&game->campaign);
    int objective = GetObjectiveStation(game);
    if (IsDoor(front) || front == objective)
        return front;

    static const int offsets[][2] = {{0, 0}, {0, -1}, {-1, 0}, {1, 0}, {0, 1}};
    int nearby = front;
    for (unsigned i = 0; i < sizeof(offsets)/sizeof(*offsets); i++)
    {
        int station = StationAt(game, x + offsets[i][0], y + offsets[i][1]);
        if (!station || IsDoor(station))
            continue;
        if (station == objective || (step && station == step->station && !nearby))
            return station;
        if (!nearby)
            nearby = station;
    }
    return nearby;
}

int GetObjectiveStation(const Game *game)
{
    const RiteStep *step = CampaignStep(&game->campaign);
    if (!step)
        return INTERACT_MENTOR;
    if (step->required != ITEM_KEEP && step->required != game->inventory.heldItem &&
        StorageHasItem(step->required))
        return INTERACT_STORAGE;
    return step->station;
}

const char *StationName(const Game *game, int station)
{
    switch (station)
    {
        case INTERACT_TABERNACLE_ENTRANCE: return "Enter the Holy Place";
        case INTERACT_TABERNACLE_EXIT: return "Return to the courtyard";
        case INTERACT_INCENSE_ALTAR: return "Golden incense altar";
        case INTERACT_MENORAH: return "Lampstand";
        case INTERACT_SHOWBREAD_TABLE: return "Bread table";
        case INTERACT_BRONZE_ALTAR: return "Bronze altar";
        case INTERACT_STORAGE: return "Supply chest";
        case INTERACT_WOOD_STACK: return "Wood stack";
        case INTERACT_LAVER: return "Bronze laver";
        case INTERACT_SACRIFICE_TABLE: return "Preparation table";
        case INTERACT_MENTOR: return "Eleazar";
        case INTERACT_VISITOR: return CampaignShift(&game->campaign)->visitor;
        default: return "";
    }
}

void HandleInteraction(Game *game)
{
    int station = GetReachableStation(game);
    if (!station)
        return;
    if (IsDoor(station))
    {
        if (station == INTERACT_TABERNACLE_ENTRANCE && !game->campaign.washed)
        {
            GameMessage(game, "Wash at the bronze laver before entering the Holy Place.");
            return;
        }
        if (!SwitchMap(game, station == INTERACT_TABERNACLE_ENTRANCE ? MAP_INSIDE : MAP_OUTSIDE))
            GameMessage(game, "The next area could not be loaded. Your progress is safe.");
        return;
    }

    if (station == INTERACT_STORAGE)
    {
        OpenStorage(game);
        return;
    }
    const RiteStep *step = CampaignStep(&game->campaign);
    ItemType held = game->inventory.heldItem;
    ItemType result;
    if (CampaignPerform(&game->campaign, station, held, &result))
    {
        SetHeldItem(&game->inventory, result);
        ApplyPersistentMapState(game);
        GameMessage(game, step->feedback);
        SaveGame(game);
        return;
    }
    if (station == INTERACT_MENTOR)
    {
        if (step)
            GameMessage(game, CampaignShift(&game->campaign)->opening);
        else
            game->screen = SCREEN_HANDOFF;
        return;
    }
    if (station == INTERACT_LAVER && (held == ITEM_BUCKET || held == ITEM_WATER_BUCKET))
    {
        SetHeldItem(&game->inventory, held == ITEM_BUCKET ? ITEM_WATER_BUCKET : ITEM_BUCKET);
        GameMessage(game, held == ITEM_BUCKET ? "You draw water from the laver." : "You return the water to the laver.");
        return;
    }
    if (step && station == step->station)
        GameMessage(game, TextFormat("Bring %s to this station for your current duty.", GetItemName(step->required)));
    else if (station == INTERACT_VISITOR)
        GameMessage(game, "Your visitor will wait here until you are ready to receive their offering.");
    else
        GameMessage(game, step ? step->instruction : "Return to Eleazar near the courtyard gate.");
}
