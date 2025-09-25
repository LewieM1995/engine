#ifndef ENEMY_H
#define ENEMY_H

#include "body.h"
#include <SDL2/SDL.h>
#include "engine.h"

typedef struct Enemy {
    Body body;
    int health;
    int mana;
    int level;
} Enemy;

void enemy_init(Enemy *enemy, float x, float y);
void enemy_update(Enemy *enemy, float timestep, Map *map);

#endif