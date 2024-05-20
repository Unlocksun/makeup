#include "makeup.h"

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
            if (seq[1] >= '0' && seq[1] <= '9')
            {
                if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
                if (seq[2] == '~')
                {
                    switch (seq[1])
                    {
                        case '1': return HOME_KEY;
                        case '3': return DEL_KEY;
                        case '4': return END_KEY;
                        case '5': return PAGE_UP;
                        case '6': return PAGE_DOWN;
                        case '7': return HOME_KEY;
                        case '8': return END_KEY;
                    }
                }
            }
            else
            {
                switch (seq[1])
                {
                    case 'A': return ARROW_UP;
                    case 'B': return ARROW_DOWN;
                    case 'C': return ARROW_RIGHT;
                    case 'D': return ARROW_LEFT;
                    case 'F': return END_KEY;
                    case 'H': return HOME_KEY;
                }
            }
        }
        else if (seq[0] == 'O') 
        {
            switch (seq[1]) 
            {
                case 'H': return HOME_KEY;
                case 'F': return END_KEY;
            }
        }
        return '\x1b';
    }
    return c;
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
    case DEL_KEY:
    case HOME_KEY:
        ui.cx = 0;
        break;
    case END_KEY:
        ui.cx = ui.win_cols - 1;
        break;
    case PAGE_UP:
    case PAGE_DOWN:
    {
        int times = ui.win_rows;
        while (times--)
            moveCursor(keyvalue == PAGE_UP ? ARROW_UP : ARROW_DOWN);
        break;
    }
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

int main(int argc, char const *argv[])
{
    if (argc != 2) {
        fprintf(stderr,"Usage: kilo <filename>\n");
        exit(1);
    }

    static Editor editor;

    editor.openFile(argv[1]);

    if(editor.ui.Init())
    {
        fprintf(stderr, "Now done");
        exit(1);
    }

    editor.run();
    return 0;
}

int Editor::openFile(string filename)
{
    fstream file_stream(filename);

    if (!file_stream.is_open()) {
        cerr << "无法打开文件: " << filename << endl;
        exit(1);
    }

    string line;
    if (getline(file_stream, line)) {
        ui.numrows = 1;
        ui.rowdata.push_back(line);
    }

    file_stream.close();
    return 0;
}
