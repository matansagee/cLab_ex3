#ifndef __SOCKET_HANDLER_H
#define __SOCKET_HANDLER_H
#define NUM_OF_CONNECTING_SERVERS 3
#define MAX_BUFFER_SIZE 1024

int WriteServerPortFile();
int WriteHttpPortFile();

void ConnectToServers(int *server_socket,int server_connected_socket[],int server_port_number);
void ProccesMessages(int server_connected_socket[],int client_port_number);

#endif//__SOCKET_HANDLER_H

