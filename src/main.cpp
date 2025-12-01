#include "base.hpp"
#include "board.hpp"
#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_surface.h>
#include <iostream>
#include <cmath>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_ttf/SDL_textengine.h>
#include "config.hpp"
#include "sound.hpp"
#include "test.hpp"

int main(int argc, char **argv) {
    if (!isTestPassed()) {
        return -1;
    }

    GoGameConfig::init("./config.json");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        std::cerr << "Init error : " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    if (!TTF_Init()) {
        std::cerr << "Font init error : " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    TTF_Font* font = TTF_OpenFont("assets/fonts/Lexend.ttf", 12);
    if (!font) {
        std::cerr << "Font load error : " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    SDL_Window* window = SDL_CreateWindow(
        "MCE Circle Demo",
        720, 405,
        SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        std::cerr << "Window creation error : " << SDL_GetError() << std::endl;
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "Renderer error : " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    TTF_TextEngine* text_engine = TTF_CreateRendererTextEngine(renderer);
    if (!text_engine) {
        std::cerr << "TextEngine creation failed: " << SDL_GetError() << "\n";
        return EXIT_FAILURE;
    }

    if (!GoSound::init()) {
        std::cerr << "Sound initialization failed: " << SDL_GetError() << "\n";
        return EXIT_FAILURE;
    }

    if (!GoSound::loadMusicFiles()) {
        std::cerr << "Music files loading failed: " << SDL_GetError() << "\n";
    }

    bool isRunning = true;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    GoBoard board(renderer, w, h, GoBoardSize::_9x9);
    board.setupTextEngine(text_engine, font);

    while (isRunning) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                isRunning = false;
                continue;
            }

            board.handleEvent(&event);
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 20, 20, 25, 255);
        SDL_RenderClear(renderer);

        int w, h;
        SDL_GetWindowSize(window, &w, &h);

        board.updateBoardInfo(w, h);
        board.render();

        board.renderUI();

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    GoSound::destroy();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}
