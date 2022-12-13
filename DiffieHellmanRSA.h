#ifndef _DIFFIE_HELLMAN_H
#define _DIFFIE_HELLMAN_H

// struct used to send server data to client
struct DiffieHellmanServerData{
    int base;
    int mod;
    int serverResult;
};

class DiffieHellmanRSA{
    private:
        int socket;
        int privateKey;
    public:
        DiffieHellman(int socket);
        void serverGetPrivateKey();
        void clientGetPrivateKey();
        int getPrivateKey();

};

#endif