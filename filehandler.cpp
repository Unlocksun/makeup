#include "filehandler.h"

fstream FileHandler::openFile(string filename)
{
    currfile.open(filename);

    if (!currfile.is_open())
    {
        cerr << "Failed to open file: " << filename << endl;
        exit(1);
    }
    else
    {
        this->filename = filename;
    }
    return fstream();
}