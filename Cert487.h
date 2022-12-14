#ifndef _CERT_487_H
#define _CERT_487_H

#include <string>
using namespace std;

struct CertData{
    int version;
    int serialNumber;
    char signatureAlgorithmIdentity[50];
    char signatureAlgorithmParameters[50];
    char issuerName[50];
    int validNotBefore;
    int validNotAfter;
    char subjectName[50];
    char publicKeyAlgorithm[50];
    char publicKeyParameters[50];
    int publicKey;
    char issuerUniqueIdentifier[50];
    int trust;
    char extensions[50];
    char signatureAlgorithm[50];
    char signatureParameters[50];
    char signature;
};

class Cert487{
    private:
        CertData data;
        void printLine(string label, string data);
        void parseCertLine(string input, string output[2]);
        void writeLineToFile(ofstream &fileOut, string label, string data);
        char cbcHash(string fileName);
        void sign(string signerFileName);
    public:
        Cert487(string fileName);
        Cert487(CertData data);
        void print();
        void printLess();
        void writeToFile(string fileName);
        int getSerialNumber();
        string getIssuer();
        string getSubjectName();
        CertData getData();
        int getPublicKey();
};

#endif