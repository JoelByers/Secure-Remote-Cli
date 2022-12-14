echo Building Server...
g++ Server.cpp DiffieHellmanRSA.cpp fastmodexpon.cpp RSA.cpp SDES.cpp Cert487.cpp CertGroup.cpp CRL.cpp -o server.out
echo Building Client...
g++ Client.cpp DiffieHellmanRSA.cpp fastmodexpon.cpp RSA.cpp SDES.cpp  Cli.cpp Cert487.cpp CertGroup.cpp CRL.cpp -o client.out
echo Building Cert Generator...
g++ CertGenerator.cpp Cert487.cpp SDES.cpp -o certGen.out
echo Done
