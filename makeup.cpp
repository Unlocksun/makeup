#include "makeup.h"

/* 从 fd 读取一个字符。它会处理任何可能的错误（除了 EAGAIN），并在非错误情况下返回读取的字符。 */
char Editor::readKey()
{
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1))!=1)
    {
        if (nread == -1 && errno != EAGAIN) 
            ui.die("read");
    }
    return c;
}

void Editor::keyHandler()
{
    char c = readKey();
    switch (c)
    {
        case CTRL_KEY('q'):
            ui.clearScreen();
            exit(0);
            break;
        case 'q':
            ui.clearScreen();
            break;
        default:
            write(STDOUT_FILENO, &c, 1);
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

Editor editor;

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
