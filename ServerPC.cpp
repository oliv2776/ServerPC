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

#define ADC1 1
#define ADC2 2
#define ADC3 3
#define ADC1_2 4
#define ADC1_3 5
#define ADC2_3 6
#define ADC1_2_3 7
#define PORT 23
#define BUFFERSIZE 32
#define SIZEDATA 1024
#define SIZEFRAME 2*SIZEDATA+24

/*Frame sent to the client: Board name, ADC number, Packet number... char[]*/
typedef union frame_u {
	struct frame_s {
		uint8_t board;
		uint8_t adc_number;
		uint32_t packet_number;
		uint32_t total_of_packet;
		uint8_t day;
		uint8_t month;
		uint16_t year;
		uint8_t hour;
		uint8_t minutes;
		uint8_t seconds;
		uint8_t miliseconds;
		uint32_t data_lenght;
		uint16_t data[SIZEDATA];
	}frame_as_field;
	char frame_as_byte[SIZEFRAME];
}frame_u;

/*Format the frame with the the informations in the structure frame but without the data*/
frame_u formatbuffer(uint8_t boardNumber, uint8_t adcnumber, uint32_t nbPacket, uint32_t numberTotalOfPackets);

int send_command(int menu_choice, int socket_error,SOCKET csocket);

int main()
{
	
#if defined (WIN32)
	WSADATA WSAData;
	int erreur = WSAStartup(MAKEWORD(2, 2), &WSAData);
#else
	int erreur = 0;
#endif

	int error = 0;

	SYSTEMTIME t;
	GetSystemTime(&t);


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
							printf("1: Start ADC 1\n2: Start ADC 2\n3: Start ADC 3\n4: Start ADC 1&2\n5: Start ADC 1&3\n6: Start ADC 2&3\n7: Start ADC 1,2&3\n9: Close connexion\n");
							cin >> menuChoice;
							break;

						case ADC1:
							menuChoice = send_command(menuChoice,sock_err,csock);
							break;

						case ADC2:
							menuChoice = send_command(menuChoice, sock_err, csock);
						break;

						case ADC3:
							menuChoice = send_command(menuChoice, sock_err, csock);
							break;

						case ADC1_2:
							menuChoice = send_command(menuChoice, sock_err, csock);
							break;

						case ADC1_3:
							menuChoice = send_command(menuChoice, sock_err, csock);
							break;

						case ADC2_3:
							menuChoice = send_command(menuChoice, sock_err, csock);
							break;

						case ADC1_2_3:
							menuChoice = send_command(menuChoice, sock_err, csock);
							break;

						case 9:
							menuChoice = send_command(menuChoice, sock_err, csock);
							break;
						default:
							menuChoice = 0;
							break;
						}
						
					} while (menuChoice != 9);
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

		/*Waiting for the user to type something to close*/
		printf("\nType any key to close, see you soon!\n");
		getchar();
		#if defined (WIN32)
			WSACleanup();
		#endif

	}


	
	return EXIT_SUCCESS;
}


frame_u formatbuffer( uint8_t boardNumber, uint8_t adcnumber, uint32_t nbPacket, uint32_t numberTotalOfPackets) {
	SYSTEMTIME t;
	GetSystemTime(&t);

	frame_u frame;

	frame.frame_as_field.board = boardNumber;
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
	frame.frame_as_field.data_lenght = SIZEDATA;

	return frame;
}


int send_command(int menu_choice, int socket_error, SOCKET csocket) {
	uint32_t total_of_packet;
	frame_u frame;

	printf("How many packets do you want?\n");
	cin >> total_of_packet;

	//format the frame before being sent 
	frame = formatbuffer(1, menu_choice, 0, total_of_packet);

	socket_error = send(csocket, frame.frame_as_byte, SIZEFRAME, 0);
	if (socket_error = SOCKET_ERROR)
	{
		printf("error while sending informations, error %d\n", socket_error);
	}
	if (menu_choice == 9) {
		return 9;
	}
	else {
		return 0;
	}
	
}