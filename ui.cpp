#include "ui.h"
#include "makeup.h"

Terminal::~Terminal()
{
    clearScreen();
    disableRawMode(STDIN_FILENO);
}

int Terminal::Init()
{
    clearScreen();
    return enableRawMode();
}

void Terminal::die(const char *s) {
    perror(s);
    exit(1);
}    

void Terminal::clearScreen()
{
    // <esc>[2J: 清屏
    buffer.writeBuffer("\x1b[2J");
    // <esc>[12;40H 将光标设置为（12，40）(默认)
    buffer.writeBuffer("\x1b[H");

    write(STDOUT_FILENO, buffer.getText(), buffer.getSize());
    buffer.clearBuffer();
}
/* 逐行在终端打印内容 */
void Terminal::drawRows()
{
    this->getWindowSize();
    for (int y = 0; y < win_rows-1; ++y)
    {
        if(y > numrows)
        {
            if (numrows == 0 && win_rows / 3 == y)
            {
                char welcome[80];
                int len = snprintf(welcome, sizeof(welcome), "Welcome!---Version: %s, row:%d, col:%d", VERTION, win_rows, win_cols);
                if(len > win_cols) len = win_cols;
                int padding = (win_cols - len)/2;
                if(padding)
                {
                    buffer.writeBuffer("@");
                    --padding;
                }
                while (padding--) buffer.writeBuffer(" ");
                
                buffer.writeBuffer(welcome);
                buffer.writeBuffer("\r\n");
                continue;
            }
            
            buffer.writeBuffer("~\r\n");
            buffer.writeBuffer("\x1b[K");   // x1b[K: 清除光标至行尾的内容
        }
        else
        {
            buffer.writeBuffer(rowdata[y].c_str());
            buffer.writeBuffer("\r\n");
        }
    }
    buffer.writeBuffer("\x1b[K");
}

void Terminal::refreshScreen()
{
    buffer.writeBuffer("\x1b[?25l"); // hide cursor
    buffer.writeBuffer("\x1b[H");

    drawRows();

    char buf[32];
    int len = snprintf(buf, sizeof(buf), "\x1b[%d;%dH", cy+1, cx+1);
    buffer.writeBuffer(buf);

    buffer.writeBuffer("\x1b[?25h"); // show cursor

    write(STDOUT_FILENO, buffer.getText(), buffer.getSize());
    buffer.clearBuffer();
}

void Terminal::disableRawMode(int fd)
{
    if(rawmode)
    {
        tcsetattr(fd, TCSAFLUSH, &orig_termios) == -1;
        rawmode = false;
    }
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

void Buffer::writeBuffer(const std::string& input)
{
    content.insert(content.end(), input.begin(), input.end());
}

int Buffer::getSize()
{
    return content.size();
}

char* Buffer::getText()
{
    return content.data();
}

void Buffer::clearBuffer()
{
    content.clear();
}