#include "player.h"
#include <SDL2/SDL.h>
#include <math.h>

void player_init(Player *player, float x, float y)
{
    player->body.x = x;
    player->body.y = y;
    player->body.rotation = 0.0f;
    player->body.scale = 1.0f;
    player->body.speed = 200.0f; // pixels per second
    player->body.vx = 0.0f;
    player->body.vy = 0.0f;
    player->body.sprite_id = 0; // Assuming 0 is the player sprite

    player->health = 100;
    player->mana = 50;
    player->level = 1;
    player->experience = 0;
}

void player_update(Player *player, float time_step)
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

    player->body.x += player->body.vx * time_step;
    player->body.y += player->body.vy * time_step;

    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    stepx = (float)mouse_x - player->body.x;
    stepy = (float)mouse_y - player->body.y;
    player->body.rotation = atan2f(stepy, stepx) * (180.0f / 3.14159f);
}
