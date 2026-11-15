#include "game_internal.h"

#include <string.h>

static const Color ink = {31, 24, 34, 250};
static const Color field = {52, 39, 50, 255};
static const Color light = {255, 239, 210, 255};
static const Color muted = {219, 196, 178, 255};
static const Color gold = {231, 184, 110, 255};

static void Panel(Rectangle area)
{
    DrawRectangleRec((Rectangle){area.x+4, area.y+5, area.width, area.height}, Fade(BLACK, 0.30f));
    DrawRectangleRec(area, ink);
    DrawRectangleLinesEx(area, 1, Fade(gold, 0.66f));
    DrawRectangleRec((Rectangle){area.x, area.y, 4, area.height}, gold);
}

static void WrapText(const char *text, int x, int y, int width, int size, Color color)
{
    char line[256] = {0};
    size_t used = 0;
    while (*text)
    {
        size_t length = strcspn(text, " ");
        size_t previous = used;
        if (used)
            line[used++] = ' ';
        if (used + length >= sizeof(line))
        {
            line[previous] = '\0';
            DrawText(line, x, y, size, color);
            return;
        }
        memcpy(line+used, text, length);
        used += length;
        line[used] = '\0';
        if (previous && MeasureText(line, size) > width)
        {
            line[previous] = '\0';
            DrawText(line, x, y, size, color);
            memcpy(line, text, length);
            line[length] = '\0';
            used = length;
            y += size + 7;
        }
        text += length;
        while (*text == ' ')
            text++;
    }
    if (used)
        DrawText(line, x, y, size, color);
}

static Rectangle MenuButton(int index)
{
    return (Rectangle){104, 440 + index*60, 330, 48};
}

static Rectangle HandoffButton(int index)
{
    return (Rectangle){384 + index*267, 489, 245, 48};
}

static const Rectangle journalButton = {866, 604, 231, 46};

static Rectangle ActiveButton(GameScreen screen, int index)
{
    if (screen == SCREEN_JOURNAL)
        return journalButton;
    if (screen == SCREEN_HANDOFF || screen == SCREEN_ENDING)
        return HandoffButton(index);
    return MenuButton(index);
}

bool GameMenuPressed(const Game *game, int index)
{
    return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
           CheckCollisionPointRec(GetMousePosition(), ActiveButton(game->screen, index));
}

static void Button(Rectangle rect, const char *label, bool primary)
{
    bool hover = CheckCollisionPointRec(GetMousePosition(), rect);
    Color fill = primary ? (hover ? light : gold) : (hover ? (Color){88, 61, 61, 255} : field);
    Color text = primary ? ink : light;
    DrawRectangleRec(rect, fill);
    DrawRectangleLinesEx(rect, hover ? 2 : 1, gold);
    DrawText(label, (int)rect.x + 15, (int)rect.y + 12, 23, text);
}

static void DateText(ServiceDate date, int x, int y, int size, Color color)
{
    DrawText(TextFormat("Year %d / Month %d / Day %d", date.year, date.month, date.day),
             x, y, size, color);
}

