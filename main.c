#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "lb_functions.h"

int main(int argc, char* argv[])
{	
	time_t t; 
	srand((unsigned) time (&t));
	int server_port_number = WriteServerPortFile();
	int client_port_number = WriteHttpPortFile();

	int server_socket;
	int server_connected_socket[NUM_OF_CONNECTING_SERVERS-1];

	ConnectToServers(&server_socket, server_connected_socket,server_port_number);
	while(1)
	{
		ProccesMessages(server_connected_socket,client_port_number);
	}
	
}

