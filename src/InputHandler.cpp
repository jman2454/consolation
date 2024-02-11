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

bool checkError()
{
    auto err = SDL_GetError();
    if (!err || *err == '\0')
    {
        // no events, continue
        return false;
    }

    throw std::runtime_error("ERROR: " + std::string(err));
}

void InputHandler::readEventLoop(std::atomic_bool& canceled)
{
    SDL_Event event;
    while (!canceled.load())
    {
        auto success = SDL_WaitEventTimeout(&event, 3000);
        if (success != 1 && !checkError())
        {
            continue;
        }

        processEvent(event);
        while (SDL_PollEvent(&event))
        {
            processEvent(event);
        }

        checkError();
    }
}

void InputHandler::processEvent(SDL_Event& event)
{
    switch (event.type)
    {
        case SDL_KEYDOWN:
            keyDown(event.key);
            break;
        case SDL_KEYUP:
            keyUp(event.key);
            break;
        case SDL_QUIT:
            break;
    }
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

void InputHandler::clearInput()
{
    _x.store(0);
    _y.store(0);
}