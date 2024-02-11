#include <thread>
#include "InputHandler.h"

int InputHandler::getX()
{
    return _x.load();
}

int InputHandler::getY()
{
    return _y.load();
}

void InputHandler::keyUp(SDL_KeyboardEvent& event)
{
    auto keyCode = event.keysym.sym;
    switch (keyCode)
    {
        case SDLK_LEFT:
            _x.store(0);
            break;
        case SDLK_RIGHT:
            _x.store(0);
            break;
        case SDLK_UP:
            _y.store(0);
            break;
        case SDLK_DOWN:
            _y.store(0);
            break;
    }
}

void InputHandler::keyDown(SDL_KeyboardEvent& event)
{
    auto keyCode = event.keysym.sym;
    switch (keyCode)
    {
        case SDLK_LEFT:
            _x.store(-1);
            break;
        case SDLK_RIGHT:
            _x.store(1);
            break;
        case SDLK_UP:
            _y.store(1);
            break;
        case SDLK_DOWN:
            _y.store(-1);
            break;
    }
}