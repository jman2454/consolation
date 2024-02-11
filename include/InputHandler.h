#pragma once

#include <unistd.h>
#include <errno.h>
#include <iostream>
#include "SDL2/SDL.h"

class InputHandler
{
public:
    int getX();
    int getY();
    void clearInput();
    void readEventLoop(std::atomic_bool& canceled);

    InputHandler() : 
    _x(0),
    _y(0)
    { }

private:
    std::atomic_int32_t _y;
    std::atomic_int32_t _x;

    void processEvent(SDL_Event& event);
    void keyDown(SDL_KeyboardEvent& event);
    void keyUp(SDL_KeyboardEvent& event);
};
