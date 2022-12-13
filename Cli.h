#ifndef _CLI_H
#define _CLI_H

#include <string>

using namespace std;

class Cli{
    private:
        void splitArgs(string input, string &command, string* &args, int &numArgs);
        bool seperatePipe(string input, string output[2]);
    public:
        bool call(string command);
};

#endif