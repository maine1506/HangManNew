#ifndef _LOGIC__H
#define _LOGIC__H

#include <SDL.h>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <fstream>
#include "defs.h"

using namespace std;

struct Hangman {
    int wordLength;
    vector<string> wordList = loadWords("assets/word_list.txt");
    unordered_set<char> guessedLetters;
    vector<string> possibleWords;
    int badGuessCount = 0;
    char currentGuess = '\0';
    string currentWordGuess = "";


    void getPossibleWords(int length) {
        wordLength = length;
        possibleWords.clear();
        for (const auto& word : wordList) {
            if (word.size() == wordLength) {
                possibleWords.push_back(word);
            }
        }
    }

    char guessNextLetter() {
        unordered_map<char, int> frequency;
        for (const auto& word : possibleWords) {
            for (char c : word) {
                if (guessedLetters.find(c) == guessedLetters.end()) {
                    frequency[c]++;
                }
            }
        }

        if (frequency.empty()) return '\0';

        currentGuess = max_element(frequency.begin(), frequency.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; })->first;
        guessedLetters.insert(currentGuess);
        return currentGuess;
    }

    string guessNextWord() {
        if (possibleWords.empty()) return "";
        currentWordGuess = possibleWords[0];
        return currentWordGuess;
    }

    void processResponse(int count) {
        vector<string> newPossibleWords;
        for (const auto& word : possibleWords) {
            if (count == std::count(word.begin(), word.end(), currentGuess)) {
                newPossibleWords.push_back(word);
            }
        }
        possibleWords = move(newPossibleWords);

        if (count == 0) badGuessCount++;
    }

    void processWordResponse(bool correct) {
        if (correct) {
            possibleWords = {currentWordGuess};
        } else {
            possibleWords.erase(
                remove(possibleWords.begin(), possibleWords.end(), currentWordGuess),
                possibleWords.end()
            );
            badGuessCount++;
        }
    }

    bool isGameOver() {
        return (badGuessCount >= MAX_BAD_GUESSES ||
                possibleWords.size() == 1 ||
                possibleWords.empty());
    }

    vector<string> loadWords(const string& filename) {
        vector<string> words;
        ifstream file(filename);
        if (!file) {
            cerr << "Can't open file " << filename << "!\n";
            exit(1);
        }
        string word;
        while (file >> word) {
            words.push_back(word);
        }
        file.close();
        return words;
    }
};

#endif // _LOGIC__H
