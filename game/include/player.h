#ifndef PLAYER_H
#define PLAYER_H

#include "body.h"
#include <SDL2/SDL.h>

typedef struct {
    Body body;
    int health;
    int mana;
    int level;
    int experience;
} Player;

void player_init(Player *player, float x, float y);
void player_update(Player *player, float dt);
//void player_render(Player *player);

#endif