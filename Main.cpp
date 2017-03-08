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

int errorCheckWSAStartup(int check);						//Make sure WSAStartup worked correctly
struct sockaddr_in createSocket(int port, char ip[]);		//Creates a new socket
void errorCheckConnect(int check);							//Make sure connect worked correctly
void sendUserInput(char buffer[], int sd);					//Let user type in command line and send data to socket
void receive(char buffer[], int bufferSize, int sd);		//Receive data from socket

#define SERV_PORT 21
#define MAX_MSG 100

int main(int argc, char *argv[])  {

	//Startup for winsock
	WORD wVersionRequested = 0x0101;
	WSADATA wsaData;
	int err;

	err = WSAStartup(wVersionRequested, &wsaData); 
	errorCheckWSAStartup(err); //Check if WSAStartup worked correctly


	int sd, rc, port = SERV_PORT;
	struct sockaddr_in servaddr;
	char buffer[4096];	//buffer for sending and receiving data

	//Create a socket for the client
	//If sd<0 there was an error in the creation of the socket
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) <0)
	{
		perror("Problem in creating the socket"); //exit(2);
	}
	
	servaddr = createSocket(port, "130.226.195.126"); //Initialize socket

	//connect server method
	rc = connect(sd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	errorCheckConnect(rc);

	while (1){
		receive(buffer, sizeof(buffer), sd); //Recieve data from socket
		sendUserInput(buffer, sd); //Let user type in the command line and send the data to the socket

	}

}

int errorCheckWSAStartup(int check){
	if (check != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		printf("WSAStartup failed with error: %d\n", check);
		return 1;
	}
	return 0;
}

struct sockaddr_in createSocket(int port, char ip[]){
	struct sockaddr_in servaddr;
	//Creation of the socket
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ip); // her skulle der stå argv[1] ved ipadressen dog erstattet vi den med ip addressen til den server vi skal kommunikere med
	// servaddr.sin_port = htons(portno); //convert to big-endian order
	servaddr.sin_port = htons(port); //convert to big-endian order
	return servaddr;
}

void errorCheckConnect(int check){
	if (check < 0){
		perror("cannot connect");
		exit(1);
	}
}

void sendUserInput(char buffer[], int sd){
	//sending code
	if (fgets(buffer, 60, stdin) != NULL) {
		/* writing content to stdout */
		//puts(buffer1);
	}
	strcat(buffer, "\r\n");
	send(sd, buffer, strlen(buffer), 0);
}


void receive(char buffer[], int bufferSize, int sd){
	//code for receivind statement ""
	int n;
	n = recv(sd, buffer, bufferSize, 0);
	buffer[n] = '\0';
	printf("%s", buffer);
}
