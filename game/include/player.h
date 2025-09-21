#ifndef PLAYER_H
#define PLAYER_H

#include "body.h"
#include <SDL2/SDL.h>
#include "engine.h"

typedef struct Player {
    Body body;
    int health;
    int mana;
    int level;
    int experience;
} Player;

void player_init(Player *player, float x, float y);
void player_update(Player *player, float dt);
void player_update_with_camera(Player *player, float dt, Camera *camera);

#endif