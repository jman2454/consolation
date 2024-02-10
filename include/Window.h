#pragma once

class Window 
{
public:
    Window(int width, int height) : _width(width), _height(height) { }

    inline int getWidth()
    {
        return _width;
    }

    inline int getHeight()
    {
        return _height;
    }

private:
    int _width;
    int _height;
};
