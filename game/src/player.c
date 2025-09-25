#include "player.h"
#include <SDL2/SDL.h>
#include <math.h>
#include "engine.h"

void player_init(Player *player, float x, float y)
{
    player->body.x = x;
    player->body.y = y;
    player->body.rotation = 0.0f;
    player->body.scale = 1.0f;
    player->body.speed = 500.0f; // pixels per second
    player->body.vx = 0.0f;
    player->body.vy = 0.0f;
    player->body.sprite_id = 0;

    player->health = 100;
    player->mana = 50;
    player->level = 1;
    player->experience = 0;
}

void player_update(Player *player, float timestep, Camera *camera, Map *map)
{
    const Uint8 *keyboard = SDL_GetKeyboardState(NULL);
    float stepx = 0.0f;
    float stepy = 0.0f;

    if (keyboard[SDL_SCANCODE_W])
        stepy -= 1.0f;
    if (keyboard[SDL_SCANCODE_S])
        stepy += 1.0f;
    if (keyboard[SDL_SCANCODE_A])
        stepx -= 1.0f;
    if (keyboard[SDL_SCANCODE_D])
        stepx += 1.0f;

    // Normalize input and set velocity
    if (stepx != 0 || stepy != 0)
    {
        float length = sqrtf(stepx * stepx + stepy * stepy);
        stepx /= length;
        stepy /= length;

        player->body.vx = stepx * player->body.speed;
        player->body.vy = stepy * player->body.speed;
    }
    else
    {
        player->body.vx = 0;
        player->body.vy = 0;
    }

    // Calculate new position (only declare once!)
    float new_x = player->body.x + player->body.vx * timestep;
    float new_y = player->body.y + player->body.vy * timestep;

    // Check X movement collision
    int tile_x = (int)(new_x / TILE_SIZE);
    int tile_y = (int)(player->body.y / TILE_SIZE);
    if (tile_x >= 0 && tile_x < map->width && tile_y >= 0 && tile_y < map->height)
    {
        Cell *cell = get_cell(map, tile_x, tile_y);
        if (is_walkable(cell))
        {
            player->body.x = new_x;
        }
    }

    // Check Y movement collision
    tile_x = (int)(player->body.x / TILE_SIZE);
    tile_y = (int)(new_y / TILE_SIZE);
    if (tile_x >= 0 && tile_x < map->width && tile_y >= 0 && tile_y < map->height)
    {
        Cell *cell = get_cell(map, tile_x, tile_y);
        if (is_walkable(cell))
        {
            player->body.y = new_y;
        }
    }

    // Convert mouse screen coordinates to world coordinates
    int mouse_screen_x, mouse_screen_y;
    SDL_GetMouseState(&mouse_screen_x, &mouse_screen_y);

    float mouse_world_x, mouse_world_y;
    camera_screen_to_world(camera, mouse_screen_x, mouse_screen_y, &mouse_world_x, &mouse_world_y);

    // Calculate direction vector from player (world pos) to mouse (world pos)
    stepx = mouse_world_x - player->body.x;
    stepy = mouse_world_y - player->body.y;

    // Calculate rotation angle in degrees
    player->body.rotation = atan2f(stepy, stepx) * (180.0f / 3.14159f);
}

int can_move_to_with_size(Map *map, float center_x, float center_y, float player_radius)
{
    // Check multiple points around the player
    float offsets[] = {-player_radius, 0, player_radius};

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            float check_x = center_x + offsets[i];
            float check_y = center_y + offsets[j];

            int tile_x = (int)(check_x / TILE_SIZE);
            int tile_y = (int)(check_y / TILE_SIZE);

            if (tile_x < 0 || tile_x >= map->width || tile_y < 0 || tile_y >= map->height)
            {
                return 0;
            }

            Cell *cell = get_cell(map, tile_x, tile_y);
            if (!is_walkable(cell))
            {
                return 0;
            }
        }
    }
    return 1;
}