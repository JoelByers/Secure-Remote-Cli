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
    server.sin_port = htons(8421);

	if (connect(socket_description , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
        cout << "Unable to connect" << endl;
		return 1;
	}

    cout << "Connected to Server\n";

    // Cert ///////////////////////////////////////////////////////////////////////////////
    cout << "Certs:" << endl;
    int numCerts = argc - 1;

    CertGroup certGroup;
    Cert487 clientCert(Cert487("client.txt"));
    certGroup.addCert(clientCert);
    clientCert.printLess();
    for(int i = 0; i < numCerts; i++){
        Cert487 cert(argv[i + 1]);
        cert.printLess();
        cout << "----------------------------------------------------\n";
        certGroup.addCert(cert);
    }    

    cout << "----------------------------------------------------\n";

    // Get server cert
    CertData serverCertData;
    recv(socket_description, &serverCertData, sizeof(serverCertData), 0);
    Cert487 serverCert(serverCertData);
    serverCert.printLess();
    certGroup.addCert(serverCert);

    // Send Client cert to server
    // TODO: Bug Sending cert
    CertData clientCertData = clientCert.getData();
    if(send(socket_description , &clientCertData, sizeof(clientCertData), 0) < 0)
    {
        cout << "Unable to send server data to client";
        return 1;
    }    

    // DH-RSA /////////////////////////////////////////////////////////////////////////////
    DiffieHellmanRSA dhrsa(socket_description);
    dhrsa.clientGetPrivateKey();
    bool key[10] = {0,0,0,0,0,0,0,0,0,0};
    asciiToBinary((char)dhrsa.getPrivateKey(), key);

    cout << "Private Key: " << dhrsa.getPrivateKey() << endl;

    // CLI ////////////////////////////////////////////////////////////////////////////////
    char message[100];
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
        cout << "'" << command << "'"<< endl;
        cout << command.length() << endl;

        cli.call(command);
    }
    while(strcmp(message, "quit") != 0);

    cout << "Closing Connection..." << endl;

    close(socket_description);

    return 0;
}   