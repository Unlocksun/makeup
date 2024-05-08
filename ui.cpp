#include "ui.h"
#include "makeup.h"

Terminal::~Terminal()
{
    disableRawMode(STDIN_FILENO);
   clearScreen();
}

void Terminal::die(const char *s) {
    clearScreen();

    perror(s);
    exit(1);
}    

void Terminal::clearScreen()
{
    // \x1b: esc; [2J: 清屏
    buffer.writeBuffer("\x1b[2J", 4);
    // <esc>[12;40H 将光标设置为（12，40）(默认)
    buffer.writeBuffer("\x1b[H", 3);

    write(STDOUT_FILENO, buffer.getText(), buffer.getSize());
    buffer.clearBuffer();
}

void Terminal::drawRows()
{
    for (int y = 0; y < win_rows-1; ++y)
    {
        if (win_rows / 3 == y)
        {
            char welcome[20];
            snprintf(welcome, win_cols, "Welcome!---Version: %s", VERTION);
            buffer.writeBuffer(welcome, 20);
            continue;
        }
        
        buffer.writeBuffer("~\r\n", 3);
        // x1b[K: 清除光标至行尾的内容
        buffer.writeBuffer("\x1b[K", 3);
    }
    buffer.writeBuffer("~", 1);
    buffer.writeBuffer("\x1b[K", 3);
}

void Terminal::refreshScreen()
{
    buffer.writeBuffer("\x1b[?25l", 6); // hide cursor
    drawRows();
    buffer.writeBuffer("\x1b[H", 3);
    buffer.writeBuffer("\x1b[?25h", 6); // show cursor

    write(STDOUT_FILENO, buffer.getText(), buffer.getSize());
    buffer.clearBuffer();
}

void Terminal::disableRawMode(int fd)
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

int Terminal::enableRawMode()
{
    if(!isatty(STDIN_FILENO) || tcgetattr(STDIN_FILENO, &orig_termios) == -1) 
        die("tcgetattr");

    struct termios raw = orig_termios;
    /* input modes: no break, no CR to NL, no parity check, no strip char,
     * no start/stop output control. */
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    /* output modes - disable post processing */
    raw.c_oflag &= ~(OPOST);
    /* control modes - set 8 bit chars */
    raw.c_cflag |= (CS8);
    /* local modes - choing off, canonical off, no extended functions,
     * no signal chars (^Z,^C) */
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    /* control chars - set return condition: min number of bytes and timer. */
    raw.c_cc[VMIN] = 0; /* Return each byte, or zero for timeout. */
    raw.c_cc[VTIME] = 1; /* 100 ms timeout (unit is tens of second). */  

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) < 0)
        die("tcgetattr");

    this->rawmode = 1; 
    return 0;
}

// get the size of current terminal
int Terminal::getWindowSize()
{
    struct winsize ws;

    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
    {
        return -1;
    }
    else
    {
        win_cols = ws.ws_col;
        win_rows = ws.ws_row;
        return 0;
    }
}

void Buffer::writeBuffer(const char* input, int len)
{
    content.insert(content.end(), input, input + (len));
    size = content.size();
}

int Buffer::getSize()
{
    return size;
}

char* Buffer::getText()
{
    return content.data();
}

void Buffer::clearBuffer()
{
    content.clear();
    size = 0;
}