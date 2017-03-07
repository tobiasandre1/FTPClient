#ifdef _WIN32
	#define _WIN32_LEAN_AND_MEAN
	#define _WINSOCK_DEPRECATED_NO_WARNINGS	
	#define _CRT_SECURE_NO_WARNINGS
	#include <WinSock2.h>
	#include <ws2tcpip.h>
	#include <Windows.h>
#else
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <unistd.h> /* close */
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define SERV_PORT 21
#define MAX_MSG 100

int main(int argc, char *argv[])  {

	WORD wVersionRequested = 0x0101;
	WSADATA wsaData;
	int err;

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		printf("WSAStartup failed with error: %d\n", err);
		return 1;
	}

	int sd, rc, n;
	struct sockaddr_in /*localAddr,*/ servaddr;
	//struct hostent *h;
	char buffer[4096];
	char buffer1[4096]; //buffersend

	//Create a socket for the client
	//If sd<0 there was an error in the creation of the socket
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) <0)
	{
		perror("Problem in creating the socket"); //exit(2);
	}

	//Creation of the socket
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("130.226.195.126"); // her skulle der stå argv[1] ved ipadressen dog erstattet vi den med ip addressen til den server vi skal kommunikere med
	// servaddr.sin_port = htons(portno); //convert to big-endian order
	servaddr.sin_port = htons(SERV_PORT); //convert to big-endian order

	//connect server method
	rc = connect(sd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	if (rc < 0){
		perror("cannot connect");
		exit(1);
	}

	//code for receivind statement ""
	while (1){
		n = recv(sd, buffer, sizeof(buffer), 0);
		buffer[n] = '\0';
		printf("%s", buffer);

		if (fgets(buffer1, 60, stdin) != NULL) {
			/* writing content to stdout */
			puts(buffer1);
		}
		strcat(buffer1, "\r\n");
		send(sd, buffer1, strlen(buffer1), 0);
	}

}