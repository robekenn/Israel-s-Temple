#include "game_internal.h"

static bool StartNewCampaign(Game *game)
{
    if (!SwitchMap(game, MAP_OUTSIDE))
    {
        GameMessage(game, "The courtyard map could not be loaded.");
        return false;
    }
    BeginCampaign(game);
    return true;
}

static void StartWatch(Game *game)
{
    game->ended = false;
    SetHeldItem(&game->inventory, ITEM_NONE);
    PlacePlayer(game, OUTSIDE_START_X, OUTSIDE_START_Y, DIR_UP);
    ApplyPersistentMapState(game);
    GameMessage(game, CampaignShift(&game->campaign)->opening);
    game->screen = SCREEN_PLAY;
}

static void AcceptHandoff(Game *game)
{
    if (CampaignFinished(&game->campaign))
    {
        game->ended = true;
        game->screen = SCREEN_ENDING;
    }
    else if (CampaignAdvanceShift(&game->campaign))
        StartWatch(game);
    SaveGame(game);
}

void GameUpdate(Game *game)
{
    if (WindowShouldClose())
    {
        if (game->started)
            SaveGame(game);
        game->shouldCloseGame = true;
        return;
    }

    bool escape = IsKeyPressed(KEY_ESCAPE);
    bool journal = IsKeyPressed(KEY_J);
    bool accept = IsKeyPressed(KEY_ENTER);
    switch (game->screen)
    {
        case SCREEN_TITLE:
            if (game->hasSave && (accept || GameMenuPressed(game, 0)))
            {
                game->started = true;
                game->screen = game->ended ? SCREEN_ENDING : SCREEN_PLAY;
                GameMessage(game, "Your watch resumes.");
            }
            else if (game->hasSave ? GameMenuPressed(game, 1) : (accept || GameMenuPressed(game, 0)))
            {
                if (game->hasSave)
                    game->screen = SCREEN_RESTART;
                else
                    StartNewCampaign(game);
            }
            else if (escape || GameMenuPressed(game, 2))
                game->shouldCloseGame = true;
            return;

        case SCREEN_RESTART:
            if (escape || GameMenuPressed(game, 1))
                game->screen = SCREEN_TITLE;
            else if (accept || GameMenuPressed(game, 0))
                StartNewCampaign(game);
            return;

        case SCREEN_JOURNAL:
            if (escape || journal || GameMenuPressed(game, 2))
                game->screen = SCREEN_PLAY;
            return;

        case SCREEN_PAUSE:
            if (escape || accept || GameMenuPressed(game, 0))
                game->screen = SCREEN_PLAY;
            else if (GameMenuPressed(game, 1) && SaveGame(game))
            {
                game->started = false;
                game->screen = SCREEN_TITLE;
            }
            else if (GameMenuPressed(game, 2) && SaveGame(game))
                game->shouldCloseGame = true;
            return;

        case SCREEN_HANDOFF:
            if (escape || GameMenuPressed(game, 1))
                game->screen = SCREEN_PLAY;
            else if (accept || IsKeyPressed(KEY_E) || GameMenuPressed(game, 0))
                AcceptHandoff(game);
            return;

        case SCREEN_ENDING:
            if (escape || GameMenuPressed(game, 1))
            {
                game->started = false;
                game->screen = SCREEN_TITLE;
            }
            else if ((accept || GameMenuPressed(game, 0)) && CampaignEnterFreeService(&game->campaign))
            {
                StartWatch(game);
                SaveGame(game);
            }
            return;

        case SCREEN_PLAY:
            break;
    }

    if (game->storage.isOpen)
    {
        UpdateStorageWithGame(game);
        return;
    }
    if (escape)
    {
        game->screen = SCREEN_PAUSE;
        return;
    }
    if (journal)
    {
        game->screen = SCREEN_JOURNAL;
        return;
    }

    UpdatePlayer(&game->player, &game->map, game->playerScale);
    if (IsKeyPressed(KEY_E))
        HandleInteraction(game);
}
