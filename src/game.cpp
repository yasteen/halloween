#include "game.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <iostream>
#include <ncurses.h>
#include <unistd.h>

namespace halloween {

    inline unsigned int rand_range(unsigned int min, unsigned int max, unsigned int div) {
        assert(max >= min);
        return min + ((rand() % (div + 1)) * (max - min)) / (div + 1);
    }

    void Entity::print(float camera_x, int game_width, int game_height) {
        assert(p != NULL);
        assert(p->size() > frame);
        int x_cur = 0;
        int y_cur = 0;
        attron(COLOR_PAIR((*p)[frame].color));
        for (size_t i = 0; i < (*p)[frame].tex.size(); i++) {
            chtype c = (*p)[frame].tex[i];
            if ((unsigned char) c == '\n') {
                x_cur = 0;
                y_cur++;
                continue;
            }
            if ((unsigned char) c != ' ') {
                game_mvaddch(y + y_cur + offset_y, x - camera_x + x_cur + offset_x, c,
                        game_width, game_height);
            }
            x_cur++;
        }
        attroff(COLOR_PAIR((*p)[frame].color));
    }

    bool Entity::isCollidingWith(const Entity & other) {
        bool x_overlap = other.x < x + w && other.x + other.w > x;
        bool y_overlap = other.y < y + h && other.y + other.h > y;
        return x_overlap && y_overlap;
    }

    inline void Entity::game_mvaddch(int y, int x, chtype ch, int game_width,
            int game_height) {
        if (x >= 0 && x < GAME_WIDTH && y >= 0 && y < GAME_HEIGHT) {
            mvaddch((game_height - GAME_HEIGHT) / 2 + y,
                    (game_width - GAME_WIDTH) / 2 + x, ch);
        }
    }

    unsigned long int Game::start() {
        getmaxyx(stdscr, game_height, game_width);
        player = Entity(PLAYER_OFFSET_X, GAME_HEIGHT - GROUND_HEIGHT - 4,
                INIT_PLAYER_SPEED, 4, 4, -1, 0, &MODEL_PLAYER);

        // Init spawn timers
        pumpkin_cooldown = rand_range(2, 6, 8) * 1000;
        zombie_cooldown = rand_range(2, 5, 5) * 1000;
        immune_timer.setTimeRelativeToNow(-IMMUNE_TIME * 2);
        dash_timer.setTimeRelativeToNow(-DASH_COOLDOWN * 2);
        while (tick());
        return (long int) std::round(camera_x);
    }

    int Game::tick() {
        getmaxyx(stdscr, game_height, game_width);

        input();

        update();
        camera_x = player.x - PLAYER_OFFSET_X;
        draw();

        refresh();
        usleep(TICK_DURATION * 1000);

        return lives > 0;
    }

    void Game::input() {
        int x = getch();
        if (x == 'q') {
            wmove(stdscr, 0, 0);
            endwin();
            exit(0);
        } else if (x == 'x') {
            if (shoot_timer.getTime() > SHOOT_COOLDOWN) {
                bullets.emplace_back(player.x + 5, player.y + 2,
                        BULLET_SPEED + player_speed, 2, 1, &MODEL_BULLET);
                
                shoot_timer.reset();
            }
        } else if (x == 'z') {
            if (player_dy == 0)
                player_dy = -PLAYER_JUMP_STRENGTH;
        } else if (x == 'c') {
            if (dash_timer.getTime() > DASH_COOLDOWN) {
                dash_timer.reset();
            }
        }
        flushinp();
    }

    void Game::update() {
        // Player movement
        if (dash_timer.getTime() < DASH_DURATION) {
            player.dx = DASH_SPEED + game_timer.getTime() / 1000 * 0.1f;
            player_dy = 0;
        } else {
            player.dx = cur_player_speed();
            player_dy = player_dy + GRAVITY;
        }
        player.x += player.dx;
        float ground_level = GAME_HEIGHT - GROUND_HEIGHT;
        player.y = std::clamp(player.y + player_dy, 0.0f, ground_level - player.h);
        if (player.y + player.h == ground_level) {
            player_dy = 0;
        }

        spawn_enemies();

        // Bullet
        for (auto & b : bullets) {
            b.x += b.dx;
        }


        // --- Collision ---
        auto player_collide = [this](Entity e){ return player.isCollidingWith(e); };
        auto off_screen_l = [this](Entity e){ return e.x < camera_x; };
        auto is_dead = [](Entity e){ return !e.alive; };
        auto bullet_off_screen = [this](Entity b){
            return b.x > std::clamp(camera_x + GAME_WIDTH, 0.0f,
                    camera_x + BULLET_RANGE);
        };

        // * Object off screen
        std::erase_if(bats, off_screen_l);
        std::erase_if(pumpkins, off_screen_l);
        std::erase_if(zombies, off_screen_l);
        std::erase_if(bullets, bullet_off_screen);

        // * Player damaged
        unsigned long int num_bats = bats.size();
        unsigned long int num_pumpkins = pumpkins.size();
        unsigned long int num_zombies = zombies.size();
        std::erase_if(bats, player_collide);
        std::erase_if(pumpkins, player_collide);
        std::erase_if(zombies, player_collide);
        if ((bats.size() < num_bats ||
                    pumpkins.size() < num_pumpkins ||
                    zombies.size() < num_zombies)
                && immune_timer.getTime() > IMMUNE_TIME) {
            lives--;
            immune_timer.reset();
        }

        // * bullet collision
        for (auto & bat : bats) {
            for (auto & b : bullets) {
                if (bat.isCollidingWith(b)) {
                    bat.alive = false;
                    b.alive = false;
                }
            }
        }
        for (auto & z : zombies) {
            for (auto & b : bullets) {
                if (z.isCollidingWith(b)) {
                    z.alive = false;
                    b.alive = false;
                }
            }
        }
        std::erase_if(bats, is_dead);
        std::erase_if(zombies, is_dead);
        std::erase_if(bullets, is_dead);
    }

