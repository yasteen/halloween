#pragma once

#include <ncurses.h>
#include <string>
#include <vector>

namespace halloween {
    typedef std::vector<std::vector<chtype>> Model;

    extern Model MODEL_PLAYER;
    extern Model MODEL_BULLET;
    extern Model MODEL_PUMPKIN;
    extern Model MODEL_ZOMBIE;

    void initialize_textures();
}
