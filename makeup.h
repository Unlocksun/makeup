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
    FileHandler(){}
    string readFile(const string& filename);
    void writeFile(const string& filename, const string& content);
};

// 编辑器类
class Editor {
public:
    FileHandler fileHandler;
    Terminal ui;
public:
    void run();
    int readKey();
    void keyHandler();
    void moveCursor(int direction);
    Editor(){}
    ~Editor(){}
};
#endif