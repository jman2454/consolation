#include <unistd.h>
#include <errno.h>
#include <iostream>
#include "InputHandler.h"

int InputHandler::getX()
{
    return _x.load();
}

int InputHandler::getY()
{
    return _y.load();
}

void InputHandler::readInput()
{
    int bytesRead = read(STDIN_FILENO, _buffer, sizeof(_buffer) - 1);
    if (bytesRead < 0 && errno != EAGAIN)
    {
        perror("read");
        return;
    }

    if (bytesRead <= 0)
    {
        return;
    }

    processInput(bytesRead);
}

void InputHandler::clearInput()
{
    _x.store(0);
    _y.store(0);
}

void InputHandler::processInput(int length)
{
    bool in_sequence = false;
    bool expect_arrow = false;
    for (int i = 0; i < length; i++)
    {
        char c = _buffer[i];
        if (c == '\x1b')
        {
            in_sequence = true;
            expect_arrow = false;
        }
        else if (in_sequence && c == '[')
        {
            expect_arrow = true;
        }
        else
        {
            if (expect_arrow)
            {
                switch (c)
                {
                case 'A':
                    _y.store(1);
                    break;
                case 'B':
                    _y.store(-1);
                    break;
                case 'C':
                    _x.store(1);
                    break;
                case 'D':
                    _x.store(-1);
                    break;
                default:
                    in_sequence = false;
                    expect_arrow = false;
                    continue;
                }
            }

            in_sequence = false;
            expect_arrow = false;
        }
    }
}