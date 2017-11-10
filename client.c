/*************************************************
Sockets

File: client.c
Usage: gcc -o client client.c
		client <host> <port>

Description: Connects at <host><port> to send a message to server.
			 Python modified from my IRC chatbot.
			 C modified from my CS344 encryption/decryption program.
**************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define FALSE 0
#define TRUE 1
//TRUE to print test messages
#define TEST FALSE
//500 character message
#define MAX_CHAR 500
//10 character username
#define MAX_HANDLE 10
#define EXIT_MSG "\\quit"

//set up client variables
struct client {
	//socket info
	struct sockaddr_in serverAddress;
	//host info
	struct hostent* serverHostInfo;
	//port
	int serverPort;
	//0/1 socket connection
	int socketFD;
	//10 char + terminator
	char handle[MAX_HANDLE+1];
	//500 char + terminator
	char buffer[MAX_CHAR+1];
	//message = 10 char + ": " + 500 char + terminator
	char msg[MAX_HANDLE + 2 + MAX_CHAR + 1];
};

void connect_client(struct client *c, int argc, char *argv[]);
void get_name(struct client *c);
void send_message(struct client *c);
void get_message(struct client *c);

/*************
Initiate Contact
@c: client
@argc: number of arguments
@argv: array of arguments
**************/
void connect_client(struct client *c, int argc, char *argv[]) {
	memset((char*)&c->serverAddress, '\0', sizeof(c->serverAddress));
	//get port
	c->serverPort = atoi(argv[2]);
	//set up socket family
	c->serverAddress.sin_family = AF_INET;
	//store the port in socket struct
	c->serverAddress.sin_port = htons(c->serverPort);
	//set up host
	c->serverHostInfo = gethostbyname(argv[1]);
	
	if (TEST == 1)
	{
		if (c->serverHostInfo == NULL) {
			printf("-----CLIENT: ERROR, no such host-----\n");
		}
	}
	
	//copy host address to socket struct
	memcpy((char*)&c->serverAddress.sin_addr.s_addr, (char*)c->serverHostInfo->h_addr, c->serverHostInfo->h_length);
	
	//set up TCP
	c->socketFD = socket(AF_INET, SOCK_STREAM, 0);
	
	if (TEST == 1)
	{
		if (c->socketFD < 0) {
			printf("-----CLIENT: ERROR opening socket-----\n");
		}
	}
	
	//try to connect to the port
	int connect_value = connect(c->socketFD, (struct sockaddr*)&c->serverAddress, sizeof(c->serverAddress));
	
	if (connect_value < 0) {
		printf("-----CLIENT: ERROR port %d denied-----\n", c->serverPort);
	} else {
		printf("-----CLIENT: Connected to port %d-----\n", c->serverPort);
	}	
}

/*************
Get username
@c: client
**************/
void get_name(struct client *c)
{
	printf("Please enter a username of 10 characters or less: ");
	
	//handles buffer overflow in portable C https://stackoverflow.com/a/38768287
	if(fgets(c->handle,MAX_HANDLE,stdin)){
            char *p;
            if(p=strchr(c->handle, '\n')){//check exist newline
                *p = 0;
            } else {
                scanf("%*[^\n]");scanf("%*c");//clear upto newline
            }
		}
		
	strtok(c->handle, "\n");
	printf("Your name will be %s for this session.\n", c->handle);
}

/*************
Send message
@c: client
**************/
void send_message(struct client *c)
{
		printf("%s: ", c->handle);
		
		memset((char*)&c->msg, '\0', sizeof(c->msg));
		memset((char*)&c->buffer, '\0', sizeof(c->buffer));
		
		if(fgets(c->buffer,MAX_CHAR,stdin)){
            char *p;
            if(p=strchr(c->buffer, '\n')){//check exist newline
                *p = 0;
            } else {
                scanf("%*[^\n]");scanf("%*c");//clear upto newline
            }
		}
	
		//set up "Name: message" string to send to server
		strtok(c->buffer, "\n");		
		strcat(c->msg, c->handle);
		strcat(c->msg, ": ");
		strcat(c->msg, c->buffer);
		//write c->msg to socket
		write(c->socketFD, c->msg, strlen(c->msg));
	
		//if our message was \quit, exit the client
		if (strncmp(c->buffer, EXIT_MSG, 5) == 0)
		{
			close(c->socketFD);
			exit(0);
		}
		
		memset((char*)&c->msg, '\0', sizeof(c->msg));
		memset((char*)&c->buffer, '\0', sizeof(c->buffer));
		
		//or else get ready to receive a message
		get_message(c);
}

/*************
Receive message
@c: client
**************/
void get_message(struct client *c)
{
		//quit message comes from server as "Server: \quit"
		char quit_msg[13];
		memset((char*)&quit_msg, '\0', sizeof(quit_msg));
		strcat(quit_msg, "Server: ");
		strcat(quit_msg, EXIT_MSG);
	
		memset((char*)&c->buffer, '\0', sizeof(c->buffer));
		//read message from server
		read(c->socketFD, c->buffer, MAX_CHAR);
		printf("%s\n", c->buffer);
		
		//if the server tells us to quit, exit the client
		if (strncmp(c->buffer, quit_msg, 13) == 0)
		{
			close(c->socketFD);
			memset((char*)&c->buffer, '\0', sizeof(c->buffer));
			exit(0);
		}
		
		//else get ready to send a message to the server
		else
		{
			send_message(c);
			memset((char*)&c->buffer, '\0', sizeof(c->buffer));
		}
}

void main(int argc, char *argv[]) {
	struct client chat_client;
	
	connect_client(&chat_client, argc, argv);
	get_name(&chat_client);
	send_message(&chat_client);
	get_message(&chat_client);
}
