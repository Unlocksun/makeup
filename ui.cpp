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
    for (int y = 0; y < win_rows-1; ++y)
    {
        if(y >= content_row_num)
        {
            // 空文档打印欢迎信息
            if (content_row_num == 0 && win_rows / 3 == y)
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
        else if (y < content_row_num-rowoff)
        {
            int rowlength = rowdata[y + rowoff].length();
            if(coloff < rowlength) 
                buffer.writeBuffer(rowdata[y + rowoff].c_str() + coloff, win_cols);

            buffer.writeBuffer("\x1b[K");   // x1b[K: 清除光标至行尾的内容
            buffer.writeBuffer("\r\n");
        }
        else
        {
            buffer.writeBuffer("~\r\n");
            buffer.writeBuffer("\x2b[K");
        }
    }

    buffer.writeBuffer("This is info line ^_^");
    buffer.writeBuffer("\x1b[K");
}

// 滚动屏幕，directing：-1上滚，1下滚，-2左滚
void Terminal::winScroll(int direction)
{
    getWindowSize();
    switch (direction)
    {
    case -1:
    case 1:
        rowoff += direction;
        // 如果行偏移量超出了上边界，则设为0
        if (rowoff < 0) rowoff = 0;
        // 如果内容行数超过窗口行数，且行偏移量超出了下边界（视图不足以显示所有内容），则设为最大可能值
        else if (content_row_num > win_rows && rowoff > content_row_num - win_rows) rowoff = content_row_num - win_rows;
        // 如果内容行数不足以填满窗口，将行偏移量设为0
        else if (content_row_num <= win_rows) rowoff = 0;
        break;
    case -2:
    case 2:
        coloff += direction/2;
        if (coloff < 0) coloff = 0;
        if (coloff >= win_cols+1) coloff =  win_cols + 1;
        break;
    default:
        break;
    }
}

void Terminal::refreshScreen()
{
    buffer.writeBuffer("\x1b[?25l"); // hide cursor
    buffer.writeBuffer("\x1b[H");

    drawRows();

    // 更新光标位置
    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", cy+1, cx+1);
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

void Buffer::writeBuffer(const string& input)
{
    content.insert(content.end(), input.begin(), input.end());
}

void Buffer::writeBuffer(const string& input, int len)
{
    int inputlen = input.length();
    if (inputlen > len)
        content.insert(content.end(), input.begin(), input.begin() + len - 1);
    else 
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