static void DrawHud(const Game *game)
{
    const Campaign *campaign = &game->campaign;
    const Shift *shift = CampaignShift(campaign);
    const Duty *duty = CampaignDuty(campaign);
    const RiteStep *step = CampaignStep(campaign);
    ServiceDate date = CampaignDate(campaign);

    Panel((Rectangle){22, 22, 396, 216});
    DrawText(game->currentMapType == MAP_OUTSIDE ? "COURTYARD" : "HOLY PLACE", 40, 37, 16, gold);
    DrawText(shift->name, 40, 63, 29, light);
    DrawText(duty ? duty->name : "Return to Eleazar", 40, 104, 21, gold);
    WrapText(step ? step->instruction : "Speak to Eleazar near the courtyard gate to close the watch.",
             40, 134, 355, 19, light);
    int duties = CampaignDutyCount(campaign);
    for (int i = 0; i < duties; i++)
    {
        int width = 348 / duties;
        DrawRectangle(40 + i*width, 217, width-5, 4,
                      i < campaign->duty ? gold : Fade(gold, i == campaign->duty ? 0.56f : 0.20f));
    }

    Panel((Rectangle){912, 22, 346, 126});
    DrawText(date.twilight ? "TWILIGHT WATCH" : "DAWN WATCH", 929, 37, 16, gold);
    DateText(date, 929, 63, 20, light);
    DrawText(TextFormat("%s  /  %s", CalendarOccasion(date), shift->rank), 929, 99, 18, muted);

    Panel((Rectangle){22, 664, 380, 76});
    DrawItemIcon(game->inventory.heldItem, game->itemSpriteSheet, (Rectangle){36, 679, 47, 47});
    DrawText("IN HAND", 92, 676, 14, gold);
    DrawText(game->inventory.occupied ? GetItemName(game->inventory.heldItem) : "Empty", 92, 697, 24, light);

    int station = GetReachableStation(game);
    Panel((Rectangle){893, 664, 365, 76});
    if (station)
    {
        DrawRectangle(907, 678, 43, 34, gold);
        DrawText("E", 921, 684, 22, ink);
        DrawText(StationName(game, station), 961, 685, 19, light);
    }
    else
        DrawText("WASD or arrows to move", 908, 681, 19, light);
    DrawText("J  Journal       Esc  Pause       Shift  Run", 908, 718, 15, muted);

    if (game->message[0])
    {
        Panel((Rectangle){428, 664, 440, 76});
        WrapText(game->message, 443, 679, 408, 18, light);
    }
    if (game->saveFailed)
        DrawText("Autosave failed", 1090, 154, 17, (Color){255, 174, 150, 255});
}

static void DrawJournal(const Game *game)
{
    const Campaign *campaign = &game->campaign;
    ServiceDate date = CampaignDate(campaign);
    Panel((Rectangle){147, 101, 986, 570});
    DrawText("Watch journal", 182, 127, 44, light);
    DateText(date, 185, 188, 21, gold);
    DrawText(date.twilight ? "Twilight" : "Dawn", 540, 191, 18, muted);
    DrawLine(184, 228, 1096, 228, Fade(gold, 0.4f));

    int count = CampaignDutyCount(campaign);
    for (int i = 0; i < count; i++)
    {
        const Duty *duty = CampaignDutyAt(campaign, i);
        int y = 263 + i*40;
        DrawRectangleLinesEx((Rectangle){185, y, 20, 20}, 1, gold);
        if (i < campaign->duty)
            DrawRectangle(189, y+4, 12, 12, gold);
        DrawText(duty->name, 222, y-2, 21, i == campaign->duty ? gold : light);
        DrawText(duty->reference, 222, y+20, 13, muted);
    }

    DrawText(CalendarOccasion(date), 672, 265, 27, light);
    DrawText(TextFormat("Day %d of %d   /   Weekday %d of 7", date.day, CalendarMonthLength(date.month),
                        date.weekday), 672, 309, 20, muted);
    for (int day = 1; day <= CalendarMonthLength(date.month); day++)
    {
        int x = 673 + ((day-1) % 10)*40;
        int y = 351 + ((day-1) / 10)*43;
        DrawRectangle(x, y, 34, 34, day == date.day ? gold : field);
        DrawText(TextFormat("%d", day), x + (day < 10 ? 12 : 7), y+8, 17, day == date.day ? ink : light);
    }
    const char *note = campaign->freeWatch < 0 ? "The story follows selected watches in the calendar." :
                       "Evening follows dawn. The calendar advances after the evening watch.";
    WrapText(note, 673, 505, 400, 18, muted);

    const char *summary = campaign->freeWatch < 0 ?
        TextFormat("Story watch %d of %d", campaign->shift + 1, CampaignShiftCount()) : "Free service";
    DrawText(summary, 185, 616, 20, gold);
    Button(journalButton, "Close journal", false);
}