    void Game::spawn_enemies() {
        // Bat
        if (bat_timer.getTime() > bat_cooldown) {
            bats.emplace_back(player.x + GAME_WIDTH,
                    GAME_HEIGHT - GROUND_HEIGHT - 4 - player.h - 0.01f, -0.7f, 5, 4, 0,
                    0, &MODEL_BAT);
            bat_timer.reset();
            bat_cooldown = rand_range(3000, 6500, 9);
        }
        // Pumpkin
        if (pumpkin_timer.getTime() > pumpkin_cooldown) {
            pumpkins.emplace_back(player.x + GAME_WIDTH, GAME_HEIGHT - GROUND_HEIGHT - 3,
                    0, 6, 3, &MODEL_PUMPKIN);
            pumpkin_timer.reset();
            pumpkin_cooldown = rand_range(2000, 5000, 4);
        }

        // Zombie
        if (zombie_timer.getTime() > zombie_cooldown) {
            zombies.emplace_back(player.x + GAME_WIDTH, GAME_HEIGHT - GROUND_HEIGHT - 4,
                    -0.3f, 4, 4, -2, 0, &MODEL_ZOMBIE);
            zombie_timer.reset();
            zombie_cooldown = rand_range(1000, 2000, 5);
        }
    }

    void Game::draw() {
        erase();
        draw_background();

        player.frame = immune_timer.getTime() <= IMMUNE_TIME ? 1 : 0;
        player.print(camera_x, game_width, game_height);

        for (auto & b : bullets) {
            b.print(camera_x, game_width, game_height);
        }
        for (auto & b : bats) {
            b.print(camera_x, game_width, game_height);
        }
        for (auto & p : pumpkins) {
            p.print(camera_x, game_width, game_height);
        }
        for (auto & z : zombies) {
            z.print(camera_x, game_width, game_height);
        }

        draw_border();
        mvprintw(0, 0, "Score: %lu", (unsigned long int) std::round(camera_x));
        mvprintw(1, 0, "Lives: %d", lives);
        mvprintw(2, 0, "Speed: %f", player_speed);
    }

    void Game::draw_background() {
        std::vector<chtype> groundArray(GAME_WIDTH, '-' | A_NORMAL | COLOR_PAIR(0));
        for (int i = 0; i < GAME_WIDTH; i++) {
            if ((long int)((i + (long int) std::round(player.x))) % 8 == 0) {
                groundArray[i] = '*' | A_NORMAL | COLOR_PAIR(0);
            }
        }
        mvaddchnstr((game_height + GAME_HEIGHT) / 2 - GROUND_HEIGHT,
                (game_width - GAME_WIDTH) / 2, &groundArray[0], GAME_WIDTH);
    }

    void Game::draw_border() {
        mvhline((game_height - GAME_HEIGHT) / 2 - 1,
                (game_width - GAME_WIDTH) / 2, '-', GAME_WIDTH);
        mvhline((game_height + GAME_HEIGHT) / 2,
                (game_width - GAME_WIDTH) / 2, '-', GAME_WIDTH);
        mvvline((game_height - GAME_HEIGHT) / 2,
                (game_width - GAME_WIDTH) / 2 - 1, '|', GAME_HEIGHT);
        mvvline((game_height - GAME_HEIGHT) / 2,
                (game_width + GAME_WIDTH) / 2 + 1, '|', GAME_HEIGHT);
    }

    float Game::cur_player_speed() {
        return INIT_PLAYER_SPEED + game_timer.getTime() / 1000 * .01f;
    }
}

