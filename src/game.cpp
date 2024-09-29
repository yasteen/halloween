#include "game.hpp"

#include "models.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <iostream>
#include <ncurses.h>
#include <unistd.h>

namespace halloween {
    void Sprite::print(float camera_x) {
        for (size_t i = 0; i < p.size(); i++) {
            if (p[i].size() > 0) {
                mvaddchnstr(y + i, x - camera_x, &p[i][0], p[i].size());
            }
        }
    }

    Sprite::Sprite(float x, float y, int w, int h, std::vector<std::string_view> strs):
        x{x}, y{y}, w{w}, h{h} {

        for (const std::string_view& str : strs) {
            int i = 0;
            if (str.length() == 0) return;
            p.emplace_back();
            for (auto & c : str) {
                if (c == '\n') {
                    p.emplace_back();
                    i++;
                } else {
                    p[i].emplace_back(c | A_NORMAL | COLOR_PAIR(0));
                }
            }
        }
    }

    void Game::start() {
        std::cout <<sizeof(std::vector<Sprite>)<<std::endl;
        srand((unsigned)time(0));

        // ncurses init
        initscr();
        curs_set(0);
        noecho();
        cbreak();
        nodelay(stdscr, false);
        timeout(0);
        getmaxyx(stdscr, game_height, game_width);
        player = Sprite(PLAYER_OFFSET_X, game_height - GROUND_HEIGHT - 4, 5, 4,
                std::vector<std::string_view>{{MODEL_player}});

        // Init spawn timers
        pumpkin_cooldown = ((rand() % 6) + 1) * 1000;
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
                bullets.emplace_back(player.x + 5, player.y + 2, 2, 1,
                        std::vector<std::string_view>{{MODEL_bullet}});
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
        auto criterion = [this](Sprite b){ return b.x > camera_x + game_width + 5; };
        std::erase_if(bullets, criterion);

        // Spawn
        if (pumpkin_timer.getTime() > pumpkin_cooldown) {
            pumpkins.emplace_back(player.x + game_width, game_height - GROUND_HEIGHT - 3,
                    6, 3, std::vector<std::string_view>{{MODEL_pumpkin}});
            pumpkin_timer.reset();
            pumpkin_cooldown = ((rand() % 3)) * 1000 + 2 * 1000;
        }
        auto p_criterion = [this](Sprite b){ return b.x < camera_x; };
        std::erase_if(pumpkins, p_criterion);
    }

    void Game::draw() {
        clear();
        draw_background();
        mvprintw(0, 0, "Score: %lu", (unsigned long int) std::round(camera_x));
        player.print(camera_x);
        for (auto & b : bullets) {
            b.print(camera_x);
        }
        for (auto & p : pumpkins) {
            p.print(camera_x);
        }
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

