#pragma once

#include "timer.hpp"
#include "models.hpp"

#include <ncurses.h>
#include <string>
#include <vector>

#define PLAYER_OFFSET_X 12
#define PLAYER_SPEED 0.9f
#define PLAYER_JUMP_STRENGTH 1.2
#define GROUND_HEIGHT 4
#define GRAVITY 0.1f
#define SHOOT_COOLDOWN 300

#define TICK_DURATION 40

namespace halloween {

    unsigned int rand_range(unsigned int min, unsigned int max, unsigned int div);

    struct Entity {
        float x, y;
        int w, h;
        int offset_x, offset_y;
        Model* p;
        bool alive = true;
        unsigned int frame = 0;

        Entity() : Entity{0, 0, 0, 0, NULL}{}
        Entity(float x, float y, int w, int h, Model* p) :
            Entity{x, y, w, h, 0, 0, p}{}
        Entity(float x, float y, int w, int h, int offset_x, int offset_y, Model* p):
            x{x}, y{y}, w{w}, h{h}, offset_x{offset_x}, offset_y{offset_y}, p{p}{};

        void print(float camera_x);
        bool isCollidingWith(const Entity & other);
    };

    class Game {
        public:
            int game_width;
            int game_height;
            float camera_x = 0; // sync to player.x

            Entity player;
            float player_dy = 0;
            int lives = 3;

            Timer shoot_timer;
            Timer pumpkin_timer;
            Timer zombie_timer;

            std::vector<Entity> bullets;
            std::vector<Entity> pumpkins;
            std::vector<Entity> zombies;

            unsigned int pumpkin_cooldown;
            unsigned int zombie_cooldown;

            void start();
            int tick();
            void input();
            void update();
            void draw();
        private:
            void draw_background();
    };
}
