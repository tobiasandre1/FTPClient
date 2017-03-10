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
	/*
	ftp servers:
	ftp://ftp.gnupg.dk
	"ftp.gnupg.dk"
	"82.180.28.130"
	"130.226.195.126"
	*/


	//Startup for winsock
	//The following lines of code are necessary for sockets to work on windows
	WORD wVersionRequested = 0x0101;
	WSADATA wsaData;
	int err;

	err = WSAStartup(wVersionRequested, &wsaData); 
	errorCheckWSAStartup(err); //Check if WSAStartup worked correctly


	int sd, sd2, sd3, rc, rc2, rc3, port; //Variable declaration
	struct sockaddr_in servaddr, passaddr, passaddr2; //Variable declaration, but these are socket adress structs
	char buffer[4096];	//buffer for receiving data (is used to send with sendUserInput, but our program is not meant to accept user inputs, so that won't matter)
	char bufferData[4096], bufferData2[4096];

	//Create a socket for the client
	//If sd<0 there was an error in the creation of the socket
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) <0) //socket(address family, data stream) and an error check 
	{
		perror("Problem in creating the socket"); //exit(2);
	}
	
	port = SERV_PORT; //Set port of connection to be 21
	servaddr = createSocket(port, "204.76.241.31"); //Initialize servaddr struct (the address the socket will communicate with 

	//connect server method
	rc = connect(sd, (struct sockaddr *) &servaddr, sizeof(servaddr)); //connects with socket to the ftp server.
	errorCheckConnect(rc); //Moved the if statement that error checks the connection to a function
	
	
	receive(buffer, sizeof(buffer), sd); //Receive response from server
	login(buffer, sizeof(buffer), sd); //Sequence of sendString and receive calls that logs in as USER anonymous with the PASS s@dtu.dk
	
	port = openPassivePort(buffer, sizeof(buffer), sd); //Get the passive port number with string manipulation of answer from PASV
	
	//Create new socket and connect again
	if ((sd2 = socket(AF_INET, SOCK_STREAM, 0)) <0)
	{
		perror("Problem in creating the socket"); //exit(2);
	}
	passaddr = createSocket(port, "204.76.241.31"); //Initialize socket
	rc2 = connect(sd2, (struct sockaddr *) &passaddr, sizeof(passaddr));
	errorCheckConnect(rc2);

	/* Lines of code relevant for data transfer
	sendString("RETR public\r\n", strlen("RETR public\r\n"), sd);
	receive(buffer, sizeof(buffer), sd); //Recieve data from socket
	receive(bufferData, sizeof(bufferData), sd2); //Get file data from socket
	*/

	//sendString("RETR README.txt\r\n", strlen("RETR README.txt\r\n"), sd);
	sendString("RETR /pub/tbw/OROP_data_20170302.txt\r\n", strlen("RETR /pub/tbw/OROP_data_20170302.txt\r\n"), sd);
	receive(buffer, sizeof(buffer), sd); //Recieve data from socket
	system("pause");
	receive(bufferData, sizeof(bufferData), sd2); //Recieve file data from socket
	sendString("NOOP\r\n", strlen("NOOP\r\n"), sd);
	receive(buffer, sizeof(buffer), sd); //Recieve data from socket
	receive(buffer, sizeof(buffer), sd); //Recieve data from socket
	
	system("pause");

	sendString("APPE WHAT\r\n", strlen("APPE WHAT\r\n"), sd); //TRYING TO UPLOAD FILE WITH APPEND (THIS WOULD BE A .txt FILE CONTAINING THE WORD WHAT)
	receive(buffer, sizeof(buffer), sd); //Receive response from server //PERMISSION DENIED//

	port = openPassivePort(buffer, sizeof(buffer), sd);
	//Create new socket and connect again
	if ((sd3 = socket(AF_INET, SOCK_STREAM, 0)) <0)
	{
		perror("Problem in creating the socket"); //exit(2);
	}
	passaddr2 = createSocket(port, "204.76.241.31"); //Initialize socket
	rc3 = connect(sd3, (struct sockaddr *) &passaddr2, sizeof(passaddr2));
	errorCheckConnect(rc3);
	
	sendString("RETR README.txt\r\n", strlen("RETR README.txt\r\n"), sd3);
	receive(buffer, sizeof(buffer), sd); //Recieve data from socket
	system("pause");
	receive(bufferData2, sizeof(bufferData2), sd3); //Recieve file data from socket
	//sendString("NOOP\r\n", strlen("NOOP\r\n"), sd);
	//receive(buffer, sizeof(buffer), sd); //Recieve data from socket
	//receive(buffer, sizeof(buffer), sd); //Recieve data from socket

	/*
	sendString("CWD /pub/outgoing\r\n", strlen("CWD /pub/outgoing\r\n"), sd);
	receive(buffer, sizeof(buffer), sd); //Recieve data from socket
	receive(buffer, sizeof(buffer), sd); //Recieve data from socket
	*/


	
	while (1){
		sendUserInput(buffer, sd); //Let user type in the command line and send the data to the socket
		receive(buffer, sizeof(buffer), sd); //Recieve data from socket
		receive(bufferData, sizeof(bufferData), sd2);


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
	while (buffer[0] == '2' && buffer[1] == '2' && buffer[2] == '0'){
		receive(buffer, bufferSize, sd);
	}
	sendString("PASS -s@dtu.dk\r\n", strlen("PASS -s@dtu.dk\r\n"), sd);
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
