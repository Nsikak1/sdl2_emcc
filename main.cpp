#include <SDL.h>
#include <SDL_ttf.h>
#include <cstdio>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

struct App
{
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* textTexture = nullptr;
    bool running = true;
};

static App app;

// --------------------------------------------------
// Main loop (frame function)
// --------------------------------------------------
void main_loop()
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT)
            app.running = false;

        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
            app.running = false;
    }

    SDL_SetRenderDrawColor(app.renderer, 20, 20, 20, 255);
    SDL_RenderClear(app.renderer);

    if (app.textTexture) {
        SDL_Rect dst{50, 50, 0, 0};
        SDL_QueryTexture(app.textTexture, nullptr, nullptr, &dst.w, &dst.h);
        SDL_RenderCopy(app.renderer, app.textTexture, nullptr, &dst);
    }

    SDL_RenderPresent(app.renderer);

    if (!app.running) {
#ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop();
#endif
        SDL_DestroyTexture(app.textTexture);
        SDL_DestroyRenderer(app.renderer);
        SDL_DestroyWindow(app.window);
        TTF_Quit();
        SDL_Quit();
    }
}

// --------------------------------------------------
// Entry point
// --------------------------------------------------
int main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    // Console output
    std::puts("Starting App");

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::printf("SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() != 0) {
        std::printf("TTF_Init failed: %s\n", TTF_GetError());
        return 1;
    }

    app.window = SDL_CreateWindow("App Title",
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  640,
                                  480,
                                  SDL_WINDOW_SHOWN);

    app.renderer = SDL_CreateRenderer(app.window,
                                      -1,
                                      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Load font (must exist)
    TTF_Font* font = TTF_OpenFont("font.ttf", 32);
    if (!font) {
        std::printf("Failed to load font: %s\n", TTF_GetError());
        return 1;
    }

    SDL_Color pink{255, 30, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, "WELCOME", pink);
    SDL_Rect dest;
    dest.x = 100;            // Horizontal position from left
    dest.y = 120;            // Vertical position from top
    dest.w = textSurface->w; // Use actual text width
    dest.h = textSurface->h; // Use actual text height

    app.textTexture = SDL_CreateTextureFromSurface(app.renderer, textSurface);
    SDL_RenderCopy(app.renderer, app.textTexture, NULL, &dest);

    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, true);
#else
    while (app.running) {
        main_loop();
        SDL_Delay(16);
    }
#endif

    return 0;
}
