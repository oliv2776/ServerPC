// ServerPC.cpp : définit le point d'entrée pour l'application console.
//


/*Includes*/
#include "stdafx.h"
/*To avoid _WINSOCK_DEPRECATED_NO_WARNINGS error while compiling*/
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")
/*defines*/
#define PORT 23


typedef int socklen_t;
using namespace std;

int main()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	int error = 0;
	/*Sockets server*/
	SOCKET sock;
	SOCKADDR_IN sin;
	socklen_t recsize = sizeof(sin);

	/*Sockets client*/
	SOCKADDR_IN csin;
	SOCKET csock;
	socklen_t crecsize = sizeof(csin);

	int sock_err;

	if (!error) {
		/*Create the socket*/
		sock = socket(AF_INET, SOCK_STREAM, 0);

		/*If sockets ok*/
		if (sock != INVALID_SOCKET) {
			printf("The socket %d is now opened in mode TCP/IP\n", sock);

			/*Configuration*/
			sin.sin_addr.s_addr = htonl(INADDR_ANY);  /* Automatique IP adress */
			sin.sin_family = AF_INET;                 /* Family protocol (IP) */
			sin.sin_port = htons(PORT);
			sock_err = bind(sock, (SOCKADDR*)&sin, recsize);

			/*If the sockets works*/
			if (sock_err != SOCKET_ERROR) {
				/*Start listening (Server Mode)*/
				sock_err = listen(sock, 5);
				printf("listening port %d\n", PORT);

				/*If the socket wors*/
				if (sock_err != SOCKET_ERROR) {
					/*Waiting while client connect*/
					printf("Wait while the client is connecting on port %d\n", PORT);
					csock = accept(sock, (SOCKADDR*)&csin, &crecsize);
					printf("The client connect with the socket %d of %s:%d\n", csock, inet_ntoa(csin.sin_addr), htons(csin.sin_port));

				}
				else
					perror("listen");
			}
			else
				perror("bind");
			/*Closing client socket and server socket*/
			printf("Closing client socket\n");
			closesocket(csock);
			printf("Closing server socket\n");
			closesocket(sock);
			printf("Closing server socket done\n");
		}
		else
			perror("socket");
		WSACleanup();
	}

	
	return EXIT_SUCCESS;
}