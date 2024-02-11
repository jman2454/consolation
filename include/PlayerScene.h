#include <vector>
#include "Scene.h"
#include "Player.h"

class PlayerScene : public Scene
{
    public:
        void update(std::shared_ptr<InputHandler> input, int delta_ms)
        {
            for (auto& obj : _objects)
            {
                obj->update(delta_ms);
            }

            int player_dx = input->getX();
            int player_dy = input->getY();
            if (player_dx != 0 && (_player->getX() != _window->getWidth() - 1 || player_dx < 0) && (_player->getX() > 0 || player_dx > 0))
            {
                _player->horizontal(1.3 * player_dx / delta_ms);
            }

            if (_player->getY() == 0 && player_dy > 0)
            {
                // n cells in a frame
                _player->vertical(1.3 / delta_ms);
            }
        }

        void draw(std::shared_ptr<Canvas> canvas)
        {
            canvas->clear();
            for (auto& ptr : _objects)
            {
                ptr->draw(canvas);
            }
            canvas->draw();
        }

        PlayerScene(std::shared_ptr<Window> window)
        : 
        _window(window),
        _objects(std::vector<std::shared_ptr<GameObject>>()), 
        _player(std::make_shared<Player>(window))
        {
            _objects.emplace_back(_player);
        }

    private:
        std::shared_ptr<Window> _window;
        std::vector<std::shared_ptr<GameObject>> _objects;
        std::shared_ptr<Player> _player;
};