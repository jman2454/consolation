#include <sys/ioctl.h>
#include <thread>
#include "InputHandler.h"
#include "Player.h"
#include "PlayerScene.h"

#define NS_PER_MS 1000000

class Engine
{
    public:
        Engine(float frame_ms = 16.57) : 
            _frame_length_ns(frame_ms * NS_PER_MS),
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
            eventLoop();
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

        void eventLoop();

    private:
        int _frame_length_ns;
        std::atomic_bool _started;
        std::atomic_bool _canceled;
        std::thread _gameThread;
        std::shared_ptr<Canvas> _canvas;
        std::shared_ptr<InputHandler> _input;

        void processEvent(SDL_Event& event);

        void gameLoop()
        {
            auto prev_start = std::chrono::steady_clock::now();
            std::chrono::nanoseconds frame_length(_frame_length_ns);
            std::chrono::nanoseconds elapsed{0};
            std::chrono::seconds duration{5};
            PlayerScene scene(_canvas);
            
            while (elapsed < duration)
            {
                auto now = std::chrono::steady_clock::now();
                std::chrono::nanoseconds delta = now - prev_start;
                prev_start = now;
                elapsed += delta;
                
                scene.update(_input, std::chrono::duration_cast<std::chrono::milliseconds>(delta).count());
                scene.draw(_canvas);
                
                auto time_taken = std::chrono::steady_clock::now() - now;
                if (time_taken < frame_length)
                {
                    std::this_thread::sleep_for(frame_length - time_taken);
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