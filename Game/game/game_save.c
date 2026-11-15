#include "game_internal.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include <windows.h>
#else
#include <unistd.h>
#endif

void SetSavePath(Game *game)
{
#if defined(_WIN32)
    const char *base = getenv("LOCALAPPDATA");
    const char *suffix = "";
#elif defined(__APPLE__)
    const char *base = getenv("HOME");
    const char *suffix = "/Library/Application Support";
#else
    const char *base = getenv("XDG_DATA_HOME");
    const char *suffix = "";
    if (!base || !base[0])
    {
        base = getenv("HOME");
        suffix = "/.local/share";
    }
#endif
    if (!base || !base[0])
        return;

    char directory[sizeof(game->savePath)];
    int length = snprintf(directory, sizeof(directory), "%s%s/IsraelsTemple", base, suffix);
    if (length < 0 || length >= (int)sizeof(directory) || MakeDirectory(directory) != 0)
        return;

    length = snprintf(game->savePath, sizeof(game->savePath), "%s/campaign.save", directory);
    if (length < 0 || length >= (int)sizeof(game->savePath))
        game->savePath[0] = '\0';
}

bool LoadGame(Game *game)
{
    FILE *file = fopen(game->savePath, "rb");
    if (!file)
        return false;

    int version, shift, duty, step, map, item, dir, ended, freeWatch;
    Vector2 position;
    char extra;
    int count = fscanf(file, " ISRAELS_TEMPLE %d %d %d %d %d %d %d %d %d %f %f %c",
                       &version, &shift, &duty, &step, &map, &item, &dir, &ended, &freeWatch,
                       &position.x, &position.y, &extra);
    fclose(file);

    Campaign campaign;
    if (count != 11 || version != 2 || map < MAP_OUTSIDE || map > MAP_INSIDE ||
        item < ITEM_NONE || item >= ITEM_COUNT || dir < DIR_DOWN || dir > DIR_UP ||
        ended < 0 || ended > 1 || !isfinite(position.x) || !isfinite(position.y) ||
        !CampaignRestore(&campaign, shift, duty, step, freeWatch) ||
        (ended && !CampaignFinished(&campaign)) || (map == MAP_INSIDE && !campaign.washed))
        return false;

    const RiteStep *next = CampaignStep(&campaign);
    if ((item == ITEM_PREPARED_OFFERING || item == ITEM_BREAD || item == ITEM_CENSER_COAL) &&
        (!next || next->required != (ItemType)item))
        return false;

    if (!SwitchMap(game, (MapType)map))
        return false;
    game->campaign = campaign;
    ApplyPersistentMapState(game);
    float feetY = GetPlayerInteractionPoint((Player){.position = position}, game->playerScale).y;
    if (position.x >= 0 && position.x < VIEW_WIDTH && feetY >= 0 && feetY < VIEW_HEIGHT &&
        !IsMapCollisionAt(&game->map, (Vector2){position.x, feetY}))
    {
        game->player.position = position;
        game->player.direction = dir;
    }
    SetHeldItem(&game->inventory, (ItemType)item);
    game->ended = ended != 0;
    game->hasSave = true;
    return true;
}

static bool ReplaceSave(const char *temp, const char *path)
{
#if defined(_WIN32)
    return MoveFileExA(temp, path, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
    return rename(temp, path) == 0;
#endif
}

bool SaveGame(Game *game)
{
    game->saveFailed = true;
    if (!game->savePath[0])
        return false;

#if defined(_WIN32)
    unsigned long process = GetCurrentProcessId();
#else
    unsigned long process = (unsigned long)getpid();
#endif
    char temp[sizeof(game->savePath) + 32];
    snprintf(temp, sizeof(temp), "%s.%lu.tmp", game->savePath, process);
    FILE *file = fopen(temp, "wb");
    if (!file)
        return false;

    const Campaign *campaign = &game->campaign;
    bool wrote = fprintf(file, "ISRAELS_TEMPLE 2\n%d %d %d %d %d %d %d %d\n%.9g %.9g\n",
                        campaign->shift, campaign->duty, campaign->step, game->currentMapType,
                        game->inventory.heldItem, game->player.direction, game->ended, campaign->freeWatch,
                        game->player.position.x, game->player.position.y) > 0;
    bool closed = fclose(file) == 0;
    if (!wrote || !closed || !ReplaceSave(temp, game->savePath))
    {
        remove(temp);
        return false;
    }

    game->saveFailed = false;
    game->hasSave = true;
    return true;
}
