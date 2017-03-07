/*
 ============================================================================
 Name        : FTP.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <sys/types.h>
#include <sys/socket.h> //socket header file
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h> /* close */ //universal standard header
#include <stdio.h>  //standard input/output
#include <stdlib.h> //standard library
#include <string.h> //string header

#define SERV_PORT 21
#define MAX_MSG 100

int main(int argc, char *argv[]) {
	int sd, rc, n; //sd er socket, n returner nummre af bytes som er modtaget ellers -1, rc skaber connection fra socket med FTP files discribtor til socket samt returnerer den mængden af bytes som er transmitteret eller -1
	struct sockaddr_in localAddr, servaddr;
	struct hostent *h;
	char buffer[4096];
	char buffer1[4096]; //dette er buffersend bare i et andet navn 4096 er størrelse på bufferen

	//Create a socket for the client
	//If sd<0 there was an error in the creation of the socket
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
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
	if (rc < 0) {
		perror("cannot connect ");
		exit(1);
	}
	while (1) {

		//code for receivind statement ""
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
