#include <sys/ioctl.h>
#include <thread>
#include "InputHandler.h"
#include "Player.h"

class Engine
{
    public:
        Engine(int frame_ms = 16) : 
            _frame_length_ms(16),
            _started(false),
            _canceled(false)
        { }

        ~Engine()
        {
            stop();
            SDL_Quit();
        }

        void run()
        {
            if (!initialize())
            {
                throw std::runtime_error("Failed to initialize!");
            }

            bool go_condition = false;
            if (!_started.compare_exchange_strong(go_condition, true))
            {
                return;
            }

            _gameThread = std::move(std::thread([this](){ this->gameLoop(); }));
            _input->readEventLoop(_canceled);
        }

        void stop()
        {
            if (!_started.load())
            {
                return;
            }

            _started.store(false);
            _canceled.store(true);

            if (!_gameThread.joinable())
            {
                return;
            }
            _gameThread.join();
        }

    private:
        int _frame_length_ms;
        std::atomic_bool _started;
        std::atomic_bool _canceled;
        std::thread _gameThread;
        std::shared_ptr<Canvas> _canvas;
        std::shared_ptr<InputHandler> _input;

        void gameLoop()
        {    
            auto start = std::chrono::steady_clock::now();
            auto last_frame = start;
            auto last_loop = start;

            std::chrono::time_point<std::chrono::steady_clock> last_anim;
            std::chrono::milliseconds frame_length(16);
            std::chrono::nanoseconds elapsed{0};
            std::chrono::seconds duration{5};
            float frame_ms = (float) frame_length.count();
            long duration_millis{std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()};
            int last_second = -1;

            int chunk_size = 10;
            int pos = 0;
            int total_dist = _canvas->getWidth() - chunk_size + 1;
            int last_pos = -1;
            std::chrono::time_point<std::chrono::steady_clock> last_player_move;

            auto player = std::make_shared<Player>(_canvas);
            std::vector<std::shared_ptr<GameObject>> objs{ player };

            while (elapsed < duration)
            {
                auto now = std::chrono::steady_clock::now();
                auto delta = now - last_loop;
                elapsed += delta;
                last_loop = now;

                auto delta_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_frame);
                if (delta_ms >= frame_length)
                {
                    for (auto& obj : objs)
                    {
                        obj->update(delta_ms);
                    }
                    pos = (total_dist * std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count()) / duration_millis;

                    int player_dx = _input->getX();
                    int player_dy = _input->getY();
                    if (player_dx != 0 && (player->getX() != _canvas->getWidth() - 1 || player_dx < 0) && (player->getX() > 0 || player_dx > 0))
                    {
                        player->horizontal(1.3 * player_dx / frame_ms);
                        last_player_move = now;
                    }

                    if (player->getY() == 0 && player_dy > 0)
                    {
                        // n cells in a frame
                        player->vertical(1.3 / frame_ms);
                        last_player_move = now;
                    }

                    _canvas->clear();
                    _canvas->fillRect('|', pos, _canvas->getHeight() - 1, chunk_size, _canvas->getHeight());
                    for (auto& ptr : objs)
                    {
                        ptr->draw(_canvas);
                    }
                    _canvas->draw();
                    last_frame = now;
                }
            }

            _canceled.store(true);
        }

        bool initialize()
        {
            struct winsize size;
            if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == -1)
            {
                std::cerr << "Failed to get terminal size" << std::endl;
                return false;
            }

            int width{size.ws_col};
            int height{size.ws_row};
            
            if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
                std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
                return false;
            }

            _canvas = std::make_shared<Canvas>(width, height);
            _input = std::make_shared<InputHandler>();
            return true;
        }
};