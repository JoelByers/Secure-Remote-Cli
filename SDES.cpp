#include <iostream>
#include <sstream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

void printBitArray(bool* ary, int arySize){
    for(int i = 0; i < arySize; i++){
        cout << ary[i] << " ";
    }

    cout << endl;
}

using namespace std;
void p10(bool key[10]){
    int newpos[10] = {3,5,2,7,4,10,1,9,8,6};
    bool temp[10];
    for(int i=0; i<10; i++)
    {
        temp[i] = key[newpos[i]-1];
    }
    for(int i=0; i<10; i++){
        key[i] = temp[i];
    }
}

void p8(bool key[10], bool nkey[8]){
    int newpos[8] = {6,3,7,4,8,5,10,9};
    for(int i = 0; i<8; i++){
        nkey[i] = key[newpos[i]-1];
    }
}

void ip(bool key[8]){
    int newpos[8] = {2,6,3,1,4,8,5,7};
    bool temp[8];
    for(int i = 0; i<8; i++){
        temp[i] = key[newpos[i]-1];
    }
    for(int i=0; i<8; i++){
        key[i] = temp[i];
    }
}

void ep(bool key[4], bool nkey[8]){
    int newpos[8] = {4,1,2,3,2,3,4,1};
    for(int i = 0; i<8; i++){
        nkey[i] = key[newpos[i]-1];
    }
}

void p4(bool key[4]){
    int newpos[4] = {2,4,3,1};
    bool temp[4];
    for(int i=0; i<4; i++){
        temp[i] = key[newpos[i]-1];
    }
    for(int i=0; i<4; i++){
        key[i] = temp[i];
    }
}


void ipinverse(bool key[8]){
    int newpos[8] = {4,1,3,5,7,2,8,6};
    bool temp[8];
    for(int i = 0; i<8; i++){
        temp[i] = key[newpos[i]-1];
    }
    for(int i=0; i<8; i++){
        key[i] = temp[i];
    }
}

// Perform and Xor between outArray and xorArray and store the 
// result in outArray.
void exclusiveOr(bool* outArray, bool* xorArray, int arraySize){
    for(int i = 0; i < arraySize; i++)
    {
        outArray[i] = outArray[i] != xorArray[i];
    }
}

void leftShift(bool* array, int arraySize){
    bool firstBit = array[0];
    for(int i = 0; i < (arraySize - 1); i++)
    {
        array[i] = array[i + 1];
    }
    array[arraySize - 1] = firstBit;
}

void circularShift(bool array[10]){
    bool aryLeft[5] = {0,0,0,0,0};
    bool aryRight[5] = {0,0,0,0,0};
    for(int i = 0; i < 5; i++){
        aryLeft[i] = array[i];
        aryRight[i] = array[i+5];
    }

    leftShift(aryLeft, 5);
    leftShift(aryRight, 5);

    for(int i = 0; i < 5; i++){
        array[i] = aryLeft[i];
        array[i+5] = aryRight[i];
    }
}

// Return a short representation of a 2 element bit array
//
// function found on this page:
// https://stackoverflow.com/questions/20434787/convert-bit-array-to-short
short shortFromBits(bool bits[2]){
    short res = 0;

    // Handle first two bits
    if (bits[0]) {
        res |= 1 << 1;
    }
    if (bits[1]) {
        res |= 1 << 0;
    }

    // Pad
    for(int i = 0; i < 13; i++){
        res |= 0 << (i + 2);
    }
    return res;
}

// return the bit representation of a short num
// where 0 <= num <= 3
void bitsFromShort(short num, bool bits[2]){
    bits[0] = num & (1 << 1);
    bits[1] = num & (1 << 0);
}

// Perform the S-Box lookup. `Data` is the array used to
// derrive the row and column, and `box` is the S table. The
// result will be stored in the array `output`
void _s(bool data[4], bool output[2], short box[4][4]){
    bool rowAry[2];
    bool colAry[2];
    rowAry[0] = data[0];
    rowAry[1] = data[3];
    colAry[0] = data[1];
    colAry[1] = data[2];

    short row = shortFromBits(rowAry);
    short col = shortFromBits(colAry);
    int boxResult = box[row][col];

    bitsFromShort(boxResult, output);
}

// Perfrom the S0 computation. This function
// is used by sBox.
void _sZero(bool data[4], bool output[2]){
    short box[4][4] = {{1,0,3,2},
                     {3,2,1,0},
                     {0,2,1,3},
                     {3,1,3,2}};
    _s(data, output, box);
}

