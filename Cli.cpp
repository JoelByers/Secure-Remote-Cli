#include "Cli.h"
#include <unistd.h>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/io.h>
#include <sys/wait.h>

using namespace std;

bool Cli::seperatePipe(string input, string output[2]){
    int index = 0;
    while(index < input.length()){
        if(input.at(index) == '>'){
            output[0] = input.substr(0, index);

            index++;
            while(input.at(index) == ' '){
                index++;
            }
            output[1] = input.substr(index, input.length() - index);
            return true;
        }
        index++;
    }

    return false;
}

void Cli::splitArgs(string input, string &command, string* &args, int &numArgs){
    int index = 0;
    // find start of command
    while(input.at(index) == ' '){
        index++;
    }

    while(index < input.length() && input.at(index) != ' '){
        command.push_back(input.at(index));
        index++;
    }

    // add command to arg list
    numArgs = 1;
    int countIndex = index;
    while(countIndex < input.length()){
        // count arguments

        while(countIndex < input.length() && input.at(countIndex) == ' '){
            countIndex++;
        }

        while(countIndex < input.length() && input.at(countIndex) != ' '){
            countIndex++;
        }  
        numArgs++;

    }

    //args = (string*)malloc(sizeof(string) * (numArgs + 1));
    args = new string[numArgs + 1];
    args[0] = command;

    int argIndex = 1;
    while(index < input.length()){
        // find next argument
        while(index < input.length() && input.at(index) == ' '){
            index++;
        }

        // add arguments
        while(index < input.length() && input.at(index) != ' '){
            args[argIndex].push_back(input.at(index));
            index++;
        }  

        argIndex++;

        while(index < input.length() && input.at(index) == ' '){
            index++;
        }
    }  
}

void stringArrayToCharArray(string* input, char** &output, int count){
    int length = 0;

    for(int i = 0; i < count; i++){
        length += input[i].length();
    }

    output = (char**)malloc(sizeof(char*) * length);

    for(int i = 0; i < count; i++){
        output[i] = (char*)input[i].c_str();
    }
}

bool Cli::commandIsAllowed(string command){
    for(int i = 0; i < 6; i++){
        if(commandWhitelist[i].compare(command) == 0){
            return true;
        }
    }

    return false;
}

bool Cli::call(string input){
    string pipeSplit[2];
    bool hasPipe = seperatePipe(input, pipeSplit);
    string command;
    if(hasPipe == true){
        input = pipeSplit[0];
    }

    string* args;
    int numArgs = 0;    
    splitArgs(input, command, args, numArgs);

    if(commandIsAllowed(command) == false){
        cout << "You don't have permission to use command \""<< command << "\"" << endl;
        return false;
    }

    char** argsAry;
    stringArrayToCharArray(args, argsAry, numArgs);
    if(command.compare("cd") == 0){
        chdir(argsAry[1]);
    }
    else{
        if(hasPipe == true){
            // Pipe code from here:
            // https://stackoverflow.com/questions/52737554/redirect-stdout-back-to-console
            int oldStdout;
            oldStdout = dup(1);
            if(oldStdout == -1){
                cout << "Unable to duplicate stream" << endl;
                return false;
            }
            
            FILE *DataFile = fopen(pipeSplit[1].c_str(), "w");
            dup2(fileno(DataFile), 1);

            if(fork() == 0){
                execvp(command.c_str(), argsAry);
            }
            else{
                wait(NULL);
            }

            fflush(stdout);
            fclose(DataFile);
            dup2(oldStdout, 1);
        }
        else{
            if(fork() == 0){
                execvp(command.c_str(), argsAry);
            }
            else{
                wait(NULL);
            }
        }
    }
    
    return true;
}
