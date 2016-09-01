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
#define _CRT_SECURE_NO_WARNINGS
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
#define BUFFERSIZE 32
#define SIZEDATA 1024
#define SIZEFRAME 2*SIZEDATA+21


union frame_u {
	struct frame_s {
		uint8_t board;
		uint8_t adc_number;
		uint32_t packet_number;
		uint32_t total_of_packet;
		uint8_t day;
		uint8_t month;
		uint8_t year;
		uint8_t hour;
		uint8_t minutes;
		uint8_t seconds;
		uint8_t miliseconds;
		uint32_t data_lenght;
		uint16_t data[SIZEDATA];
	}frame_as_field;
	char frame_as_byte[SIZEFRAME];
};

void formatbuffer(union frame_u frame, uint8_t adcnumber, uint32_t nbPacket, uint32_t numberTotalOfPackets);

int main()
{
	
#if defined (WIN32)
	WSADATA WSAData;
	int erreur = WSAStartup(MAKEWORD(2, 2), &WSAData);
#else
	int erreur = 0;
#endif

	union frame_u frame1;
	int error = 0;
	char buffer[BUFFERSIZE];
	for (int i = 0; i < BUFFERSIZE; i++) {
		buffer[i] = '0';
	}
	char temp[6];
	int menuChoice = 0;
	uint32_t timeConversion;

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
							printf("1: Start ADC 1\n2: Start ADC 2\n3: Start ADC 3\n4: close connexion\n");
							cin >> menuChoice;
							break;
						case 1:
							printf("How long do you want to convert the signal?\n");
							cin >> timeConversion;
							for (int i = 2; i < 30; i++) {
								printf("%d \t", frame1.frame_as_byte[i]);
							}
							frame1.frame_as_field.adc_number = 1;
							frame1.frame_as_field.total_of_packet = timeConversion;
							formatbuffer(frame1, 1, 0, timeConversion);
							
							
							printf("\n");
							sock_err = send(csock, frame1.frame_as_byte, SIZEFRAME, 0);
							if (sock_err = SOCKET_ERROR)
							{
								printf("error while sending informations, error %d\n",sock_err);
							}

							menuChoice = 0;
							break;
						case 2:
							printf("How long do you want to convert the signal?\n");
							cin >> timeConversion;
							
							sock_err = send(csock, buffer, BUFFERSIZE, 0);
							if (sock_err = SOCKET_ERROR)
							{
								printf("error while sending informations\n");
							}

							menuChoice = 0;
							break;
						case 3:
							printf("How long do you want to convert the signal?\n");
							cin >> timeConversion;
							
							if (sock_err = SOCKET_ERROR)
							{
								printf("error while sending informations\n");
							}

							menuChoice = 0;
							break;
						default:
							printf("Wrong choice\n");
							menuChoice = 0;
							break;
						}

					} while (menuChoice != 4);
					/* shooting down the connexion*/
					buffer[0] = 'z';
					sock_err = send(csock, buffer, BUFFERSIZE, 0);
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

void formatbuffer(union frame_u frame, uint8_t adcnumber, uint32_t nbPacket, uint32_t numberTotalOfPackets) {
	SYSTEMTIME t;
	GetSystemTime(&t);

	frame.frame_as_field.board = 1;
	frame.frame_as_field.adc_number = adcnumber;
	frame.frame_as_field.packet_number = nbPacket;
	frame.frame_as_field.total_of_packet = numberTotalOfPackets;
	frame.frame_as_field.day = t.wDay;
	frame.frame_as_field.month = t.wMonth;
	frame.frame_as_field.year = t.wYear;
	frame.frame_as_field.hour = t.wHour;
	frame.frame_as_field.minutes = t.wMinute;
	frame.frame_as_field.seconds = t.wSecond;
	frame.frame_as_field.miliseconds = t.wMilliseconds;



}
