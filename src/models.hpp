#pragma once

#include <ncurses.h>
#include <string>
#include <vector>

#define COLOR_HURT 1
#define COLOR_SELECT 15

namespace halloween {
    struct Texture {
        std::vector<chtype> tex;
        unsigned int color;

        Texture(std::vector<chtype> tex): tex{tex}, color{0} {};
        Texture(std::vector<chtype> tex, unsigned int color): tex{tex}, color{color} {};
    };

    typedef std::vector<Texture> Model;

    extern Model MODEL_PLAYER;
    extern Model MODEL_BULLET;
    extern Model MODEL_PUMPKIN;
    extern Model MODEL_ZOMBIE;
    extern Model MODEL_BAT;

    void initialize_textures();
}
