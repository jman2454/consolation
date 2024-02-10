

#include "Window.h"
#include <vector>
#include <termios.h>

class Canvas : public Window
{
public:
    void clear();
    void fillRect(char value, int left, int top, int width, int height);
    void draw();

    Canvas(int width, int height) : Window(width, height), 
                                    _buf(std::make_unique<char[]>(width * height + 1)),
                                    _buflen(width * height + 1),
                                    _canvaslen(width * height),
                                    _cursorX(0),
                                    _cursorY(0),
                                    _dirty(std::vector<std::pair<int, int>>()),
                                    _lastDirty(std::vector<std::pair<int, int>>())
    {
        enableRawMode();
        hideCursor();
        _buf[_buflen - 1] = '\0';

        _lastDirty.push_back(std::make_pair(0, _canvaslen));
        clear();
        draw();
    }

    ~Canvas()
    {
        disableRawMode();
        showCursor();
        fflush(stdout);
    }

private:
    int _buflen;
    int _canvaslen;
    int _cursorX;
    int _cursorY;
    std::vector<std::pair<int, int>> _dirty;
    std::vector<std::pair<int, int>> _lastDirty;
    std::unique_ptr<char[]> _buf;

    void moveCursorTo(int pos);
    void moveCursorBy(int dx, int dy);
    void clearToEOL();
    void hideCursor();
    void showCursor();
    void bufferWrite(char *dest, char val, int len);
    struct termios orig_termios;
    void disableRawMode();
    void enableRawMode();
};
