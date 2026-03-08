#ifndef CHARACTER_SYSTEM_H
#define CHARACTER_SYSTEM_H

#include "raylib.h"
#include "../MapLoader/MapLoader.h"

// Keep your existing values here.
// These must match ONE FRAME of your character spritesheet.
#define SPRITE_WIDTH 16
#define SPRITE_HEIGHT 20
#define FRAMES_PER_ROW 3
#define DIRECTIONS 4

typedef struct Player {
    Vector2 position;
    float speed;
    int direction;
    int animFrame;
    float animTimer;
    int characterIndex;
} Player;

Rectangle GetPlayerSourceRect(Player player);
void UpdatePlayer(Player *player, TileMap *map, float drawScale);
void DrawPlayer(Player player, Texture2D spriteSheet, float drawScale);

#endif