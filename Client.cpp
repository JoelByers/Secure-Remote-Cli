#include <iostream>   
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include "fastmodexpon.h"
#include <unistd.h>
#include "SDES.h"
#include "RSA.h"
#include "DiffieHellmanRSA.h"

using namespace std;

int main(){
    // create socket
    int socket_description = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_description == -1){
        cout << "Unable to create socket" << endl;
        return 1;
    }

    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr("0.0.0.0");
	server.sin_family = AF_INET;
    server.sin_port = htons(8421);

	if (connect(socket_description , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
        cout << "Unable to connect" << endl;
		return 1;
	}

    cout << "Connected to Server\n";

    DiffieHellmanRSA dhrsa(socket_description);
    dhrsa.clientGetPrivateKey();
    bool key[10] = {0,0,0,0,0,0,0,0,0,0};
    asciiToBinary((char)dhrsa.getPrivateKey(), key);

    cout << "Private Key: " << dhrsa.getPrivateKey() << endl;

    char message[100];

    do{
        int received = 0;
        bool encryptedBytes[100][8] = {{}};
        recv(socket_description, &encryptedBytes, sizeof(encryptedBytes), 0);

        // Decrypt
        for(int i = 0; i < 100; i++){
            decrypt(encryptedBytes[i], key);
            message[i] = binaryToChar(encryptedBytes[i]);
        }

        cout << message << endl;
    }
    while(strcmp(message, "quit") != 0);

    cout << "Closing Connection..." << endl;

    close(socket_description);

    return 0;
}   