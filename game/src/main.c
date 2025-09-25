#include <SDL2/SDL.h>
#include "engine.h"
#include "player.h"
#include <stdbool.h>
#include "levels.h"
#include "enemy.h"

void render_map_with_camera(Map *map, Camera *camera)
{
    // Calculate visible tile range based on camera position
    float camera_left = camera->x - (camera->screen_width / 2.0f);
    float camera_top = camera->y - (camera->screen_height / 2.0f);
    float camera_right = camera_left + camera->screen_width;
    float camera_bottom = camera_top + camera->screen_height;

    const int TILE_MARGIN = 1; // number of extra tiles around the screen
    int start_x = (int)(camera_left / TILE_SIZE) - TILE_MARGIN;
    int start_y = (int)(camera_top / TILE_SIZE) - TILE_MARGIN;
    int end_x = (int)(camera_right / TILE_SIZE) + TILE_MARGIN;
    int end_y = (int)(camera_bottom / TILE_SIZE) + TILE_MARGIN;

    // Clamp to map bounds
    if (start_x < 0)
        start_x = 0;
    if (start_y < 0)
        start_y = 0;
    if (end_x >= map->width)
        end_x = map->width - 1;
    if (end_y >= map->height)
        end_y = map->height - 1;

    // Render visible tiles
    for (int y = start_y; y <= end_y; y++)
    {
        for (int x = start_x; x <= end_x; x++)
        {
            Cell *cell = get_cell(map, x, y);

            // Convert world position to screen position
            float world_x = x * TILE_SIZE + (TILE_SIZE / 2); // Center of tile
            float world_y = y * TILE_SIZE + (TILE_SIZE / 2);
            float screen_x, screen_y;
            camera_world_to_screen(camera, world_x, world_y, &screen_x, &screen_y);

            RenderCommand tile_cmd = {
                screen_x, screen_y,
                0.0f,            // rotation
                1.0f,            // scale
                cell->tile_type, // sprite_id
                0                // layer (background)
            };

            engine_submit(tile_cmd);
        }
    }
}

int main(void)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    SDL_Window *window = SDL_CreateWindow("ARPG Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    engine_init(renderer);

    Camera camera;
    camera_init(&camera, 1920, 1080);
    // game_init();

    LevelConfig config = load_level_config(2);
    Map *map = create_map(config.width, config.height);
    generate_map(map, config.seed);

    Player player;
    int spawn_x = (config.width * TILE_SIZE) / 2;
    int spawn_y = (config.height * TILE_SIZE) / 2;
    player_init(&player, spawn_x, spawn_y);

    Enemy enemy;
    enemy_init(&enemy, spawn_x + 100, spawn_y + 100);

    const float FIXED_DT = 1.0f / 60.0f;
    float accumulator = 0.0f;
    uint64_t prev = SDL_GetPerformanceCounter();

    bool running = true;
    while (running)
    {
        uint64_t now = SDL_GetPerformanceCounter();
        float frameTime = (now - prev) / (float)SDL_GetPerformanceFrequency();
        prev = now;
        accumulator += frameTime;

        // input
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }

        // fixed updates
        while (accumulator >= FIXED_DT)
        {
            // engine_update(FIXED_DT);
            // game_update(FIXED_DT);
            accumulator -= FIXED_DT;
        }

        engine_begin_frame();
        render_map_with_camera(map, &camera);

        enemy_update(&enemy, frameTime, map);
        player_update(&player, frameTime, &camera, map);
        camera_update(&camera, frameTime, map, player.body.x, player.body.y);

        float player_screen_x, player_screen_y;
        camera_world_to_screen(&camera, player.body.x, player.body.y, &player_screen_x, &player_screen_y);

        RenderCommand player_cmd = {
            player_screen_x,
            player_screen_y,
            player.body.rotation,
            player.body.scale,
            player.body.sprite_id,
            1};
        engine_submit(player_cmd);

        float enemy_screen_x, enemy_screen_y;
        camera_world_to_screen(&camera, enemy.body.x, enemy.body.y, &enemy_screen_x, &enemy_screen_y);

        RenderCommand enemy_cmd = {
            enemy_screen_x,
            enemy_screen_y,
            enemy.body.rotation,
            enemy.body.scale,
            enemy.body.sprite_id,
            1                  
        };
        engine_submit(enemy_cmd);

        engine_end_frame();

        SDL_Delay(1); // prevent 100% CPU usage
    }

    // game_shutdown();
    cleanup_map(map);
    engine_shutdown();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
