#include "player.h"
#include <SDL2/SDL.h>
#include <math.h>
#include "engine.h"
#include "enemy.h"

void enemy_init(Enemy *enemy, float x, float y)
{
    enemy->body.x = x;
    enemy->body.y = y;
    enemy->body.rotation = 0.0f;
    enemy->body.scale = 1.0f;
    enemy->body.speed = 150.0f; // pixels per second
    enemy->body.vx = 0.0f;
    enemy->body.vy = 0.0f;
    enemy->body.sprite_id = 0;

    enemy->health = 100;
    enemy->mana = 50;
    enemy->level = 1;
}

void enemy_update(Enemy *enemy, float timestep, Map *map)
{
    // Move randomly
    float stepx = (rand() % 3 - 1); // -1, 0, or 1
    float stepy = (rand() % 3 - 1);

    // Normalize input and set velocity
    if (stepx != 0 || stepy != 0)
    {
        float length = sqrtf(stepx * stepx + stepy * stepy);
        stepx /= length;
        stepy /= length;

        enemy->body.vx = stepx * enemy->body.speed;
        enemy->body.vy = stepy * enemy->body.speed;
    }
    else
    {
        enemy->body.vx = 0;
        enemy->body.vy = 0;
    }

    // Calculate new position
    float new_x = enemy->body.x + enemy->body.vx * timestep;
    float new_y = enemy->body.y + enemy->body.vy * timestep;

    // Check X movement collision
    int tile_x = (int)(new_x / TILE_SIZE);
    int tile_y = (int)(enemy->body.y / TILE_SIZE);
    if (tile_x >= 0 && tile_x < map->width && tile_y >= 0 && tile_y < map->height)
    {
        Cell *cell = get_cell(map, tile_x, tile_y);
        if (is_walkable(cell))
        {
            enemy->body.x = new_x;
        }
        if (!is_walkable(cell))
        {
            enemy->body.x -= enemy->body.vx * timestep; // undo the step
        }
    }

    // Check Y movement collision
    tile_x = (int)(enemy->body.x / TILE_SIZE);
    tile_y = (int)(new_y / TILE_SIZE);
    if (tile_x >= 0 && tile_x < map->width && tile_y >= 0 && tile_y < map->height)
    {
        Cell *cell = get_cell(map, tile_x, tile_y);
        if (is_walkable(cell))
        {
            enemy->body.y = new_y;
        }
        if (!is_walkable(cell))
        {
            enemy->body.x -= enemy->body.vx * timestep; // undo the step
        }
    }
}