#ifndef UI_H
#define UI_H

#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <sys/ioctl.h>
#include <vector>

/*** Defines ***/
#define VERTION "0.0.1"

using namespace std;
// 缓冲区类
class Buffer {
private:
    vector<char> content;
public:
    void writeBuffer(const char* input, int len);
    int getSize();
    char* getText();
    void clearBuffer();
};

// 控制终端
class Terminal {
friend class Editor;
private:
    int cx, cy;                 // x为列，y为行
    int rowoff, coloff;         // 显示文件的行列偏移
    bool rawmode;

    struct termios orig_termios;
    int win_cols;           // size of window
    int win_rows;
    Buffer buffer;
public:
    Terminal()
    {
        cx = 0;
        cy = 0;
        rowoff = 0;
        coloff = 0;

        if (getWindowSize() == -1)
        {
            die("getWindowSize");
        }
    }
    ~Terminal();
    int enableRawMode();
    int getWindowSize();
    void disableRawMode(int fd);
    void clearScreen();
    void die(const char *s);
    void refreshScreen();
    void drawRows();
};

#endif