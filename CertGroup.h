#ifndef _CERT_GROUP_H
#define _CERT_GROUP_H

#include <vector>
#include <stack>
#include "Cert487.h"
#include "CRL.h"

class CertGroup {
    private:
        vector<Cert487> certs;
        bool findNextLink(int currentIndex, int certTwoSerial, CRL crl);
    public:
        void addCert(Cert487 cert);
        bool validateChain(int certOneSerial, int certTwoSerial, CRL crl);
        void print();

};

#endif