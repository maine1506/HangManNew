#ifndef _BUTTON__H
#define _BUTTON__H

#include <SDL.h>

using namespace std;

struct Button {
    SDL_Rect rect;
    string label;

    bool isClicked(int mouseX, int mouseY) const {
        return (mouseX >= rect.x && mouseX <= rect.x + rect.w
                && mouseY >=rect.y && mouseY <= rect.y + rect.h);
    }
};

Button yesButton = {{100, 200, 100, 50}, "YES"};
Button noButton  = {{250, 200, 100, 50}, "NO"};

#endif // _BUTTON__H
