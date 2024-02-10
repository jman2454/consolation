#include <sys/ioctl.h>
#include <thread>
#include "InputHandler.h"
#include "Player.h"

void pollInput(InputHandler &handler)
{
    auto sleep_period = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::milliseconds(30));
    while (true)
    {
        handler.readInput();
        std::this_thread::sleep_for(sleep_period);
    }
}

int main()
{
    struct winsize size;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == -1)
    {
        std::cerr << "Failed to get terminal size" << std::endl;
        return 1;
    }

    int width{size.ws_col};
    int height{size.ws_row};

    // alloc
    Canvas canvas{width, height};

    // render
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

    InputHandler input;

    int chunk_size = 10;
    int pos = 0;
    int total_dist = width - chunk_size + 1;
    int last_pos = -1;
    std::chrono::time_point<std::chrono::steady_clock> last_player_move;

    auto input_thread = std::thread(pollInput, std::ref(input));
    auto player = std::make_shared<Player>(static_cast<Window&>(canvas));
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

            int player_dx = input.getX();
            int player_dy = input.getY();

            if (now - last_player_move > std::chrono::milliseconds(5))
            {
                if (player_dx != 0 && (player->getX() != canvas.getWidth() - 1 || player_dx < 0) && (player->getX() > 0 || player_dx > 0))
                {
                    player->horizontal(1.5 * player_dx / frame_ms);
                    last_player_move = now;
                }

                if (player_dy != 0 && (player->getY() == 0 || player_dy > 0))
                {
                    // n cells in a frame
                    player->vertical(1.5 / frame_ms);
                    last_player_move = now;
                }
            }

            canvas.clear();

            canvas.fillRect('|', pos, canvas.getHeight() - 1, chunk_size, canvas.getHeight());
            for (auto& ptr : objs)
            {
                ptr->draw(canvas);
            }
            input.clearInput();
            canvas.draw();
            last_frame = now;
        }
    }

    input_thread.detach();

    // cleanup
    return 0;
}