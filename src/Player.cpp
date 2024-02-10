#include <chrono>

#include "Player.h"

void Player::update(std::chrono::milliseconds delta)
{
    auto ms = delta.count();
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
    canvas->fillRect('O', getX(), getY(), 1, 1);
}

void Player::vertical(float d) 
{
    _dy = d;
}

void Player::horizontal(float d)
{
    _dx = d;
}