// Perfrom the S1 computation. This function
// is used by sBox.
void _sOne(bool data[4], bool output[2]){
    short box[4][4] = {{0,1,2,3},
                     {2,0,1,3},
                     {3,0,1,0},
                     {2,1,0,3}};
    _s(data, output, box);
}

// Perform the compete SBox computation
void sBox(bool data[8], bool output[4]){
    bool leftHalf[4];
    bool rightHalf[4];
    for(int i = 0; i < 4; i++){
        leftHalf[i] = data[i];
        rightHalf[i] = data[i + 4];
    }

    bool leftOut[2];
    bool rightOut[2];
    _sZero(leftHalf, leftOut);
    _sOne(rightHalf, rightOut);

    for(int i = 0; i < 2; i++){
        output[i] = leftOut[i];
        output[i + 2] = rightOut[i];
    }
}

// Swap the first half of the array
// with the second, and the second with
// the first
void swapHalf(bool data[8]){
    bool temp;

    for(int i = 0; i < 4; i++){
        temp = data[i];
        data[i] = data[i + 4];
        data[i + 4] = temp;
    }
}

void getKeys(bool theKey[10], bool keyOne[8], bool keyTwo[8]){
    p10(theKey);

    circularShift(theKey);
    p8(theKey, keyOne);

    // get key 2
    circularShift(theKey);
    circularShift(theKey);
    p8(theKey, keyTwo);
}

// Perform all F-Box operations
//
//                              key1              Bits 0-3
//                               |                    |
//                               v                    v
//  8 Bits -> Bits 4-7 -> EP -> XOR -> SBox -> P4 -> Xor -> Recombine L and R
//
void fbox(bool input[8], bool key[8]){
    bool rightHalf[4] = {0,0,0,0};
    bool leftHalf[4] = {0,0,0,0};
    for(int i = 0; i < 4; i++){
        rightHalf[i] = input[i + 4];
        leftHalf[i] = input[i];
    }

    bool epOut[8] = {0,0,0,0,0,0,0,0};
    ep(rightHalf, epOut);
    exclusiveOr(epOut, key, 8);
    bool sBoxOut[4] = {0,0,0,0};
    sBox(epOut, sBoxOut);
    p4(sBoxOut);
    exclusiveOr(sBoxOut, leftHalf, 4);

    for(int i = 0; i < 4; i++){
        input[i] = sBoxOut[i];
    }
}

//Part of code segment taken from: https://www.educative.io/answers/how-to-convert-a-number-from-decimal-to-binary-in-cpp
void asciiToBinary(char letter, bool binaryArray[8]){
    int binary =0;
    int remainder, product = 1;
    int decimal = int(letter);
    for(int i = 0; i<8; i++){
        binaryArray[i] = 0;
    }
    while(decimal != 0){
        remainder = decimal %2;
        binary = binary + (remainder * product);
        decimal = decimal / 2;
        product *= 10;
    }
    string tempstr = to_string(binary);
    int difference = 8- tempstr.length();
    for(int i = 0; i<tempstr.length(); i++){
        binaryArray[i+difference] = int(tempstr[i])-48;
    }
}

// Solution found here:
// https://stackoverflow.com/questions/23344257/convert-a-string-of-binary-into-an-ascii-string-c
char binaryToAscii(bool binary[8]){
    string binaryStr = "";
    for(int i = 0; i < 8; i++){
        binaryStr.push_back(binary[i] ? '1' : '0');
    }
    
    std::stringstream sstream(binaryStr);
    std::string output;
    std::bitset<8> bits;
    sstream >> bits;
    char c = char(bits.to_ulong());
    
    return c;
}

char binaryToChar(bool binary[8]){
    char binString[8] = {};
    for(int i = 0; i < 8; i++){
        binString[i] = (binary[i] ? '1' : '0');
    }
    char c = strtol(binString, 0, 2);

    return c;
}

void encrypt(bool data[8], bool key[10]){
    bool keyOne[8] = {0,0,0,0,0,0,0,0}; 
    bool keyTwo[8] = {0,0,0,0,0,0,0,0}; 
    getKeys(key, keyOne, keyTwo);
    ip(data);
    fbox(data, keyOne);
    swapHalf(data);
    fbox(data, keyTwo);
    ipinverse(data);
}

void decrypt(bool data[8], bool key[10]){
    bool keyOne[8] = {0,0,0,0,0,0,0,0}; 
    bool keyTwo[8] = {0,0,0,0,0,0,0,0}; 
    getKeys(key, keyOne, keyTwo);
    ip(data);
    fbox(data, keyTwo);
    swapHalf(data);
    fbox(data, keyOne);
    ipinverse(data);
}
