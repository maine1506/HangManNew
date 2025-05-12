#ifndef _GRAPHICS__H
#define _GRAPHICS__H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "defs.h"
#include "logic.h"
#include "button.h"

using namespace std;

struct Graphics {
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_Texture* background;
    vector<SDL_Texture*> stage;

    void init() {
        initSDL();
        loadTextures();
    }

    void loadTextures() {
        background = loadTexture("assets/background.png");
        for (int i = 0; i < MAX_BAD_GUESSES; i++) {
            string path = "assets/st" + to_string(i) + ".png";
            stage.push_back(loadTexture(path.c_str()));
        }
    }

    void renderInputText(const string& text, int x, int y) {
        SDL_Color black = {0, 0, 0, 255};
        SDL_Texture* texture = renderText(text.c_str(), "assets/sarifa.ttf", 26, black);
        renderTexture(texture, x, y);
        SDL_DestroyTexture(texture);
    }

    void renderQuestion(const string& s, const Hangman& game, bool showButtons = false) {
        prepareScene(background);
        renderTexture(stage[game.badGuessCount], 0, 0);

        SDL_Color black = {0, 0, 0, 255};
        SDL_Texture* msg = renderText(s.c_str(), "assets/sarifa.ttf", 26, black);
        renderTexture(msg, 170, 150);
        SDL_DestroyTexture(msg);

        if (showButtons) {
            renderButtons();
        }

        presentScene();
}

    void renderButtons() {
        SDL_Color black = {0, 0, 0, 255}; // Black text for buttons
        SDL_Texture* yesText = renderText("YES", "assets/sarifa.ttf", 26, black);
        SDL_Texture* noText  = renderText("NO", "assets/sarifa.ttf", 26, black);

        // Draw button rectangles
        SDL_SetRenderDrawColor(renderer, 0, 128, 0, 255); // Green
        SDL_RenderFillRect(renderer, &yesButton.rect);
        SDL_SetRenderDrawColor(renderer, 128, 0, 0, 255); // Red
        SDL_RenderFillRect(renderer, &noButton.rect);

        // Draw button text
        renderTextureMid(yesText, yesButton.rect.x + yesButton.rect.w / 2, yesButton.rect.y + yesButton.rect.h / 2);
        renderTextureMid(noText, noButton.rect.x + noButton.rect.w / 2, noButton.rect.y + noButton.rect.h / 2);

        SDL_DestroyTexture(yesText);
        SDL_DestroyTexture(noText);
    }

    void logErrorAndExit(const char* msg, const char* error) {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
                       "%s: %s", msg, error);
        SDL_Quit();
    }

    void initSDL() {
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
            logErrorAndExit("SDL_Init", SDL_GetError());

        window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
                   SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT,
                   SDL_WINDOW_SHOWN);
        if (window == nullptr)
                   logErrorAndExit("CreateWindow", SDL_GetError());

        if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))
            logErrorAndExit("SDL_image error:", IMG_GetError());

        renderer = SDL_CreateRenderer(window, -1,
                     SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        if (renderer == nullptr)
             logErrorAndExit("CreateRenderer", SDL_GetError());

        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
        SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

        if (TTF_Init() == -1) {
            logErrorAndExit("SDL_ttf could not initialize! SDL_ttf Error: ",
                             TTF_GetError());
        }
    }

    void prepareScene(SDL_Texture * background) {
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, background, NULL, NULL);
    }

    void presentScene() {
        SDL_RenderPresent(renderer);
    }

    TTF_Font* loadFont(const char* path, int size) {
        TTF_Font* gFont = TTF_OpenFont(path, size);
        if (gFont == nullptr) {
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION,
                           SDL_LOG_PRIORITY_ERROR,
                           "Load font %s", TTF_GetError());
        }
        return gFont;
    }

    SDL_Texture* renderText(const char* text, const char* path,
                            int size, SDL_Color textColor)
    {
        TTF_Font* font = loadFont(path, size);
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, text, textColor);
        if (textSurface == nullptr) {
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION,
                           SDL_LOG_PRIORITY_ERROR,
                           "Render text surface %s", TTF_GetError());
            TTF_CloseFont(font);
            return nullptr;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (texture == nullptr) {
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION,
                           SDL_LOG_PRIORITY_ERROR,
                           "Create texture from text %s", SDL_GetError());
        }

        SDL_FreeSurface(textSurface);
        TTF_CloseFont(font);
        return texture;
    }

    SDL_Texture *loadTexture(const char *filename) {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
                       "Loading %s", filename);
        SDL_Texture *texture = IMG_LoadTexture(renderer, filename);
        if (texture == NULL)
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION,
                  SDL_LOG_PRIORITY_ERROR, "Load texture %s", IMG_GetError());
        return texture;
    }

    void renderTexture(SDL_Texture *texture, int x, int y) {
        SDL_Rect dest;
        dest.x = x;
        dest.y = y;
        SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
        SDL_RenderCopy(renderer, texture, NULL, &dest);
    }

    void renderTextureMid(SDL_Texture *texture, int x, int y) {
        SDL_Rect dest;
        SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
        dest.x = x - dest.w / 2;
        dest.y = y - dest.h / 2;
        SDL_RenderCopy(renderer, texture, NULL, &dest);
    }

    void quit() {
        TTF_Quit();
        IMG_Quit();
        for (auto tex : stage) SDL_DestroyTexture(tex);
        SDL_DestroyTexture(background);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

#endif // _GRAPHICS__H
