#include "DiffieHellmanRSA.h"
#include <cstdlib>
#include <time.h>
#include "fastmodexpon.h"
#include <sys/socket.h>
#include <iostream>
#include "RSA.h"

using namespace std;

DiffieHellmanRSA::DiffieHellmanRSA(int socket){
    this->socket = socket;
}

void DiffieHellmanRSA::serverGetPrivateKey(){

    // RSA ///////////////////////////////////////////////////////////////////////////////////////
    RSA rsa;
    int serverRsaE = rsa.getE();

    if( send(socket , &serverRsaE, sizeof(serverRsaE), 0) < 0)
	{
		cout << "Unable to send server RSA Keys to client";
		return;
	}

    int clientRsaE;
    recv(socket, &clientRsaE, sizeof(clientRsaE), 0) > 0;

    // Diffie-Hellman /////////////////////////////////////////////////////////////////////////////

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
    // data.base = base;
    // data.mod = mod;
    // data.serverResult = serverResult;
    data.base = rsa.encrypt(base, clientRsaE);
    data.mod = rsa.encrypt(mod, clientRsaE);
    data.serverResult = rsa.encrypt(serverResult, clientRsaE);

    if( send(this->socket , &data, sizeof(data), 0) < 0)
	{
		cout << "Unable to send Diffie-Hellman data to client";
		return;
	}

    // Wait for client response
    int clientResult = -1;
    recv(this->socket, &clientResult, sizeof(clientResult), 0) > 0;
    clientResult = rsa.decrypt(clientResult);
    this->privateKey = FastModExpon(clientResult, serverSecret, mod);
}

void DiffieHellmanRSA::clientGetPrivateKey(){

    // RSA //////////////////////////////////////////////////////////////////////////
    int serverRSAKey;
    recv(socket, &serverRSAKey, sizeof(serverRSAKey), 0);

    RSA rsa;
    int ClientRsaE = rsa.getE();

    if(send(socket , &ClientRsaE, sizeof(ClientRsaE) , 0) < 0)
	{
		cout << "Unable to send RSA Keys to server" << endl;
		return;
	}

    // Diffie-Hellman ////////////////////////////////////////////////////////////////
    DiffieHellmanServerData serverData;

    recv(this->socket, &serverData, sizeof(serverData), 0);
    
    // Decrypt with RSA
    serverData.base = rsa.decrypt(serverData.base);
    serverData.mod = rsa.decrypt(serverData.mod);
    serverData.serverResult = rsa.decrypt(serverData.serverResult);

    // Pick Client secret number
    srand(time(0) * 2);
    int clientSecret = rand() % 50;

    // Raise Client secret number to base and mod
    int clientResult = FastModExpon(serverData.base, clientSecret, serverData.mod);
    clientResult = rsa.encrypt(clientResult, serverRSAKey);

    // Send Client result to server
    if(send(this->socket , &clientResult, sizeof(clientResult) , 0) < 0)
	{
		cout << "Unable to send client data to server" << endl;
		return;
	}

    // Raise Server result to Client secret number
    this->privateKey = FastModExpon(serverData.serverResult, clientSecret, serverData.mod);
}

int DiffieHellmanRSA::getPrivateKey(){
    return this->privateKey;
}