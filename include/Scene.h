#include "Canvas.h"
#include "InputHandler.h"

class Scene 
{
    public:
        virtual void update(std::shared_ptr<InputHandler> input, int delta_ms) = 0;
        virtual void draw(std::shared_ptr<Canvas> canvas) = 0;
};