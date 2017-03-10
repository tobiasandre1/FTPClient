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
#include <string>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

void errorCheckWSAStartup(int check);						//Make sure WSAStartup worked correctly
struct sockaddr_in createSocket(int port, char ip[]);		//Creates a new socket
void errorCheckConnect(int check);							//Make sure connect worked correctly

//Functions for FTP server operations
void sendUserInput(char buffer[], int sd);					//Let user type in command line and send data to socket
void sendString(char buffer[], int bufferSize, int sd);		//Sends a string to the FTP server
void receive(char buffer[], int bufferSize, int sd);		//Receive data from socket
void login(char buffer[], int bufferSize, int sd);
int openPassivePort(char buffer[], int bufferSize, int sd);

#define SERV_PORT 21
#define MAX_MSG 100

int main(int argc, char *argv[])  {

	//Startup for winsock
	WORD wVersionRequested = 0x0101;
	WSADATA wsaData;
	int err;

	err = WSAStartup(wVersionRequested, &wsaData); 
	errorCheckWSAStartup(err); //Check if WSAStartup worked correctly


	int sd, sd2, rc, rc2, port;
	struct sockaddr_in servaddr, passaddr;
	char buffer[4096];	//buffer for sending and receiving data

	//Create a socket for the client
	//If sd<0 there was an error in the creation of the socket
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) <0)
	{
		perror("Problem in creating the socket"); //exit(2);
	}
	
	port = SERV_PORT;
	servaddr = createSocket(port, argv[0] != NULL ? "130.226.195.126" : argv[0]); //Initialize socket

	//connect server method
	rc = connect(sd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	errorCheckConnect(rc);

	receive(buffer, sizeof(buffer), sd);
	login(buffer, sizeof(buffer), sd);

	port = openPassivePort(buffer, sizeof(buffer), sd);

	//Create new socket and connect again
	if ((sd2 = socket(AF_INET, SOCK_STREAM, 0)) <0)
	{
		perror("Problem in creating the socket"); //exit(2);
	}
	passaddr = createSocket(port, argv[0] != NULL ? "130.226.195.126" : argv[0]); //Initialize socket
	rc2 = connect(sd2, (struct sockaddr *) &passaddr, sizeof(passaddr));
	errorCheckConnect(rc2);

	//receive(buffer, sizeof(buffer), sd); //Recieve data from socket


	while (1){
		sendUserInput(buffer, sd); //Let user type in the command line and send the data to the socket
		receive(buffer, sizeof(buffer), sd); //Recieve data from socket

	}

}

void errorCheckWSAStartup(int check){
	if (check != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		printf("WSAStartup failed with error: %d\n", check);
		exit(1);
	}
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
	else{
		printf("%s\n", "Connected.");
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

void sendString(char buffer[], int bufferSize, int sd){
	//strcat(buffer, "\r\n");
	printf("%s", buffer);
	send(sd, buffer, bufferSize, 0);
}


void receive(char buffer[], int bufferSize, int sd){
	//code for receivind statement ""
	int n;
	n = recv(sd, buffer, bufferSize, 0);
	buffer[n] = '\0';
	printf("%s", buffer);
}

void login(char buffer[], int bufferSize, int sd){
	sendString("USER anonymous\r\n", strlen("USER anonymous\r\n"), sd);
	receive(buffer, bufferSize, sd);
	sendString("PASS s@dtu.dk\r\n", strlen("PASS s@dtu.dk\r\n"), sd);
	receive(buffer, bufferSize, sd);
}

int openPassivePort(char buffer[], int bufferSize, int sd){
	sendString("PASV\r\n", strlen("PASV\r\n"), sd);
	//code for receivind statement ""
	int n;
	n = recv(sd, buffer, bufferSize, 0);
	buffer[n] = '\0';
	//We have the buffer as a char array and can read from it

	//We create new two arrays and two integers to store our numbers
	char add_number[3] = { '0', '0', '0' };
	char multiply_number[3] = { '0','0','0' };
	int an;
	int mn;

	int j = 2; //Top place in number char arrays
	int i = n - 5; //Buffer always follows the same pattern, so we start from here. Determined by trail/error
	int target = i - 3; //Target is 3 smaller than i so that we can read max 3 numbers
	for (i; i>target; i--){
		if (isdigit(buffer[i])){ //If what we're reading is a number...
			add_number[j] = buffer[i]; //add it to the char array
		}
		else{
			break; //i (variable) will not have reached target and we stop this for loop
		}
		j--; //j counts down along with i
	}
	an = atoi(add_number); //Integer atoi is equal to value of string inside the corresponding char array

	j = 2;
	i--; //Skip the comma between the numbers
	target = i - 3; //New target is 3 smaller than i. Length of array.
	for (i; i>target; i--){ //Same for loop as before.
		if (isdigit(buffer[i])){
			multiply_number[j] = buffer[i];
		}
		else{
			break;
		}
		j--;
	}
	mn = atoi(multiply_number); //Integer mn is equal to value of its corresponding char array

	//Print response
	printf("%s", buffer);
	printf("%s", "Passive port... ");
	printf("%d * 256 + ", mn);
	printf("%d = ", an);
	printf("%d\n", mn * 256 + an);

	return mn * 256 + an; //Return port calculation result
}
