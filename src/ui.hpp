#pragma once

namespace halloween {

// Opens menu screen; blocks execution until selection is made.
// score: score to display; negative number to display nothing
// return true if "play" is selected.
bool menu(long int score);

// Renders screen size warning; does not block execution.
void draw_screen_sz(int screen_width, int screen_height);

}
