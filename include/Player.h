#pragma once

#include "GameObject.h"

class Player : public GameObject
{
    public:
        Player(std::shared_ptr<Window> window, int x = 0, int y = 0) : GameObject(window, x, y), _dx(0), _dy(0) { }

        void update(int ms);
        void draw(std::shared_ptr<Canvas> canvas);
        void vertical(float d);
        void horizontal(float d);

    private:
        float _dx;
        float _dy;
};
