#include <chrono>
#include "Canvas.h"
#include "Window.h"

class GameObject 
{
    public:
        virtual void update(std::chrono::milliseconds delta) = 0;
        virtual void draw(Canvas& canvas) = 0;

        GameObject(Window& window, int x, int y) : _window(window), _x(x), _y(y) { }

        void setX(float x)
        {
            _x = std::min((float) _window.getWidth() - 1, std::max((float) 0, x));
        }

        void setY(float y)
        {
            _y = std::max((float) 0, std::min((float) _window.getHeight() - 1, y));
        }

        float getX() { return _x; }

        float getY() { return _y; }
    
    private:
        Window& _window;
        float _x;
        float _y;
};
