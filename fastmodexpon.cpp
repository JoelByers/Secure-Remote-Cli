#include <iostream>
#include <string>
#include <fstream>
#include <time.h>

using namespace std;
int randPrime(){
    string tempstr;
    int random;
    int counter = 0;
    srand(time(NULL));
    random = rand() % 25 + 1;
    ifstream inFile("primes.txt");
    while(inFile >> tempstr){
        if(counter == random){
            return stoi(tempstr.erase(tempstr.length()-1,tempstr.length()));
        }
        counter++;
    }
    return 0;
}
//Part of code segment taken from: https://www.educative.io/answers/how-to-convert-a-number-from-decimal-to-binary-in-cpp
void intToBinary(int input, bool binaryArray[32]){
    int binary =0;
    int remainder, product = 1;
    int decimal = int(input);
    for(int i = 0; i<32; i++){
        binaryArray[i] = 0;
    }
    while(decimal != 0){
        remainder = decimal %2;
        binary = binary + (remainder * product);
        decimal = decimal / 2;
        product *= 10;
    }
    string tempstr = to_string(binary);
    int difference = 32- tempstr.length();
    for(int i = 0; i<tempstr.length(); i++){
        binaryArray[i+difference] = int(tempstr[i])-48;
    }
}


int FastModExpon(int base, int power, int mod){
    bool binaryArray[32];
    intToBinary(power, binaryArray);
    int i = 0;
    int result = base;
    while(binaryArray[i] !=1){
        i++;
    }
    i++;
    while(i<32){
        if(binaryArray[i]==true){
            result = (result*result*base)%mod;
        }
        else{
            result = (result*result)%mod;
        }
        i++;
    }
    return result;
}
/* 
int main(){
    cout<< randPrime() <<endl;
    cout<< FastModExpon(151,12,257)<<endl;
    return 0;
} */