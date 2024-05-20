#ifndef UI_H
#define UI_H

#include <iostream>
#include <fstream>
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
    void writeBuffer(const std::string& input);
    int getSize();
    char* getText();
    void clearBuffer();
};

// 控制终端
class Terminal {
friend class Editor;
private:
    int cx, cy;                 // x为列，y为行
    int rowoff, coloff;         // 显示内容在文件中的行列偏移

    int numrows;                // 有文件内容的行数，超出范围使用 ~ 替代
    vector<string> rowdata;     // 存储每行的数据

    struct termios orig_termios; // 原始终端参数，用于退出后恢复终端
    bool rawmode;

    int win_cols, win_rows;     // 终端行列
    Buffer buffer;              // 终端缓冲
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
    int Init();
    int enableRawMode();
    int getWindowSize();
    void disableRawMode(int fd);
    void clearScreen();
    void die(const char *s);
    void refreshScreen();
    void drawRows();
};

#endif