static void DrawHandoff(const Game *game)
{
    bool ended = game->screen == SCREEN_ENDING;
    Panel((Rectangle){354, 225, 572, 337});
    DrawText(ended ? "CAMPAIGN COMPLETE" : "WATCH COMPLETE", 383, 250, 16, gold);
    DrawText(ended ? "The Ledger Is Yours" : CampaignShift(&game->campaign)->name,
             383, 278, 38, light);
    const char *message = ended ?
        "You served from your first dawn to the Festival of Booths. The watch is yours to keep as the calendar turns." :
        CampaignShift(&game->campaign)->closing;
    WrapText(message, 383, 334, 502, 21, light);

    Campaign next = game->campaign;
    bool nextExists = ended ? CampaignEnterFreeService(&next) : CampaignAdvanceShift(&next);
    if (nextExists)
    {
        ServiceDate date = CampaignDate(&next);
        DrawText(date.twilight ? "NEXT WATCH AT TWILIGHT" : "NEXT WATCH AT DAWN", 383, 425, 15, gold);
        DateText(date, 383, 449, 18, muted);
    }
    Button(HandoffButton(0), ended ? "Keep serving" : (CampaignFinished(&game->campaign) ? "Receive the ledger" : "Begin next watch"), true);
    Button(HandoffButton(1), ended ? "Title screen" : "Back to courtyard", false);
}

static void DrawMenu(const Game *game)
{
    if (game->screen == SCREEN_TITLE || game->screen == SCREEN_RESTART)
        DrawTitleScene();
    DrawRectangleGradientH(0, 0, VIEW_WIDTH, VIEW_HEIGHT, (Color){27, 19, 35, 190}, (Color){66, 43, 57, 0});
    Panel((Rectangle){61, 99, 548, 556});
    DrawRectangle(104, 162, 110, 3, gold);

    if (game->screen == SCREEN_TITLE)
    {
        DrawText("THE WATCH AT THE TABERNACLE", 104, 133, 16, gold);
        DrawText("Israel's", 101, 189, 78, light);
        DrawText("Temple", 101, 266, 78, light);
        WrapText("Tend the fire and the lamps. Serve the people who come to the courtyard.",
                 104, 355, 430, 21, muted);
        Button(MenuButton(0), game->hasSave ? "Continue" : "Begin the first watch", true);
        if (game->hasSave)
            Button(MenuButton(1), "New game", false);
        Button(MenuButton(2), "Quit", false);
        return;
    }

    DrawText(game->screen == SCREEN_RESTART ? "NEW GAME" : "WATCH PAUSED", 104, 133, 16, gold);
    DrawText(game->screen == SCREEN_RESTART ? "Start over?" : "Paused", 103, 203, 64, light);
    if (game->screen == SCREEN_RESTART)
    {
        WrapText("Starting the first watch replaces your existing campaign save.", 104, 300, 405, 23, muted);
        Button(MenuButton(0), "Start a new campaign", true);
        Button(MenuButton(1), "Back", false);
    }
    else
    {
        DateText(CampaignDate(&game->campaign), 104, 316, 24, muted);
        if (game->saveFailed)
            WrapText("Your save could not be written. You can retry from here.", 104, 364, 414, 20, gold);
        Button(MenuButton(0), "Resume watch", true);
        Button(MenuButton(1), "Save and return to title", false);
        Button(MenuButton(2), "Save and quit", false);
    }
}

void GameDraw(const Game *game)
{
    DrawWorld(game);
    if (game->screen == SCREEN_PLAY)
    {
        DrawHud(game);
        DrawStorage(game);
    }
    else if (game->screen == SCREEN_JOURNAL)
    {
        DrawRectangle(0, 0, VIEW_WIDTH, VIEW_HEIGHT, Fade(ink, 0.70f));
        DrawJournal(game);
    }
    else if (game->screen == SCREEN_HANDOFF || game->screen == SCREEN_ENDING)
    {
        DrawRectangle(0, 0, VIEW_WIDTH, VIEW_HEIGHT, Fade(ink, 0.72f));
        DrawHandoff(game);
    }
    else
        DrawMenu(game);
}
