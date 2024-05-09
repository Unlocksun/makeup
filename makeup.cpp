#include "makeup.h"

enum editorKey {
  ARROW_LEFT = 200,
  ARROW_RIGHT,
  ARROW_UP,
  ARROW_DOWN
};

/* 从 fd 读取一个字符。它会处理任何可能的错误（除了 EAGAIN），并在非错误情况下返回读取的字符。 */
int Editor::readKey()
{
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1)
    {
        if (nread == -1 && errno != EAGAIN)
            ui.die("readError");
    }
    // 读到转义字符
    if (c == '\x1b')
    {
        // 读取下面两个字符
        char seq[3];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';
        if (seq[0] == '[')
        {
            switch (seq[1])
            {
            case 'A':
                return ARROW_UP;
            case 'B':
                return ARROW_DOWN;
            case 'C':
                return ARROW_RIGHT;
            case 'D':
                return ARROW_LEFT;
            }
        }
        return '\x1b';
    }
    else return c;
}

void Editor::moveCursor(int direction)
{
    switch (direction)
    {
    case ARROW_LEFT:
        if(ui.cx > 0) ui.cx--;
        break;
    case ARROW_RIGHT:
        if(ui.cx < ui.win_cols - 1) ui.cx++;
        break;
    case ARROW_UP:
        if(ui.cy > 0) ui.cy--;
        break;
    case ARROW_DOWN:
        if(ui.cy < ui.win_rows - 1) ui.cy++;
        break;
    }
}

void Editor::keyHandler()
{
    int keyvalue = readKey();
    switch (keyvalue)
    {
        case CTRL_KEY('c'):
            exit(0);
            break;
        case ARROW_DOWN:
        case ARROW_UP:
        case ARROW_LEFT:
        case ARROW_RIGHT:
            moveCursor(keyvalue);
            break;
        default:
            break;
    }
}

void Editor::run()
{
    while(1)
    {
        ui.refreshScreen();
        keyHandler();
    }
}

static Editor editor;
int main(int argc, char const *argv[])
{
    if (argc != 2) {
        fprintf(stderr,"Usage: kilo <filename>\n");
        exit(1);
    }

    if(editor.ui.enableRawMode())
    {
        fprintf(stderr, "Now done");
        exit(1);
    }

    editor.run();
    return 0;
}
