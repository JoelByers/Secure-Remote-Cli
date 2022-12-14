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
    server.sin_addr.s_addr = inet_addr("0.0.0.0");
	server.sin_family = AF_INET;
    server.sin_port = htons(8431);

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
    // for(int i = 0; i < numCerts; i++){
    //     Cert487 cert(argv[i + 1]);
    //     cert.printLess();
    //     cout << "----------------------------------------------------\n";
    //     certGroup.addCert(cert);
    // }    

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

    // Verify Server Cert
    if(certGroup.validateChain(clientCert.getSerialNumber(),serverCert.getSerialNumber() , crl) == false){
        cout << "Unable to validate Chain" << endl;
        return 1;
    }
    cout << "=============================================" << endl;

    // DH-RSA /////////////////////////////////////////////////////////////////////////////

    bool useDiffieHellman;
    recv(socket_description, &useDiffieHellman, sizeof(useDiffieHellman), 0);

    bool key[10] = {0,0,0,0,0,0,0,0,0,0};
    if(useDiffieHellman == true){
        DiffieHellmanRSA dhrsa(socket_description);
        dhrsa.clientGetPrivateKey();
        asciiToBinary((char)dhrsa.getPrivateKey(), key);
        cout << "Private Key: " << dhrsa.getPrivateKey() << endl;
    }
    else{
        RSA rsa;
        int entryptedKey;
        recv(socket_description, &entryptedKey, sizeof(entryptedKey), 0);
        int sharedKey = rsa.decrypt(entryptedKey, rsa.getD());
        asciiToBinary((char)sharedKey, key);
        cout << "Private Key: " << sharedKey << endl;
    }

    // CLI ////////////////////////////////////////////////////////////////////////////////
    char message[100] = {};
    Cli cli;

    do{
        int received = 0;
        bool encryptedBytes[100][8] = {{}};
        recv(socket_description, &encryptedBytes, sizeof(encryptedBytes), 0);

         // Decrypt
        for(int i = 0; i < 100; i++){
            decrypt(encryptedBytes[i], key);
            message[i] = binaryToChar(encryptedBytes[i]);
        }

        string command(message);
        cli.call(command);
    }
    while(strcmp(message, "quit") != 0);

    cout << "Closing Connection..." << endl;

    close(socket_description);

    return 0;
}   