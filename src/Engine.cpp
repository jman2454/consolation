#include "Engine.h"

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

void Engine::processEvent(SDL_Event& event)
{
    switch (event.type)
    {
        case SDL_KEYDOWN:
            _input->keyDown(event.key);
            break;
        case SDL_KEYUP:
            _input->keyUp(event.key);
            break;
        case SDL_QUIT:
            break;
    }
}

void Engine::eventLoop()
{
    SDL_Event event;
    while (!_canceled.load())
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
