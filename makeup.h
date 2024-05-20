#ifndef KILO_H
#define KILO_H
#include "ui.h"

/*** defines ***/
#define CTRL_KEY(k) ((k) & 0x1f)

/*** keys ***/
enum editorKey {
  ARROW_LEFT = 200,
  ARROW_RIGHT,
  ARROW_UP,
  ARROW_DOWN,
  HOME_KEY,
  END_KEY,
  DEL_KEY,
  PAGE_UP,
  PAGE_DOWN
};

// 文件处理类
class FileHandler {
private:
    int fd;
    string filename;
    int unsaved;
    int rownums, colnums;       // row and column nums of file
public:
    FileHandler()
    {
        rownums = 0;
        colnums = 0;
    }
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
    int openFile(string filename);
    void keyHandler();
    void moveCursor(int direction);
    Editor(){}
    ~Editor(){}
};
#endif