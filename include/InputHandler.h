#include <unistd.h>
#include <errno.h>
#include <iostream>

class InputHandler
{
public:
    int getX();
    int getY();
    void readInput();
    void clearInput();

    InputHandler()
    {
        clearInput();
    }

    ~InputHandler()
    {
        delete[] _buffer;
    }

private:
    char *_buffer = new char[256];
    std::atomic_int32_t _y;
    std::atomic_int32_t _x;
    int _version = 0;

    void processInput(int length);
};
