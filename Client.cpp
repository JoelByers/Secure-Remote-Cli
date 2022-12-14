#include <iostream>   
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include "fastmodexpon.h"
#include <unistd.h>
#include "SDES.h"
#include "RSA.h"
#include "DiffieHellmanRSA.h"
#include "Cli.h"
#include "Cert487.h"
#include "CertGroup.h"
#include <string>
#include "CRL.h"

using namespace std;

int main(int argc, char** argv){
    // create socket
    int socket_description = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_description == -1){
        cout << "Unable to create socket" << endl;
        return 1;
    }

    struct sockaddr_in server;
    // server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_addr.s_addr = inet_addr("0.0.0.0");
	server.sin_family = AF_INET;
    server.sin_port = htons(8235);

	if (connect(socket_description , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
        cout << "Unable to connect" << endl;
		return 1;
	}

    cout << "Connected to Server\n";

    // CRL /////////////////////////////////////////////////////////////////////////////////
    CRL crl("crl.txt");
    cout << "\nCRL:" << endl;
    crl.print();
    cout << "=============================================" << endl;
  

    // Cert ///////////////////////////////////////////////////////////////////////////////
    cout << "Certs:" << endl;

    CertGroup certGroup;
    Cert487 clientCert(Cert487("client.txt"));
    certGroup.addCert(clientCert);
    clientCert.printLess();
    cout << "----------------------------------------------------\n";

    // Get server cert
    CertData serverCertData;
    recv(socket_description, &serverCertData, sizeof(serverCertData), 0);
    Cert487 serverCert(serverCertData);
    serverCert.printLess();
    certGroup.addCert(serverCert);

    // Send Client cert to server
    CertData clientCertData = clientCert.getData();
    if(send(socket_description , &clientCertData, sizeof(clientCertData), 0) < 0)
    {
        cout << "Unable to send server data to client";
        return 1;
    }    

    cout << endl;
    // Verify Server Cert
    if(certGroup.validateChain(clientCert.getSerialNumber(),serverCert.getSerialNumber() , crl) == false){
        cout << "Unable to validate Chain" << endl;
        return 1;
    }
    cout << "====================================================" << endl;

    // DH-RSA /////////////////////////////////////////////////////////////////////////////

    bool useDiffieHellman;
    recv(socket_description, &useDiffieHellman, sizeof(useDiffieHellman), 0);

    bool key[10] = {0,0,0,0,0,0,0,0,0,0};
    if(useDiffieHellman == true){
        cout << "Using Signed Diffie-Hellman" << endl;
        DiffieHellmanRSA dhrsa(socket_description);
        dhrsa.clientGetPrivateKey();
        asciiToBinary((char)dhrsa.getPrivateKey(), key);
        cout << "Private Key: " << dhrsa.getPrivateKey() << endl;
    }
    else{
        cout << "Using cutsom key" << endl;
        RSA rsa;
        int entryptedKey;
        recv(socket_description, &entryptedKey, sizeof(entryptedKey), 0);
        int sharedKey = rsa.decrypt(entryptedKey, rsa.getD());
        asciiToBinary((char)sharedKey, key);
        cout << "Private Key: " << sharedKey << endl;
    }
    cout << "====================================================" << endl;

    // CLI ////////////////////////////////////////////////////////////////////////////////

    // do{
    //     cerr << endl << "@Host $ ";

    //     int received = 0;
    //     bool encryptedBytes[100][8] = {{}};
    //     recv(socket_description, &encryptedBytes, sizeof(encryptedBytes), 0);

    //      // Decrypt
    //     for(int i = 0; i < 100; i++){
    //         decrypt(encryptedBytes[i], key);
    //         message[i] = binaryToChar(encryptedBytes[i]);
    //     }
    //     cout << endl;
    //     string command(message);
    //     cli.call(command);   
    // }
    // while(strcmp(message, "quit") != 0);

    bool encryptedBytes[100][8] = {{}};
    char message[100] = {};
    Cli cli;

    while(recv(socket_description, &encryptedBytes, sizeof(encryptedBytes), 0) > 0){
        cerr << endl << "@Host $ ";

        int received = 0;

         // Decrypt
        for(int i = 0; i < 100; i++){
            decrypt(encryptedBytes[i], key);
            message[i] = binaryToChar(encryptedBytes[i]);
        }

        cout << endl;

        if(strcmp(message, "quit") == 0){
            break;
        }

        //string command(message);
        cli.call(message);   

		memset(encryptedBytes, 0 , 800);
		memset(message ,'\0' , 100);

    }

    cout << "Closing Connection..." << endl;

    close(socket_description);

    return 0;
}   