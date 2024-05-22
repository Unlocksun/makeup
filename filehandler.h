#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <fstream>
#include <string>
#include <iostream>

using namespace std;

// 文件处理类
class FileHandler {
private:
    fstream currfile;
public:
    int rownum, colnum;       // row and column nums of file
    int unsaved;
    string filename;
public:
    FileHandler()
    {
        rownum = 0;
        colnum = 0;
    }
    fstream openFile(string filename);
    string readFile(const string& filename);
    void writeFile(const string& filename, const string& content);
    fstream& getFileStream() {
        return currfile;
    }
};

#endif