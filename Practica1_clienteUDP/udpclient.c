#include <stdio.h>
#include <ws2tcpip.h>
#include <time.h>

#pragma comment(lib, "Ws2_32.lib")//Busca la biblioteca Ws2_32.lib

#define UDP_CLIENT_PORT	60001
#define UDP_SERVER_PORT	60000

int main(int *argc, char *argv[]){
	// Variables de incialización de los Windows Sockets
	WORD wVersionRequested;
	WSADATA wsaData;

	// Fin Variables de incialización de los Windows Sockets
	SOCKET sockfd;
	struct sockaddr_in client_in,server_in,input_in;
	int input_l;
	char buffer_in[2048], buffer_out[2048];
	int in_len;
	char user_input[1024];
	int recibidos=0;
	int enviados=0;
	char iplocal[32]="192.168.66.253";
    char ipdest[32]="127.0.0.1";
	int n_secuencia=1;
	int err=0;
	char cadtemp[32];

	//-------------------------------
	char mensaje[20] = "";
	char remitente[8];
	char destinatario[8];
	char texto[1000];
	//-------------------------------
	
	// Inicialización Windows Sockets versión 2.2
	wVersionRequested=MAKEWORD(2,2);
	err=WSAStartup(wVersionRequested,&wsaData);
	if(err!=0)
		return(0);

	if(LOBYTE(wsaData.wVersion)!=2||HIBYTE(wsaData.wVersion)!=2){
		WSACleanup();
		return(0);
	}// Fin Inicialización Windows Sockets

	sockfd=socket(PF_INET,SOCK_DGRAM,0); //SOCKET crea el socket 
	if(sockfd==INVALID_SOCKET){   //Comprueba si hay error 
		printf("CLIENTE UDP> Error, no se ha creado el socket\r\n");
	}
	else {
		printf("CLIENTE UDP> Socket creado con exito\r\n");
		//Dirección local del cliente
		client_in.sin_family = AF_INET;
		client_in.sin_port = htons(UDP_CLIENT_PORT);


		printf("CLIENTE UDP> IP del cliente [%s] : ", iplocal);

		gets_s(cadtemp, sizeof(cadtemp));
		if (strcmp(cadtemp, "") != 0) 
			strcpy_s(iplocal, sizeof(iplocal), cadtemp);
		
	
			inet_pton(AF_INET, iplocal, &client_in.sin_addr.s_addr);
		

		if(bind(sockfd,(struct sockaddr *)&client_in,sizeof(client_in))==SOCKET_ERROR){  //SOCKET asocia una dirección local
			printf("CLIENTE UDP> Error %d\r\n",GetLastError());
		
		}else{
		

				// Dirección remota del servidor para cada envío
				printf("CLIENTE UDP> IP del servidor [%s] : ",ipdest);
				
				gets_s(cadtemp,sizeof(cadtemp));
				if(strcmp(cadtemp,"")!=0)
					strcpy_s(ipdest,sizeof(ipdest),cadtemp);

				server_in.sin_family=AF_INET;
				server_in.sin_port=htons(UDP_SERVER_PORT);
				//server_in.sin_addr.s_addr=inet_addr(ipdest);
				inet_pton(AF_INET,ipdest,&server_in.sin_addr.s_addr);

			do{// Se estarán enviando mensajes de eco hasta que se pulse solo un enter

				//printf("CLIENTE UDP> Introduzca una cadena para enviar al servidor: ");
				//gets_s(user_input,sizeof(user_input));
				//sprintf_s(buffer_out,sizeof(buffer_out),"ECHO %d %s\r\n",n_secuencia,user_input);

				//-------------------------------------------------------------------
				printf("CLIENTE UDP> Introduce una cadena para el servidor con el formato (MENSAJE remitente destinatario texto): ");
				gets_s(user_input, sizeof(user_input));
				strncpy_s(mensaje, sizeof(mensaje), user_input, 8); //copiamos la variable usar_input en la variable mensaje
				sscanf_s(user_input, "MENSAJE%s %s %s\r\n", remitente, sizeof(remitente), destinatario, sizeof(destinatario), texto, sizeof(texto));
				sprintf_s(buffer_out,sizeof(buffer_out), "%s %s %s %s\r\n",mensaje, remitente, destinatario, texto);
				//-------------------------------------------------------------------
				
				enviados=sendto(sockfd,buffer_out,(int)strlen(buffer_out),0,(struct sockaddr *)&server_in,sizeof(server_in));  //SOCKET enviar datos al servidor
				if(enviados!=SOCKET_ERROR){
					printf("CLIENTE UDP> Enviados %d bytes\r\n",enviados);				
					printf("CADENA ENVIADA AL SERVIDOR --> ");
					printf(buffer_out,sizeof(buffer_out),"%s%s %s %s\r\n",mensaje, remitente, destinatario, texto);
					in_len=sizeof(buffer_in);
					input_l=sizeof(input_in);

					recibidos=recvfrom(sockfd,buffer_in,in_len,0,(struct sockaddr *)&input_in,&input_l);  //SOCKET recibir datos del servidor
					if(recibidos!=SOCKET_ERROR){
						
						char peer[32]="";
						int r_secuencia=0;
						char eco[1024] ="";
						buffer_in[recibidos]=0;
						
						inet_ntop(AF_INET, &input_in.sin_addr, peer, sizeof(peer));
						
						printf("CLIENTE UDP> Recibidos %d bytes de %s %d\r\n",recibidos,peer/*inet_ntoa(input_in.sin_addr)*/,ntohs(input_in.sin_port));
						
						//---------------------------------------------------------------
						scanf_s(buffer_in, "OK %d %s %s",&r_secuencia, remitente, texto);
						if(r_secuencia==n_secuencia && strlen(remitente) && strlen(texto)>0){
							printf("CLIENTE UDP> Mensaje recibido: %s %s\r\n",remitente, texto);
						}else{
							printf("CLIENTE UDP> Error en la respuesta");
						}
						//--------------------------------------------------------------

					}
					n_secuencia++;
				}
			}while(strcmp("",user_input)!=0);
		}
		closesocket(sockfd);         //SOCKET cierra el socket
	}//fin sockfd==INVALID_SOCKET
	
	WSACleanup();// solo Windows

}//fin main