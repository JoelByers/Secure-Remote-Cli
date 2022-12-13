#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "fastmodexpon.h"
#include <time.h>
#include <cstdlib>
#include <cstring>
#include "SDES.h"
#include "RSA.h"
#include <unistd.h>

using namespace std;

struct DiffieHellmanServerData{
    int base;
    int mod;
    int serverResult;
};

int main(){

    // create socket
    int socket_description = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_description == -1){
        cout << "Unable to create socket" << endl;
        return 1;
    }

    struct sockaddr_in server;
    struct sockaddr_in client;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8421);

    // bind to port
    if(bind(socket_description,(struct sockaddr*) &server, sizeof(server)) < 0){
        cout << "Unable to bind to port" << endl;
        return 1;
    }
    cout << "Socket bound, waiting for client..." << endl;

    // listen for client
    listen(socket_description, 3);

    int c = sizeof(struct sockaddr_in);
    int new_socket = accept(socket_description, (struct sockaddr*) &client, (socklen_t*)&c);
    if(new_socket < 0){
        cout << "Failed to accept client connection" << endl;
        return 1;
    }

    // RSA ///////////////////////////////////////////////////////////////////////////////////////
    RSA rsa;
    int serverRsaE = rsa.getE();

    cout << "Sending RSA keys to client...\n";
    if( send(new_socket , &serverRsaE, sizeof(serverRsaE), 0) < 0)
	{
		cout << "Unable to send server data to client";
		return 1;
	}

    int clientRsaE;
    recv(new_socket, &clientRsaE, sizeof(clientRsaE), 0) > 0;
    cout << clientRsaE << endl;

    // DIFFIE-HELLMAN /////////////////////////////////////////////////////////////////////////////

    // Choose public base and mod

    int mod = randPrime();
    int base = 3;
    // Choose a Server secret number
    srand(time(0));
    int serverSecret = rand() % 50;

    // Raise base to secret number
    
    int serverResult = FastModExpon(base, serverSecret, mod);

    // Send to client
    DiffieHellmanServerData data;
    data.base = rsa.encrypt(base, clientRsaE);
    data.mod = rsa.encrypt(mod, clientRsaE);
    data.serverResult = rsa.encrypt(serverResult, clientRsaE);
    // data.base = base;
    // data.mod = mod;
    // data.serverResult = serverResult;

    cout << "Sending modulus, base, and server result to Client...\n";
    if( send(new_socket , &data, sizeof(data), 0) < 0)
	{
		cout << "Unable to send server data to client";
		return 1;
	}
    cout << "Sent, waiting on client result\n";

    // Wait for client response
    int clientResult = -1;

    recv(new_socket, &clientResult, sizeof(clientResult), 0) > 0;
    clientResult = rsa.decrypt(clientResult);

    cout << "-------------------------------------\n";
    cout << "Base          : " << base << "\n";
    cout << "Mod           : " << mod << "\n";
    cout << "Server Secret : " << serverSecret << "\n";
    cout << "Server Result : " << serverResult << "\n";
    cout << "Client Result : " << clientResult << "\n";
    cout << "-------------------------------------\n";

    cout << "Received result from Client...\n";
    // Raise base to client response
    int privateKey = FastModExpon(clientResult, serverSecret, mod);
    bool key[10] = {0,0,0,0,0,0,0,0,0,0};
    asciiToBinary((char)privateKey, key);

    cout << "Private Key: " << privateKey << endl;

    cout << "Enter a message to send securely (< 100 chars): ";
    string message;
    getline(cin,message);

    char messageAry[100] = {};
    strcpy(messageAry, message.c_str());
    bool encryptedBytes[100][8] = {{}};
    
    // for(int i = 0; i < message.length(); i++){
    //     bool charBits[8] = {0,0,0,0,0,0,0,0};
    //     asciiToBinary(messageAry[i], charBits);
    //     for(int j = 0; j < 8; j++){
    //         encryptedBytes[i][j] = charBits[j];
    //     }

    //     encrypt(charBits,key);

    //     for(int j = 0; j < 8; j++){
    //         encryptedBytes[i][j] = charBits[j];
    //     }
    // }

    // length + 1 for null terminator
    for(int i = 0; i < message.length() + 1; i++){
        bool charBits[8] = {0,0,0,0,0,0,0,0};
        asciiToBinary(messageAry[i], charBits);
        for(int j = 0; j < 8; j++){
            encryptedBytes[i][j] = charBits[j];
        }

        encrypt(charBits,key);

        for(int j = 0; j < 8; j++){
            encryptedBytes[i][j] = charBits[j];
        }
    }

    if(send(new_socket , &encryptedBytes, sizeof(encryptedBytes), 0) < 0)
	{
		cout << "Unable to send server data to client";
		return 1;
	}

    close(socket_description);

    return 0;
}