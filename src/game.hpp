#pragma once

#include "timer.hpp"

#include <ncurses.h>
#include <string>
#include <vector>

#define PLAYER_OFFSET_X 8
#define PLAYER_SPEED 0.8f
#define PLAYER_JUMP_STRENGTH 1.08f
#define GROUND_HEIGHT 4
#define GRAVITY 0.1f
#define SHOOT_COOLDOWN 300

#define TICK_DURATION 40

namespace halloween {

    struct Sprite {
        float x, y;
        int w, h;
        std::vector<std::vector<chtype>> p;
        void print(float camera_x);

        Sprite() : Sprite{0, 0, 0, 0, std::vector<std::string_view>()}{};
        Sprite(float x, float y, int w, int h, std::vector<std::string_view> strs);
    };

    class Game {
        public:
            int game_width;
            int game_height;
            float camera_x = 0; // sync to player.x

            Sprite player;
            float player_dy = 0;

            Timer shoot_timer;
            Timer pumpkin_timer;

            std::vector<Sprite> bullets;
            std::vector<Sprite> pumpkins;

            unsigned int pumpkin_cooldown;

            void start();
            int tick();
            void input();
            void update();
            void draw();
        private:
            void draw_background();
    };
}
