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
void player_update(Player *player, float timestep, Camera *camera, Map *map);
int can_move_to_with_size(Map *map, float x, float y, float player_radius);

#endif