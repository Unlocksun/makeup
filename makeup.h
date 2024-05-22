#ifndef MAKEUP_H
#define MAKEUP_H
#include "ui.h"
#include "filehandler.h"

/*** defines ***/
#define CTRL_KEY(k) ((k) & 0x1f)
#define TABLE_SIZE 8

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


// 编辑器类
class Editor {
public:
    FileHandler fh;
    Terminal ui;
public:
    void run();
    int readKey();
    int readFile(string filename);
    void keyHandler();
    void moveCursor(int direction);
    Editor(){}
    ~Editor(){}
};
#endif