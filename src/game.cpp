#include "game.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <iostream>
#include <ncurses.h>
#include <unistd.h>

namespace halloween {

    unsigned int rand_range(unsigned int min, unsigned int max, unsigned int div) {
        assert(max >= min);
        return min + ((rand() % (div + 1)) * (max - min)) / (div + 1);
    }

    void Entity::print(float camera_x) {
        assert(p != NULL);
        assert(p->size() > frame);
        int x_cur = 0;
        int y_cur = 0;
        for (size_t i = 0; i < (*p)[frame].size(); i++) {
            char c = (*p)[frame][i];
            if (c == '\n') {
                x_cur = 0;
                y_cur++;
                continue;
            }
            if (c != ' ') {
                mvaddch(y + y_cur + offset_y, x - camera_x + x_cur + offset_x, c);
            }
            x_cur++;
        }
    }

    bool Entity::isCollidingWith(const Entity & other) {
        bool x_overlap = other.x < x + w && other.x + other.w > x;
        bool y_overlap = other.y < y + w && other.y + other.w > y;
        return x_overlap && y_overlap;
    }

    void Game::start() {
        srand((unsigned)time(0));

        initialize_textures();

        // ncurses init
        initscr();
        curs_set(0);
        noecho();
        cbreak();
        nodelay(stdscr, false);
        timeout(0);
        getmaxyx(stdscr, game_height, game_width);
        player = Entity(PLAYER_OFFSET_X, game_height - GROUND_HEIGHT - 4, 4, 4, -1, 0,
                &MODEL_PLAYER);

        // Init spawn timers
        pumpkin_cooldown = rand_range(2, 6, 8) * 1000;
        zombie_cooldown = rand_range(2, 5, 5) * 1000;
        while (tick());
    }

    int Game::tick() {
        getmaxyx(stdscr, game_height, game_width);

        input();

        update();
        camera_x = player.x - PLAYER_OFFSET_X;
        draw();

        refresh();
        usleep(TICK_DURATION * 1000);

        return 1;
    }

    void Game::input() {
        int x = getch();
        if (x == 'q') {
            wmove(stdscr, 0, 0);
            endwin();
            exit(0);
        } else if (x == 'x') {
            if (shoot_timer.getTime() > SHOOT_COOLDOWN) {
                bullets.emplace_back(player.x + 5, player.y + 2, 2, 1, &MODEL_BULLET);
                shoot_timer.reset();
            }
        } else if (x == 'z') {
            if (player_dy == 0)
                player_dy = -PLAYER_JUMP_STRENGTH;
        }
        flushinp();
    }

    void Game::update() {
        // Player movement
        player.x += PLAYER_SPEED;
        float ground_level = game_height - GROUND_HEIGHT;
        player_dy = player_dy + GRAVITY;
        player.y = std::clamp(player.y + player_dy, 0.0f, ground_level - player.h);
        if (player.y + player.h == ground_level) {
            player_dy = 0;
        }

        // Bullet
        for (auto & b : bullets) {
            b.x += 2;
        }
        auto b_criterion = [this](Entity b){ return b.x > camera_x + game_width + 5; };
        std::erase_if(bullets, b_criterion);

        // Spawn
        // * Pumpkin
        if (pumpkin_timer.getTime() > pumpkin_cooldown) {
            pumpkins.emplace_back(player.x + game_width, game_height - GROUND_HEIGHT - 3,
                    6, 3, &MODEL_PUMPKIN);
            pumpkin_timer.reset();
            pumpkin_cooldown = rand_range(2000, 5000, 4);
        }

        // * Zombie
        if (zombie_timer.getTime() > zombie_cooldown) {
            zombies.emplace_back(player.x + game_width, game_height - GROUND_HEIGHT - 4,
                    4, 4, -2, 0, &MODEL_ZOMBIE);
            zombie_timer.reset();
            zombie_cooldown = rand_range(1000, 2000, 5);
        }

        // Collision
        auto player_collide = [this](Entity e){ return player.isCollidingWith(e); };
        auto off_screen_l = [this](Entity e){ return e.x < camera_x; };
        auto is_dead = [](Entity e){ return !e.alive; };
        // * Off screen
        std::erase_if(pumpkins, off_screen_l);
        std::erase_if(zombies, off_screen_l);
        // * Pumpkin
        unsigned long int num_pumpkins = pumpkins.size();
        std::erase_if(pumpkins, player_collide);
        if (pumpkins.size() < num_pumpkins) lives--;
        // * Zombie
        unsigned long int num_zombies = zombies.size();
        std::erase_if(zombies, player_collide);
        if (zombies.size() < num_zombies) lives--;
        // ** Zombie/bullet
        for (auto & z : zombies) {
            for (auto & b : bullets) {
                if (z.isCollidingWith(b)) {
                    z.alive = false;
                    b.alive = false;
                }
            }
        }
        std::erase_if(zombies, is_dead);
        std::erase_if(bullets, is_dead);
    }

    void Game::draw() {
        clear();
        draw_background();
        player.print(camera_x);
        for (auto & b : bullets) {
            b.print(camera_x);
        }
        for (auto & p : pumpkins) {
            p.print(camera_x);
        }
        for (auto & z : zombies) {
            z.print(camera_x);
        }

        mvprintw(0, 0, "Score: %lu", (unsigned long int) std::round(camera_x));
        mvprintw(1, 0, "Lives: %d", lives);
    }

    void Game::draw_background() {
        std::vector<chtype> groundArray(game_width, '-' | A_NORMAL | COLOR_PAIR(0));
        for (int i = 0; i < game_width; i++) {
            if ((long int)((i + (long int) std::round(player.x))) % 8 == 0) {
                groundArray[i] = '*' | A_NORMAL | COLOR_PAIR(0);
            }
        }
        mvaddchstr(game_height - GROUND_HEIGHT, 0, &groundArray[0]);
    }
}

