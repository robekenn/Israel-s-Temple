#include "game_internal.h"

#include <math.h>

static const Color gold = {244, 196, 109, 255};

static void DrawCharacterAt(const Game *game, int station, int outfit)
{
    Vector2 p;
    if (!FindStation(game, station, &p))
        return;
    DrawEllipse((int)p.x, (int)p.y + 5, 12, 5, Fade(BLACK, 0.28f));
    Player npc = {.position = {p.x, p.y - SPRITE_HEIGHT*game->playerScale*0.32f},
                  .direction = DIR_DOWN, .animFrame = 1, .characterIndex = outfit};
    DrawPlayer(npc, game->spriteSheet, game->playerScale);
}

static void DrawCabinet(const Game *game)
{
    Vector2 p;
    if (!FindStation(game, INTERACT_STORAGE, &p))
        return;
    float x = p.x - 16;
    float y = p.y - 15;
    DrawRectangleRec((Rectangle){x+2, y+7, 32, 16}, (Color){46, 28, 33, 255});
    DrawRectangleRec((Rectangle){x, y+3, 32, 15}, (Color){128, 77, 48, 255});
    DrawRectangleRec((Rectangle){x, y, 32, 7}, (Color){190, 125, 72, 255});
    DrawRectangleRec((Rectangle){x+14, y+6, 5, 5}, gold);
}

static void DrawEmbers(Vector2 p, bool incense)
{
    double time = GetTime();
    for (int i = 0; i < 9; i++)
    {
        float age = (float)fmod(time*(incense ? 0.53 : 0.8) + i*0.14, 1.0);
        float x = p.x + sinf(age*5 + i*1.9f)*(4 + 12*age);
        float y = p.y - 11 - age*(incense ? 57 : 40);
        Color color = incense ? (Color){234, 211, 189, 255} : (Color){255, 163, 69, 255};
        DrawCircleV((Vector2){x, y}, incense ? 1.5f + age*4 : 2.2f,
                    Fade(color, (1 - age) * (incense ? 0.45f : 0.85f)));
    }
}

static void DrawBread(const Game *game)
{
    Vector2 p;
    if (!FindStation(game, INTERACT_SHOWBREAD_TABLE, &p))
        return;
    p.y = 10.5f * GetMapTileHeightOnScreen(&game->map);
    for (int i = 0; i < 2; i++)
    {
        DrawEllipse((int)p.x - 5, (int)p.y - 13 + i*12, 9, 5, (Color){133, 68, 38, 255});
        DrawEllipse((int)p.x - 5, (int)p.y - 15 + i*12, 8, 4, gold);
    }
}

static void DrawObjective(const Game *game)
{
    Vector2 p;
    int station = GetObjectiveStation(game);
    if (!FindStation(game, station, &p))
    {
        int door = game->currentMapType == MAP_OUTSIDE ? INTERACT_TABERNACLE_ENTRANCE : INTERACT_TABERNACLE_EXIT;
        if (!FindStation(game, door, &p))
            return;
    }

    p.y -= 27 + 4*sinf((float)GetTime()*4);
    DrawCircleV(p, 15, Fade((Color){34, 22, 33, 255}, 0.88f));
    DrawCircleLines((int)p.x, (int)p.y, 15, gold);
    DrawTriangle((Vector2){p.x-5,p.y-3}, (Vector2){p.x,p.y+5}, (Vector2){p.x+5,p.y-3}, gold);
}

