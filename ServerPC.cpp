/*!
*\file ServerPC.cpp
*\brief The app server
*\author Olivier Deschênes
*\version 0.1
*/

#if defined (WIN32)
typedef int socklen_t;
#include "stdafx.h"
/*To avoid _WINSOCK_DEPRECATED_NO_WARNINGS error while compiling*/
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <iostream>
#include <fstream>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

#elif defined (linux)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
#endif



#include <stdio.h>
#include <stdlib.h>
#include <string>

#define PORT 23



int main()
{
	
#if defined (WIN32)
	WSADATA WSAData;
	int erreur = WSAStartup(MAKEWORD(2, 2), &WSAData);
#else
	int erreur = 0;
#endif

	int error = 0;
	char buffer[32] = "Hello world!\n";
	int menuChoice = 0;
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

				/*If the socket works*/
				if (sock_err != SOCKET_ERROR) {
					/*Waiting while client connect*/
					printf("Wait while the client is connecting on port %d\n", PORT);
					csock = accept(sock, (SOCKADDR*)&csin, &crecsize);
					printf("The client connect with the socket %d of %s:%d\n", csock, inet_ntoa(csin.sin_addr), htons(csin.sin_port));
					do 
					{
						switch (menuChoice)
						{
						case 0:
							printf("1: send hello world\n2: send a 1\n3: close connexion\n");
							cin >> menuChoice;
							break;
						case 1:
							/*Sending a string*/
							
								sock_err = send(csock, buffer, 32, 0);


								if (sock_err != SOCKET_ERROR)
								{
									printf("String sent : %s\n", buffer);
								}
								else {
									printf("Transmition error\n");
								
								
							}
							
							menuChoice = 0;
							break;
						case 2:
							sock_err = send(csock, "1", 32, 0);
							if (sock_err != SOCKET_ERROR)
							{
								printf("1 sent\n");
							}
							menuChoice = 0;
							break;
						default:
							menuChoice = 0;
							break;
						}
					} while (menuChoice != 3);
					/* shooting down the connexion*/
					shutdown(csock, 2);
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

		#if defined (WIN32)
			WSACleanup();
		#endif

	}
	getchar();

	
	return EXIT_SUCCESS;
}