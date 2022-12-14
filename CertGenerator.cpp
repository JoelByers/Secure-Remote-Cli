#include "Cert487.h"
#include <iostream>

int main(int argc, char** argv){

    if(argc < 3){
        cerr << "Not enough arguments. Usage: executable.out <inputfile> <outputfile>\n";
        return 1;
    }

    string inputFile = argv[1];
    string outputFile = argv[2];

    Cert487 cert(inputFile);
    cert.print();
    cert.writeToFile(outputFile);

    return 0;
}