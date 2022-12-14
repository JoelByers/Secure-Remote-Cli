#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include "CRL.h"
#include "RSA.h"
#include "SDES.h"
#include "Cert487.h"

using namespace std;

void parseLine(string input, string output[2]){
    output[0] = input.substr(0, input.find("="));
    output[1] = input.substr(input.find("=") + 1, input.length() - 1);
}

CRL::CRL(string fileName){
    crlobject tempcrlobject;
    ifstream crlFile;
    crlFile.open(fileName);
    ofstream outfile;
    //outfile.open("CRLout.txt");
    string temp;
    string parsedInput[2] = {"",""};
    while(!crlFile.eof()){
        getline(crlFile, temp);
        parseLine(temp, parsedInput);
        if(parsedInput[0].compare("signatureAlgorithmIdentity")==0){
            strcpy(tempcrlobject.signatureAlgorithmIdentity, parsedInput[1].c_str());
        }
        else if(parsedInput[0].compare("signatureAlgorithmParameters")==0){
            strcpy(tempcrlobject.signatureAlgorithmParameters, parsedInput[1].c_str());
        }
        else if(parsedInput[0].compare("issuerName")==0){
            strcpy(tempcrlobject.issuerName, parsedInput[1].c_str());
        }
        else if(parsedInput[0].compare("thisDate")==0){
            tempcrlobject.thisDate = stoi(parsedInput[1]);
        }
        else if(parsedInput[0].compare("nextDate")==0){
            tempcrlobject.nextDate = stoi(parsedInput[1]);
        }
        else if(parsedInput[0].compare("revokedSerialNumber")==0){
            tempcrlobject.revokedSerialNumber = stoi(parsedInput[1]);
        }
        else if(parsedInput[0].compare("revokedDate")==0){
            tempcrlobject.revokedDate = stoi(parsedInput[1]);
            crlList.push_back(tempcrlobject);
        }
    }
    //outfile<<"signature="<<cbcHash(fileName);
    //outfile.close();
}

CRL::CRL(){}

void CRL::printCRL(string fileName){
    ifstream crlFile;
    crlFile.open(fileName);
    ofstream outfile;
    outfile.open("CRLout.txt");
    string temp;
    while(!crlFile.eof()){
        getline(crlFile, temp);
        outfile<<temp<<endl;
    }
    outfile<<"signature="<<cbcHash(fileName);
    outfile.close();
}

void CRL::print(){
    for(crlobject obj : crlList){
        cout << "Signature Algorithm Identity: " << obj.signatureAlgorithmIdentity << endl;
        cout << "Signature Algorithm Parameters: " << obj.signatureAlgorithmParameters << endl;
        cout << "Issuer Name: " << obj.issuerName << endl;
        cout << "This Date: " << obj.thisDate << endl;
        cout << "Next Date: " << obj.nextDate << endl;
        cout << "Revoked Serial Number: " << obj.revokedSerialNumber << endl;
        cout << "Revoked Date: " << obj.revokedDate << endl << endl;
    }
    
}

bool CRL::cbcHashCheck(string fileName){
	fstream infile(fileName);
	string temp;
    string temp1;
    string tempSerialString;
    char sig;
    bool sig_present = false;
    int tempSerialNum;
    RSA rsa;
	
    bool iv[8] = {0,0,0,0,0,0,0,0};

	while(getline(infile, temp)){
        if(temp.length()>10){
            temp1 = temp.substr(0,10);
            if(strcmp(temp1.c_str(),"signature=")==0){
                sig = rsa.encrypt(int(temp[temp.length()-1]),rsa.getE());
                sig_present = true;
            }
        }
        if(temp.length()>13 &&sig_present == false){
            try{
                temp1 = temp.substr(0,13);
            }
            catch(...){
                cout<<"ERROR"<<endl;
            }
            
        }
        if(strcmp(temp1.c_str(),"serialNumber=")==0){
            try{
                tempSerialString = temp.substr(13,temp.length());
            }
            catch(...){
                cout<<"ERROR CAUGHT"<<endl;
            }
            tempSerialNum = stoi(tempSerialString);
        }
		for(int i = 0; i < temp.length(); i++){
			bool bits[8] = {0,0,0,0,0,0,0,0};
			bool key[10] = {0,0,0,0,0,0,0,0,0,0};
			asciiToBinary(temp[i], bits);
			exclusiveOr(bits, iv, 8); //exclusive or before encrypting with iv
			encrypt(bits, key);
				
			for(int j = 0; j < 8; j++){
				iv[j] = bits[j];
			}
			temp[i] = binaryToAscii(bits);
		}
	}
    if(sig_present == true){
        cout<<sig<<" : "<<binaryToAscii(iv)<<endl;
        if(sig != binaryToAscii(iv)){
            return false;
        }
    }
    
	return true;
}

char CRL::cbcHash(string fileName){
	fstream infile(fileName);
	string temp;
	
    bool iv[8] = {0,0,0,0,0,0,0,0};

	while(getline(infile, temp)){
		for(int i = 0; i < temp.length(); i++){
			bool bits[8] = {0,0,0,0,0,0,0,0};
			bool key[10] = {0,0,0,0,0,0,0,0,0,0};
			asciiToBinary(temp[i], bits);
			exclusiveOr(bits, iv, 8); //exclusive or before encrypting with iv
			encrypt(bits, key);
				
			for(int j = 0; j < 8; j++){
				iv[j] = bits[j];
			}
			temp[i] = binaryToAscii(bits);
		}
	}
    
	return binaryToAscii(iv);
}

crlobject CRL::getObj(int index){
    return crlList.at(index);
}

void CRL::addObj(crlobject obj){
    crlList.push_back(obj);
}

bool CRL::find(int serialNum){
    for(auto const &i: crlList){
        if(i.revokedSerialNumber == serialNum){
            return true;
        }
    }
    return false;
}

int CRL::getNumObj(){
    return crlList.size();
}