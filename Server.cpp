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
#include "DiffieHellmanRSA.h"
#include "Cert487.h"
#include "CertGroup.h"

using namespace std;

int main(int argc, char** argv){

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
    cout << "Connecting..." << endl;

    // listen for client
    listen(socket_description, 3);

    int c = sizeof(struct sockaddr_in);
    int new_socket = accept(socket_description, (struct sockaddr*) &client, (socklen_t*)&c);
    if(new_socket < 0){
        cout << "Failed to accept client connection" << endl;
        return 1;
    }

    cout << "Connected" << endl;
    // CRL /////////////////////////////////////////////////////////////////////////////////

    CRL crl("crl.txt");
    cout << "\nCRL:" << endl;
    crl.print();
    cout << "=============================================" << endl;
  
    // Cert //////////////////////////////////////////////////////////////////////////////

    int numCerts = argc - 1;

    cout << "Certs:" << endl;

    CertGroup certGroup;
    Cert487 serverCert(Cert487("Server.txt"));
    certGroup.addCert(serverCert);
    serverCert.printLess();

    for(int i = 0; i < numCerts; i++){
        Cert487 cert(argv[i + 1]);
        cert.printLess();
        cout << "----------------------------------------------------\n";
        certGroup.addCert(cert);
    }    

    cout << "----------------------------------------------------\n";

    // Send server cert to client
    CertData serverCertData = serverCert.getData();
    if(send(new_socket , &serverCertData, sizeof(serverCertData), 0) < 0)
    {
        cout << "Unable to send server data to client";
        return 1;
    }

    // Get client cert
    CertData clientCertData;
    recv(new_socket, &clientCertData, sizeof(clientCertData), 0);
    Cert487 clientCert(clientCertData);
    clientCert.printLess();
    certGroup.addCert(clientCert);

    // DH-RSA ////////////////////////////////////////////////////////////////////////////
    bool key[10] = {0,0,0,0,0,0,0,0,0,0};
    DiffieHellmanRSA dhrsa(new_socket);
    dhrsa.serverGetPrivateKey();
    asciiToBinary((char)dhrsa.getPrivateKey(), key);

    cout << "Private Key: " << dhrsa.getPrivateKey() << endl;

    // CLI ////////////////////////////////////////////////////////////////////////////////

    string command = "";
    
    do{
        char messageAry[100] = {};
        bool encryptedBytes[100][8] = {{}};
        cout << "Remote $ ";
        getline(cin, command);
        strcpy(messageAry, command.c_str());

        // length + 1 for null terminator
        for(int i = 0; i < command.length() + 1; i++){
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
    }
    while(command != "quit");

    cout << "Closing connection..." << endl;

    close(socket_description);

    return 0;
}