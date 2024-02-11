#include <sys/ioctl.h>
#include <thread>
#include "InputHandler.h"
#include "Player.h"
#include "PlayerScene.h"

class Engine
{
    public:
        Engine(float frame_ms = 16.57) : 
            _frame_length_ns(frame_ms * 1000000),
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
        int _frame_length_ns;
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

            std::chrono::nanoseconds frame_length(_frame_length_ns);
            std::chrono::nanoseconds elapsed{0};
            std::chrono::seconds duration{5};
            PlayerScene scene(_canvas);

            while (elapsed < duration)
            {
                auto now = std::chrono::steady_clock::now();
                auto delta = now - last_loop;
                elapsed += delta;
                last_loop = now;

                auto delta_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_frame);
                if (delta_ms >= frame_length)
                {
                    scene.update(_input, delta_ms.count());
                    scene.draw(_canvas);
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