#include "ui.hpp"

#include "constants.hpp"
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

        int screen_width, screen_height;
        bool open_help = false;

        // 0 = play, 1 = instructions, 2 = exit
        int selection = 0;
        std::string options[] = {"Play", "Instructions", "Quit"};

        while (true) {
            getmaxyx(stdscr, screen_height, screen_width);
            int option_height = screen_height / 2 - MENU_OPTIONS;

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
            erase();
            if (screen_width < GAME_WIDTH + 2 || screen_height < GAME_HEIGHT + 2) {
                draw_screen_sz(screen_width, screen_height);
            } else if (open_help) {
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
                    mvprintw((screen_height - txt_len) / 2 + i,
                            (screen_width - txt[i].size()) / 2, "%s", txt[i].c_str());
                }
            } else {
                if (score >= 0) {
                    std::string score_str = std::to_string(score);
                    mvprintw(option_height - 4,
                            (screen_width - score_str.size() - 13) / 2,
                            "Final score: %s", score_str.c_str());
                }
                if (score < 0 || confirm_timer.getTime() >= CONFIRM_TIMEOUT) {
                    for (int i = 0; i < MENU_OPTIONS; i++) {
                        if (selection == i) attron(COLOR_PAIR(COLOR_SELECT));
                        mvprintw(option_height + 2 * i,
                                screen_width / 2 - options[i].size() / 2, "%s",
                                options[i].c_str());
                        if (selection == i) attroff(COLOR_PAIR(COLOR_SELECT));
                    }
                    std::string help =
                        "Press 'w' and 's' to select an option, 'c' to confirm";
                    mvprintw(screen_height - 1, (screen_width - help.size()) / 2, "%s",
                            help.c_str());
                }
            }
            refresh();

            usleep(TICK_DURATION * 1000);
        }

        return selection == 0;
    }

    void draw_screen_sz(int screen_width, int screen_height) {
        erase();
        std::string sw = std::to_string(screen_width);
        std::string sh = std::to_string(screen_height);
        std::string gw = std::to_string(GAME_WIDTH + 2);
        std::string gh = std::to_string(GAME_HEIGHT + 2);
        mvprintw(screen_height / 2 - 1, (screen_width - 35) / 2,
                "Increase your terminal's dimensions");
        mvprintw(screen_height / 2, (screen_width - 25 - sw.size() - sh.size()) / 2,
                "Terminal dimensions: (%s, %s)", sw.c_str(), sh.c_str());
        mvprintw(screen_height / 2 + 1, (screen_width - 21 - gw.size() - gh.size()) / 2,
                "Game dimensions: (%s, %s)", gw.c_str(), gh.c_str());
    }
}
