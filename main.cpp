#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "logic.h"
#include "graphics.h"
#include "defs.h"
#include "button.h"

using namespace std;

enum GameState {
    INPUT_LENGTH,
    GUESS_LETTER,
    INPUT_COUNT,
    GUESS_WORD,
    GAME_OVER
};

string inputText = "";

void updateGameState(Hangman& game, Graphics& graphics, GameState& state) {
    if (game.isGameOver()) {
        string msg;
        if (game.possibleWords.empty()) {
            msg = "I lost! No possible words left.";
        } else if (game.badGuessCount >= MAX_BAD_GUESSES) {
            msg = "I lost! Too many wrong guesses.";
        } else if (game.possibleWords.size() == 1) {
            msg = "I give up! The word might be: " + game.possibleWords[0];
        }
        graphics.renderQuestion(msg, game, false);
        state = GAME_OVER;
    }
    else if (game.possibleWords.size() <= WORD_GUESS_THRESHOLD) {
        string msg = "Is your word '" + game.guessNextWord() + "'?";
        graphics.renderQuestion(msg, game, true);
        state = GUESS_WORD;
    }
    else {
        string msg = "Is there a letter '" + string(1, game.guessNextLetter()) + "'?";
        graphics.renderQuestion(msg, game, true);
        state = GUESS_LETTER;
    }
}

void processClick(int mouseX, int mouseY, Hangman& game, Graphics& graphics, GameState& state) {
    if (yesButton.isClicked(mouseX, mouseY)) {
        if (state == GUESS_LETTER) {
            inputText = "";
            string msg = "How many letters '" + string(1, game.currentGuess) + "'? ";
            graphics.renderQuestion(msg, game, false);
            state = INPUT_COUNT;
        } else if (state == GUESS_WORD) {
            game.processWordResponse(true);
            updateGameState(game, graphics, state);
        }
    }
    if (noButton.isClicked(mouseX, mouseY)) {
        if (state == GUESS_LETTER) {
            game.processResponse(0);
            updateGameState(game, graphics, state);
        } else if (state == GUESS_WORD) {
            game.processWordResponse(false);
            // Đặc biệt xử lý khi chỉ còn 1 từ mà vẫn ấn NO
            if (game.possibleWords.size() == 1) {
                game.badGuessCount = MAX_BAD_GUESSES; // Buộc thua
            }
            updateGameState(game, graphics, state);
        }
    }
}

int main(int argc, char* argv[]) {
    Graphics graphics;
    graphics.init();

    Hangman game;
    GameState state = INPUT_LENGTH;

    graphics.renderQuestion("Enter your word's length: ", game, false);

    bool quit = false;
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;

                case SDL_TEXTINPUT:
                    if (state == INPUT_LENGTH || state == INPUT_COUNT) {
                        if (isdigit(event.text.text[0])) {
                            inputText += event.text.text;

                            if (state == INPUT_LENGTH) {
                                string msg = "Enter your word's length: " + inputText;
                                graphics.renderQuestion(msg, game, false);
                            }
                            else if (state == INPUT_COUNT) {
                                string msg = "How many letters '" + string(1, game.currentGuess) + "'? " + inputText;
                                graphics.renderQuestion(msg, game, false);
                            }
                        }
                    }
                    break;

                case SDL_KEYDOWN:
                    if (state == INPUT_LENGTH || state == INPUT_COUNT) {
                        if (event.key.keysym.sym == SDLK_RETURN && !inputText.empty()) {
                            try {
                                int num = stoi(inputText);
                                if (state == INPUT_LENGTH) {
                                    if (num >= MIN_WORD_LENGTH && num <= MAX_WORD_LENGTH) {
                                        game.getPossibleWords(num);
                                        updateGameState(game, graphics, state);
                                    }
                                }
                                else if (state == INPUT_COUNT) {
                                    game.processResponse(num);
                                    updateGameState(game, graphics, state);
                                }
                                inputText = "";
                            } catch (...) {
                                inputText = "";
                            }
                        }
                        else if (event.key.keysym.sym == SDLK_BACKSPACE && !inputText.empty()) {
                            inputText.pop_back();

                            if (state == INPUT_LENGTH) {
                                string msg = "Enter your word's length: " + inputText;
                                graphics.renderQuestion(msg, game, false);
                            }
                            else if (state == INPUT_COUNT) {
                                string msg = "How many letters '" + string(1, game.currentGuess) + "'? " + inputText;
                                graphics.renderQuestion(msg, game, false);
                            }
                        }
                    }
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (state == GUESS_LETTER || state == GUESS_WORD) {
                        int x, y;
                        SDL_GetMouseState(&x, &y);
                        processClick(x, y, game, graphics, state);
                    }
                    break;
            }
        }
        SDL_Delay(50);
    }

    graphics.quit();
    return 0;
}
