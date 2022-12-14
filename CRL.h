#ifndef _CRL_H
#define _CRL_H

#include <string>
#include <vector>
#include "Cert487.h"

using namespace std;

struct crlobject{
    char signatureAlgorithmIdentity[50];
    char signatureAlgorithmParameters[50];
    char issuerName[50];
    int thisDate = 2;
    int nextDate=4;
    int revokedSerialNumber;
    int revokedDate=2;
};

class CRL{
    private:
        vector<crlobject> crlList;
        char cbcHash(string fileName);
        bool cbcHashCheck(string fileName);
    public:
        CRL(string fileName);
        CRL();
        void printCRL(string fileName);
        void print();
        crlobject getObj(int index);
        void addObj(crlobject obj);
        int getNumObj();
        bool find(int serialNum);
};

#endif