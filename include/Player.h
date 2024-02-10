#include "GameObject.h"

class Player : public GameObject
{
    public:
        Player(Window& window, int x = 0, int y = 0) : GameObject(window, x, y), _dx(0), _dy(0) { }

        void update(std::chrono::milliseconds delta);
        void draw(Canvas& canvas);
        void vertical(float d);
        void horizontal(float d);

    private:
        float _dx;
        float _dy;
};
