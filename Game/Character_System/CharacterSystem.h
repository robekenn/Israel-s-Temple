#ifndef CHARACTER_SYSTEM_H
#define CHARACTER_SYSTEM_H

#include "../src/raylib.h"



#define SPRITE_WIDTH 16
#define SPRITE_HEIGHT 20
#define FRAMES_PER_ROW 3
#define DIRECTIONS 4

typedef struct Player {
    Vector2 position;
    int characterIndex;   // 0 through 7
    int direction;        // 0=down, 1=left, 2=right, 3=up
    int animFrame;        // 0,1,2
    float animTimer;
    float speed;
} Player;

Rectangle GetPlayerSourceRect(Player player);

void UpdatePlayer(Player *player);

void DrawPlayer(Player player, Texture2D spriteSheet, float drawScale);

#endif