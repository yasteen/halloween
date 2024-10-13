#include "game.hpp"
#include "ui.hpp"

#include <ncurses.h> 

using namespace halloween;

int main() {

    srand((unsigned)time(0));

    // ncurses init
    initscr();
    curs_set(0);
    noecho();
    cbreak();
    nodelay(stdscr, false);
    timeout(0);

    initialize_textures();

    if (menu(-1)) {
        while(true) {
            Game game;
            unsigned long int score = game.start();
            if(!menu(score)) {
                break;
            }
        }
    }
    wmove(stdscr, 0, 0);
    endwin();
    return 0;
}
