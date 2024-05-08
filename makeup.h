#ifndef KILO_H
#define KILO_H
#include "ui.h"

/*** defines ***/
#define CTRL_KEY(k) ((k) & 0x1f)


// 文件处理类
class FileHandler {
private:
    int fd;
    string openFile;
    int unsaved;
    int rownums, colnums;       // row and column nums of file
public:
    FileHandler(){
        fd = STDIN_FILENO;
    }
    string readFile(const string& filename) {
        return "";
    }

    void writeFile(const string& filename, const string& content) {
    }
};

// 编辑器类
class Editor {
public:
    FileHandler fileHandler;
    Terminal ui;
public:
    void run();
    char readKey();
    void keyHandler();
    Editor(){
    }
};
#endif