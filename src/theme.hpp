#ifndef GO_THEMES_H
#define GO_THEMES_H

#include <SDL3/SDL_pixels.h>
#include <vector>

struct GoTheme {
    SDL_Color bg_color;
    SDL_Color board_color;
    SDL_Color board_line_color;
    SDL_Color black_color;
    SDL_Color white_color;

    SDL_Color text_color;
    SDL_Color error_text_color;

    bool hidden_board;
};

class GoThemeHandler {
    static int current_idx;
    static std::vector<GoTheme> themes;

public:
    static void init ();
    static bool loadThemes ();

    static GoTheme getTheme ();
    static void nextTheme ();

    static GoTheme getDefaultTheme ();

    // Prevent instantiation
    GoThemeHandler() = delete;
    GoThemeHandler(const GoThemeHandler&) = delete;
    GoThemeHandler& operator=(const GoThemeHandler&) = delete;
};

#endif
