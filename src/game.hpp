#pragma once

#include "constants.hpp"
#include "timer.hpp"
#include "models.hpp"

#include <ncurses.h>
#include <vector>


namespace halloween {

    unsigned int rand_range(unsigned int min, unsigned int max, unsigned int div);

    struct Entity {
        float x, y;
        float dx;
        int w, h;
        int offset_x, offset_y;
        Model* p;
        bool alive = true;
        unsigned int frame = 0;

        Entity() : Entity{0, 0, 0, 0, 0, NULL}{}
        Entity(float x, float y, float dx, int w, int h, Model* p) :
            Entity{x, y, dx, w, h, 0, 0, p}{}
        Entity(float x, float y, float dx, int w, int h, int offset_x, int offset_y,
                Model* p):
            x{x}, y{y}, dx{dx}, w{w}, h{h}, offset_x{offset_x}, offset_y{offset_y},
            p{p}{};

        void print(float camera_x, int screen_width, int screen_height);
        bool isCollidingWith(const Entity & other);

        static inline void game_mvaddch(int y, int x, chtype ch, int screen_width,
                int screen_height);
    };

    class Game {
        public:
            int screen_width;
            int screen_height;
            float camera_x = 0; // sync to player.x

            Entity player;
            float player_dy = 0;
            float player_speed = INIT_PLAYER_SPEED;
            int lives = 3;
            float dash_start = 0;

            // Timers
            Timer game_timer;
            Timer immune_timer;
            Timer dash_timer;
            Timer shoot_timer;
            Timer bat_timer;
            Timer pumpkin_timer;
            Timer zombie_timer;
            unsigned int bat_cooldown;
            unsigned int pumpkin_cooldown;
            unsigned int zombie_cooldown;

            // Other entities
            std::vector<Entity> bullets;

            std::vector<Entity> bats;
            std::vector<Entity> pumpkins;
            std::vector<Entity> zombies;

            unsigned long int start();
        private:
            int tick();
            void input();

            void update();
            void spawn_enemies();

            void draw();
            void draw_background();
            void draw_border();

            float cur_player_speed();
    };
}
