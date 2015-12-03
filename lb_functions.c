#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "lb_functions.h"


int RepeatNumberOfHTMLTokens (char *buff);

int GenereatePortNumber()
{
	int random_area = 64000-1024;
	int random_number = rand() %random_area;
	return random_number + 1024;
}
int WriteServerPortFile()
{
	FILE *file;
	file=fopen("server_port","w+");
	int port_nubmber = GenereatePortNumber();
	fprintf(file,"%d",port_nubmber);
	fclose(file);
	return port_nubmber;
}
int WriteHttpPortFile()
{
	FILE *file;
	file=fopen("http_port","w+");
	int port_nubmber = GenereatePortNumber();	
	fprintf(file,"%d",port_nubmber);
	fclose(file);
	return port_nubmber;
}
void ConnectToServers(int *server_socket,int server_connected_socket[],int server_port_number)
{
	int i;
	struct sockaddr_in server;
	struct sockaddr_in server_dest[NUM_OF_CONNECTING_SERVERS-1];
	socklen_t socksize = sizeof(struct sockaddr_in);
	//configure server
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=INADDR_ANY;//inet_addr("127.0.0.1");
	server.sin_port=htons(server_port_number);
	*server_socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	//bind server
	bind(*server_socket, (struct sockaddr*) &server,sizeof(server));
	for (i=0;i<NUM_OF_CONNECTING_SERVERS;i++)
	{
		listen(*server_socket,1);
		server_connected_socket[i] = accept(*server_socket, (struct sockaddr*) &server_dest[i], &socksize);
	}
}

void ConnectToClient(int *client_socket,int *client_connected_socket,int client_port_number)
{
	socklen_t socksize = sizeof(struct sockaddr_in);
	*client_connected_socket = accept(*client_socket, NULL, &socksize);
}
void ConfigureClientSocket(int *client_socket,int client_port_number)
{
	struct sockaddr_in client;
	struct sockaddr_in client_dest;
	socklen_t socksize = sizeof(struct sockaddr_in);
	//configure client
	client.sin_family=AF_INET;
	client.sin_addr.s_addr=INADDR_ANY;
	client.sin_port=htons(client_port_number);
	*client_socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	//bind client
	bind(*client_socket, (struct sockaddr*) &client,sizeof(client));
	listen(*client_socket,1);

}
void ReceiveMessageFromClient(int server_connected_socket,int client_connected_socket);
void ReceiveMessageFromServer(int server_connected_socket,int client_connected_socket);

void ProccesMessages(int server_connected_socket[],int client_port_number)
{
	int client_socket;
	int client_connected_socket;
	int server_to_be_use = 0;
	ConfigureClientSocket(&client_socket,client_port_number);
	while(1)
	{
		ConnectToClient(&client_socket, &client_connected_socket,client_port_number);
		ReceiveMessageFromClient(server_connected_socket[server_to_be_use],client_connected_socket);
		ReceiveMessageFromServer(server_connected_socket[server_to_be_use],client_connected_socket);
		if(server_to_be_use == NUM_OF_CONNECTING_SERVERS - 1)
			server_to_be_use = 0;
		else
			server_to_be_use++;
		close(client_connected_socket);
	}
}
void ReceiveMessageFromClient(int server_connected_socket,int client_connected_socket)
{
	int client_bytes_recv;
	char* client_buff = (char*) malloc(sizeof(char*)*MAX_BUFFER_SIZE);
	do
	{
		client_bytes_recv = recv(client_connected_socket,client_buff,MAX_BUFFER_SIZE,0);
		send(server_connected_socket,client_buff,strlen(client_buff),0);
	}
	while(strstr(client_buff,"\r\n\r\n") == NULL);
}
void ReceiveMessageFromServer(int server_connected_socket,int client_connected_socket)
{
	int server_bytes_recv;
	char* server_buff = (char*) malloc(sizeof(char*)*MAX_BUFFER_SIZE);
	int break_statment;
	int token_reptition =0;
	do
	{
		server_bytes_recv = recv(server_connected_socket,server_buff,MAX_BUFFER_SIZE,0);
		break_statment = RepeatNumberOfHTMLTokens(server_buff);
		send(client_connected_socket,server_buff,strlen(server_buff),0);
		token_reptition += RepeatNumberOfHTMLTokens(server_buff);
		if (token_reptition == 2 )
			break_statment = 0;
		else
			break_statment = 1;
	}
	while(break_statment);
}
//token is "\r\n\r\n"
int RepeatNumberOfHTMLTokens (char *buff)
{
	char *temp_buff = (char*) malloc(sizeof(char*)*strlen(buff));
	strcpy(temp_buff,buff);
	char *token = "\r\n\r\n";
	char *ptr1,*ptr2;
	ptr1 = strstr (temp_buff,token);
	if (ptr1 == NULL)
		return 0;
	ptr2 = strstr (ptr1 + strlen(token),token);
	if (ptr2 == NULL )
		return 1;
	else
		return 2;
	}
