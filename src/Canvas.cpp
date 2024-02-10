
#include <termios.h>
#include <unistd.h>
#include "Canvas.h"

void Canvas::clear()
{
    for (auto &pair : _lastDirty)
    {
        bufferWrite(_buf.get() + pair.first, ' ', pair.second);
    }
    _lastDirty.clear();
}

void Canvas::fillRect(char value, int left, int top, int width, int height)
{
    int drawn_width = std::min(left + width, getWidth()) - left;
    int inverted_y = getHeight() - top - 1;
    int drawn_height = std::min(getHeight(), inverted_y + height) - inverted_y;

    for (int i = 0; i < drawn_height; i++)
    {
        bufferWrite(_buf.get() + left + getWidth() * (inverted_y + i), value, drawn_width);
    }
}

void Canvas::draw()
{
    std::sort(_dirty.begin(), _dirty.end(),
                [](std::pair<int, int> left, std::pair<int, int> right)
                {
                    return left.first < right.first;
                });

    int lastStart = -1;
    int currLength = -1;
    int merged_index = 0;
    for (auto &pair : _dirty)
    {
        // Nothing to merge with
        if (lastStart == -1)
        {
            lastStart = pair.first;
            currLength = pair.second;
        }
        // Starts within/adjacent to the currently tracked range, extend to whatever the length should be
        else if (pair.first <= lastStart + currLength)
        {
            currLength = std::max(lastStart + currLength, pair.first + pair.second) - lastStart;
        }
        // Doesn't start within our current range or at its start, so must be after
        // Flush the current range
        else
        {
            _dirty[merged_index++] = std::make_pair(lastStart, currLength);
            lastStart = pair.first;
            currLength = pair.second;
        }
    }

    if (lastStart != -1)
    {
        _dirty[merged_index++] = std::make_pair(lastStart, currLength);
    }

    _dirty.resize(merged_index);

    for (auto &pair : _dirty)
    {
        moveCursorTo(pair.first);
        fwrite(_buf.get() + pair.first, sizeof(char), pair.second, stdout);
        _lastDirty.push_back(pair);
    }

    _dirty.clear();
    fflush(stdout);
}

void Canvas::moveCursorTo(int pos)
{
    char command[32];
    int x = (pos / getWidth()) + 1;
    int y = (pos % getWidth()) + 1;
    snprintf(command, sizeof(command), "\x1b[%d;%dH", x, y);
    fwrite(command, strlen(command), 1, stdout);
}

void Canvas::moveCursorBy(int dx, int dy)
{
    char command[32];
    snprintf(command, sizeof(command), "\x1b[%d;%dH", _cursorX + dx, _cursorY + dy);
    fwrite(command, strlen(command), 1, stdout);
}

void Canvas::clearToEOL()
{
    const char *command = "\x1b[K";
    fwrite(command, strlen(command), 1, stdout);
}

void Canvas::hideCursor()
{
    const char *command = "\x1b[?25l";
    fwrite(command, strlen(command), 1, stdout);
}

void Canvas::showCursor()
{
    const char *command = "\x1b[?25h";
    fwrite(command, strlen(command), 1, stdout);
}

void Canvas::bufferWrite(char *dest, char val, int len)
{
    _dirty.push_back(std::make_pair((int)(dest - _buf.get()), len));
    memset(dest, val, len);
}

void Canvas::disableRawMode()
{
    // Restore the original terminal attributes
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void Canvas::enableRawMode()
{
    // Read the current terminal attributes
    tcgetattr(STDIN_FILENO, &orig_termios);
    // atexit(disableRawMode); // Ensure we disable raw mode on exit

    // Make a copy of the original termios attributes to modify
    struct termios raw = orig_termios;

    // Input flags - Turn off input processing
    // IXON: Turn off Ctrl-S and Ctrl-Q
    // ICRNL: Stop Ctrl-M from being interpreted as newline on input
    // BRKINT: Disable break condition that causes a SIGINT to be sent
    // INPCK: Disable parity checking
    // ISTRIP: Disable 8th bit stripping
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);

    // Output flags - Turn off output processing
    // OPOST: Turn off all output processing (e.g., converting newline to carriage return + newline)
    raw.c_oflag &= ~(OPOST);

    // Control flags - Set character size to 8 bits per byte
    raw.c_cflag |= (CS8);

    // Local flags - Turn off echoing and canonical mode
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    // Control characters - Set minimum number of bytes and a timeout for read()
    raw.c_cc[VMIN] = 0;  // Minimum data to read before read() returns
    raw.c_cc[VTIME] = 1; // Time to wait before read() returns (tenths of a second)

    // Apply the modified termios attributes
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}