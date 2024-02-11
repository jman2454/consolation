#include <chrono>

#include "Player.h"

void Player::update(int ms)
{
    setX((float) getX() + ms * _dx);
    setY((float) getY() + ms * _dy);

    _dx = ((_dx > 0) - (_dx < 0)) * std::max(std::abs(_dx) - 1.0/1024.0 * (double) ms, 0.0);

    if (getY() > 0)
    {
        _dy -= 1.0/1024.0 * (double) ms;
    }
    else
    {
        _dy = 0;
    }
}

void Player::draw(std::shared_ptr<Canvas> canvas)
{
    int factor = 10;
    canvas->fillRect('O', std::max(getX() - (float) factor + 1.0, 0.0), std::max(getY() + (float) factor - 1.0, 0.0), factor, factor);
}

void Player::vertical(float d) 
{
    _dy = d;
}

void Player::horizontal(float d)
{
    _dx = d;
}
