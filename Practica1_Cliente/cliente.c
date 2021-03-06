/*******************************************************
Protocolos de Transporte
Grado en Ingenier�a Telem�tica
Dpto. Ingen�er�a de Telecomunicaci�n
Univerisdad de Ja�n

Fichero: cliente.c
Versi�n: 2.0
Fecha: 09/2018
Descripci�n:
	Cliente sencillo TCP para IPv4 e IPv6

Autores:
         Javier S�nchez Samper.
         Daniel Mesa Gonz�lez.

*******************************************************/
#include <stdio.h>
#include <ws2tcpip.h>//Necesaria para las funciones IPv6
#include <conio.h>
#include <stdlib.h>
#include "protocol.h"

#pragma comment(lib, "Ws2_32.lib")

int main(int *argc, char *argv[])
{
	SOCKET sockfd;
	struct sockaddr *server_in = NULL;
	struct sockaddr_in server_in4;
	struct sockaddr_in6 server_in6;
	int address_size = sizeof(server_in4);
	char buffer_in[1024], buffer_out[1024], input[1024];
	int recibidos = 0, enviados = 0;
	int estado = S_HELO;
	char option;
	int ipversion = AF_INET;//IPv4 por defecto
	char ipdest[256];
	char default_ip4[16] = "127.0.0.1";
	char default_ip6[64] = "::1";
	char Charnumero1[20]=" ", Charnumero2[20]=" ";
	int numero1 = 0, numero2 = 0;
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	//Inicializaci�n Windows sockets - SOLO WINDOWS
	wVersionRequested=MAKEWORD(1,1);
err = WSAStartup(wVersionRequested, &wsaData);
if (err != 0)
return(0);

if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {
	WSACleanup();
	return(0);
}
//Fin: Inicializaci�n Windows sockets

printf("**************\r\nCLIENTE TCP SENCILLO SOBRE IPv4 o IPv6\r\n*************\r\n");


do {

	printf("CLIENTE> �Qu� version de IP desea usar? 6 para IPv6, 4 para IPv4 [por defecto] ");
	gets_s(ipdest, sizeof(ipdest));

	if (strcmp(ipdest, "6") == 0) {       //En caso de usar IP6
		ipversion = AF_INET6;

	}
	else { //Distinto de 6 se elige la versi�n 4
		ipversion = AF_INET;
	}

	sockfd = socket(ipversion, SOCK_STREAM, 0);   //SOCKET creamos el socket
	if (sockfd == INVALID_SOCKET) {          //SOCKET y comprobamos el error
		printf("CLIENTE> ERROR\r\n");
		exit(-1);
	}
	else {
		printf("CLIENTE> Introduzca la IP destino (pulsar enter para IP por defecto): ");
		gets_s(ipdest, sizeof(ipdest));

		//Direcci�n por defecto seg�n la familia
		if (strcmp(ipdest, "") == 0 && ipversion == AF_INET)
			strcpy(ipdest, default_ip4);

		if (strcmp(ipdest, "") == 0 && ipversion == AF_INET6)
			strcpy(ipdest, default_ip6);


		//DIFERENCIA IP4 respecto IP6
		if (ipversion == AF_INET) {
			server_in4.sin_family = AF_INET;
			server_in4.sin_port = htons(TCP_SERVICE_PORT);
			//server_in4.sin_addr.s_addr=inet_addr(ipdest);
			inet_pton(ipversion, ipdest, &server_in4.sin_addr.s_addr); //no se usa en ip6
			server_in = (struct sockaddr*)&server_in4; //en ip4 es server_ip4
			address_size = sizeof(server_in4);
		}
		//DIFERENCIA IP6 respecto IP4
		if (ipversion == AF_INET6) {
			memset(&server_in6, 0, sizeof(server_in6));
			server_in6.sin6_family = AF_INET6;  //en ip6 AF_INET6 es especifico para la ip6
			server_in6.sin6_port = htons(TCP_SERVICE_PORT);
			inet_pton(ipversion, ipdest, &server_in6.sin6_addr);
			server_in = (struct sockaddr*)&server_in6; //en ip6 es server_ip6
			address_size = sizeof(server_in6);
		}

		estado = S_HELO;

		if (connect(sockfd, server_in, address_size) == 0) {      //SOCKET crea la conexi�n con el servidor
			printf("CLIENTE> CONEXION ESTABLECIDA CON %s:%d\r\n", ipdest, TCP_SERVICE_PORT);

			//Inicio de la m�quina de estados
			do {
				switch (estado) {
				case S_HELO: //COMANDO DE APLICACI�N (Se encarga de mostrar el mensaje de bienvenida)
					// Se recibe el mensaje de bienvenida
					break;
				case S_USER:  //COMANDO DE APLICACI�n (se encarga de enviar al servidor un nombre de usuario para autentificaci�n)
					// establece la conexion de aplicacion 
					printf("CLIENTE> Introduzca el usuario (enter para salir): ");
					gets_s(input, sizeof(input));
					if (strlen(input) == 0) {
						sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", SD, CRLF);    //COMANDO SD (finaliza la conexi�n)
						estado = S_QUIT;   //COMANDO APLICACI�N (finaliza la conexi�n con el cliente)
					}
					else

						sprintf_s(buffer_out, sizeof(buffer_out), "%s %s%s", SC, input, CRLF);   //COMANDO SC (solicitud de conexi�n)
					break;
				case S_PASS:  //COMANDO DE APLICACI�N (se encarga de enviar al servidor una clave de usuario para autentificaci�n)
					printf("CLIENTE> Introduzca la clave (enter para salir): ");
					gets_s(input, sizeof(input));
					if (strlen(input) == 0) {
						sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", SD, CRLF);
						estado = S_QUIT;
					}
					else
						sprintf_s(buffer_out, sizeof(buffer_out), "%s %s%s", PW, input, CRLF);   //COMANDO PW (solicitud de password del usuario)
					break;

				case S_DATA:   //COMANDO APLICACI�N (se encarga de enviar al servidor todos los datos del cliente)

					    printf("Introduce el primer numero (cuatro cifras): ");
						gets_s(Charnumero1,sizeof(Charnumero1));
						numero1 = atoi(Charnumero1);

						printf("Introduce el segundo numero (cuatro cifras): ");
						gets_s(Charnumero2,sizeof(Charnumero2));
						numero2 = atoi(Charnumero2);
						
		                if((numero1<0 && numero1<10000) && (numero2 <0 && numero2<10000) ) {
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", ERORR, CRLF); //Env�o el error al servidor
							estado = S_QUIT;
		                }
						//----------------------
						else{
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s%i%s%i%s", SUM, SP, numero1, SP, numero2, CRLF);
							printf_s(buffer_out, sizeof(buffer_out), "%s %s%s", ECHO, input, CRLF);
						}
				
					break;
				}
					

					if(estado!=S_HELO){
						enviados=send(sockfd,buffer_out,(int)strlen(buffer_out),0);    //SOCKET env�a los datos al servidor
						if(enviados==SOCKET_ERROR){
							 estado=S_QUIT;
							 continue;
						}
					}
						//Falta algo en el envio de datos.
					recibidos=recv(sockfd,buffer_in,512,0);   //SOCKET recibe los datos del servidor
					if(recibidos<=0){
						DWORD error=GetLastError();
						if(recibidos<0){
							printf("CLIENTE> Error %d en la recepci�n de datos\r\n",error);
							estado=S_QUIT;
						}
						else{
							printf("CLIENTE> Conexion con el servidor cerrada\r\n");
							estado=S_QUIT;
						}
					}else{
						buffer_in[recibidos]=0x00;
						printf(buffer_in);
						if (estado != S_DATA && strncmp(buffer_in, OK, 2) == 0) {
							estado++;
						}
						//------------------------------------------
						else {
							printf("�Desea realizar otra operacion? (Pulsa tecla para seguir o ENTER para salir)");
							gets_s(input, sizeof(input));

								if (strlen(input) == 0) {
						     	sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", SD, CRLF);
										estado= S_QUIT;   
									}

								 else {
										estado = S_DATA;
								    }
						//------------------------------------------
						}
					}

				}while(estado!= S_QUIT);		
			}
			else{
				int error_code=GetLastError();
				printf("CLIENTE> ERROR AL CONECTAR CON %s:%d\r\n",ipdest,TCP_SERVICE_PORT);
			}		
			// fin de la conexion de transporte
			closesocket(sockfd);  //SOCKET cierra la conexi�n del socket
			
		}	
		printf("-----------------------\r\n\r\nCLIENTE> Volver a conectar (S/N)\r\n");
		option=_getche();


	}while(option!='n' && option!='N');

	return(0);
}
