#include "ui.hpp"

#include "models.hpp"
#include "timer.hpp"

#include <ncurses.h>
#include <string>
#include <unistd.h>

#define TICK_DURATION 40
#define MENU_OPTIONS 3
#define CONFIRM_TIMEOUT 1500

namespace halloween {

    bool menu(long int score) {
        Timer confirm_timer;

        int game_width, game_height;
        bool open_help = false;

        // 0 = play, 1 = instructions, 2 = exit
        int selection = 0;
        std::string options[] = {"Play", "Instructions", "Quit"};

        while (true) {
            getmaxyx(stdscr, game_height, game_width);
            int option_height = game_height / 2 - MENU_OPTIONS;

            // Input
            int x = getch();
            if (open_help) {
                if (x != -1) {
                    open_help = false;
                }
            } else {
                if (x == 'w') {
                    selection = (selection - 1) % MENU_OPTIONS;
                } else if (x == 's') {
                    selection = (selection + 1) % MENU_OPTIONS;
                } else if (x == 'z' || x == 'c' || x == 'x') {
                    if (score < 0 || confirm_timer.getTime() >= CONFIRM_TIMEOUT) {
                        if (selection != 1) break;
                        open_help = true;
                    }
                } else if (x == 'q') {
                    return 0;
                }
            }
            flushinp();

            // Print
            clear();
            if (open_help) {
                std::string txt[] = {
                    "October 31",
                    "",
                    "You're running at night at the park.",
                    "You see monsters appear in the dark.",
                    "Press 'c' to dash,",
                    "And 'x'  to shoot,",
                    "And 'z' to jump in an arc.",
                };
                int txt_len = sizeof(txt) / sizeof(std::string);
                for (int i = 0; i < txt_len; i++) {
                    mvprintw((game_height - txt_len) / 2 + i,
                            (game_width - txt[i].size()) / 2, "%s", txt[i].c_str());
                }
            } else {
                if (score >= 0) {
                    std::string score_str = std::to_string(score);
                    mvprintw(option_height - 4, (game_width - score_str.size() - 13) / 2,
                            "Final score: %s", score_str.c_str());
                }
                if (score < 0 || confirm_timer.getTime() >= CONFIRM_TIMEOUT) {
                    for (int i = 0; i < MENU_OPTIONS; i++) {
                        if (selection == i) attron(COLOR_PAIR(COLOR_SELECT));
                        mvprintw(option_height + 2 * i,
                                game_width / 2 - options[i].size() / 2, "%s",
                                options[i].c_str());
                        if (selection == i) attroff(COLOR_PAIR(COLOR_SELECT));
                    }
                    std::string help =
                        "Press 'w' and 's' to select an option, 'c' to confirm";
                    mvprintw(game_height - 1, (game_width - help.size()) / 2, "%s",
                            help.c_str());
                }
            }
            refresh();

            usleep(TICK_DURATION * 1000);
        }

        return selection == 0;
    }
}