void DrawWorld(const Game *game)
{
    bool inside = game->currentMapType == MAP_INSIDE;
    ServiceDate date = CampaignDate(&game->campaign);
    Vector2 p;
    DrawTileMap(&game->map);
    if (inside)
    {
        DrawCabinet(game);
        if (game->campaign.breadSet)
            DrawBread(game);
    }
    else
    {
        DrawCharacterAt(game, INTERACT_MENTOR, 2);
        DrawCharacterAt(game, INTERACT_VISITOR, game->campaign.shift % 4 + 3);
    }

    DrawEllipse((int)game->player.position.x, (int)GetPlayerInteractionPoint(game->player, game->playerScale).y + 4,
                13, 5, Fade(BLACK, 0.3f));
    DrawPlayer(game->player, game->spriteSheet, game->playerScale);

    if (!inside && date.twilight)
        DrawRectangleGradientV(0, 0, VIEW_WIDTH, VIEW_HEIGHT, (Color){52, 26, 72, 134}, (Color){43, 32, 78, 75});
    if (inside)
        DrawRectangleGradientV(0, 0, VIEW_WIDTH, VIEW_HEIGHT, (Color){28, 18, 42, 100}, (Color){70, 29, 38, 23});

    if (!inside && game->campaign.fireTended && FindStation(game, INTERACT_BRONZE_ALTAR, &p))
        DrawEmbers(p, false);
    if (inside && game->campaign.incenseLit && FindStation(game, INTERACT_INCENSE_ALTAR, &p))
        DrawEmbers(p, true);
    if (inside && game->campaign.lampsLit && FindStation(game, INTERACT_MENORAH, &p))
        DrawCircleGradient((int)p.x, (int)(9.5f*GetMapTileHeightOnScreen(&game->map)), 56,
                           (Color){255, 187, 67, 68}, (Color){255, 187, 67, 0});

    if (game->screen == SCREEN_PLAY && !game->storage.isOpen)
        DrawObjective(game);
}

static void DrawDune(int horizon, float phase, int amplitude, Color color)
{
    for (int x = 0; x < VIEW_WIDTH; x += 8)
    {
        float slope = 0.62f*sinf(x*0.0057f + phase) + 0.38f*sinf(x*0.012f + phase*2);
        int y = (horizon + (int)(amplitude*slope)) / 4 * 4;
        DrawRectangle(x, y, 8, VIEW_HEIGHT-y, color);
    }
}

void DrawTitleScene(void)
{
    DrawRectangleGradientV(0, 0, VIEW_WIDTH, VIEW_HEIGHT, (Color){43, 37, 67, 255}, (Color){211, 126, 92, 255});
    DrawCircle(983, 285, 119, (Color){230, 161, 103, 255});
    DrawCircle(983, 285, 101, (Color){244, 197, 132, 255});
    DrawDune(388, 2.0f, 90, (Color){120, 72, 87, 255});
    DrawDune(464, 4.4f, 74, (Color){97, 63, 79, 255});
    DrawDune(536, 6.1f, 36, (Color){86, 54, 65, 255});

    // Broad woven canopy and the warm opening of the tent.
    for (int x = 744; x < 1176; x += 6)
    {
        int rise = (x < 960 ? x - 744 : 1176 - x) / 2;
        int y = 451 - rise / 4 * 4;
        Color cloth = ((x - 744)/36) % 2 ? (Color){147, 82, 64, 255} : (Color){170, 104, 74, 255};
        DrawRectangle(x, y, 6, 509-y, cloth);
        DrawRectangle(x, y, 6, 5, (Color){213, 150, 96, 255});
    }
    DrawRectangle(757, 507, 406, 85, (Color){65, 42, 55, 255});
    DrawRectangle(757, 507, 406, 9, (Color){201, 136, 84, 255});
    for (int x = 757; x <= 1160; x += 67)
        DrawRectangle(x, 515, 5, 81, (Color){184, 121, 77, 255});
    DrawRectangle(925, 521, 70, 71, (Color){202, 130, 77, 255});
    DrawRectangleGradientV(932, 527, 56, 65, (Color){246, 182, 98, 255}, (Color){130, 71, 57, 255});

    DrawDune(652, 7.6f, 32, (Color){52, 38, 54, 255});
    DrawDune(716, 9.0f, 30, (Color){39, 30, 45, 255});
    DrawLine(656, 605, 1206, 605, (Color){141, 95, 83, 255});
    for (int x = 675; x < 1205; x += 53)
        DrawRectangle(x, 597, 3, 21, (Color){168, 108, 80, 255});
}
