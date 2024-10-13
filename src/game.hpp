#pragma once

#include "timer.hpp"
#include "models.hpp"

#include <ncurses.h>
#include <string>
#include <vector>

#define GAME_WIDTH 80
#define GAME_HEIGHT 40
#define PLAYER_OFFSET_X 12
#define BULLET_SPEED 1.5f
#define BULLET_RANGE 80
#define INIT_PLAYER_SPEED 1.5f
#define DASH_SPEED INIT_PLAYER_SPEED * 3.0f
#define DASH_DURATION 150
#define DASH_COOLDOWN 450
#define PLAYER_JUMP_STRENGTH 1.2
#define GROUND_HEIGHT 4
#define GRAVITY 0.1f
#define SHOOT_COOLDOWN 300
#define IMMUNE_TIME 1000

#define TICK_DURATION 40

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

        void print(float camera_x, int game_width, int game_height);
        bool isCollidingWith(const Entity & other);

        static inline void game_mvaddch(int y, int x, chtype ch, int game_width,
                int game_height);
    };

    class Game {
        public:
            int game_width;
            int game_height;
            float camera_x = 0; // sync to player.x

            Entity player;
            float player_dy = 0;
            float player_speed = INIT_PLAYER_SPEED;
            int lives = 3;
            bool can_dash = true;

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
            //inline void game_mvprintw();

            float cur_player_speed();
    };
}
