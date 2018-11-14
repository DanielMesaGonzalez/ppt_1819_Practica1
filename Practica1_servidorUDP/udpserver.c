#include <stdio.h>
#include <ws2tcpip.h>
#include <time.h>

//******************************************************
//EXPLICACIÓN APARTADO 10
//El servidor realiza un servicio de respuesta mediante mensajes al cliente como si fuese un eco
//Los comandos de aplicación que se usan entre cliente y servidor es el comando MENSAJE







#define UDP_CLIENT_PORT	60001//Puerto del que deben venir los mensajes
#define UDP_SERVER_PORT	60000//Puerto en el que el servidor recibirá peticiones

#pragma comment(lib, "Ws2_32.lib")//Busca la biblioteca Ws2_32.lib

int main(int *argc, char *argv[])
{
	// Variables de incialización de los Windows Sockets
	WORD wVersionRequested;
	WSADATA wsaData;
	// Fin Variables de incialización de los Windows Sockets

	SOCKET sockfd;
	struct sockaddr_in server_in,input_in;
	int input_l;
	char buffer_in[2048], buffer_out[2048];
	char command[9];
	char user_input[1024]= "";
	int recibidos=0;
	int enviados=0;
	char iplocal[32]="10.82.241.165";  //OJOOOOO AQUI DEBEMOS DE PONER LA IP DONDE SE ALOJA EL SERVIDOR
	int n_secuencia=0;
	int err=0;

	//-------------------------------------------
	char remitente[8];
	char destinatario[8];
	char texto[1000];
	//-------------------------------------------

	// Inicialización Windows Sockets
	wVersionRequested=MAKEWORD(2,2);
	err=WSAStartup(wVersionRequested,&wsaData);
	if(err!=0)
		return(0);

	if(LOBYTE(wsaData.wVersion)!=2||HIBYTE(wsaData.wVersion)!=2){
		WSACleanup();
		return(0);
	}// Fin Inicialización Windows Sockets
	
	sockfd=socket(PF_INET,SOCK_DGRAM,0);    // SOCKET creamos el socket
	if(sockfd==INVALID_SOCKET){ //Comprobamos si hay errror
		printf("SERVIDOR UDP> Error al crear el socket\r\n");
	}else{
		printf("SERVIDOR UDP> Socket creado correctamente\r\n");

		//Dirección local del servidor
		server_in.sin_family=AF_INET;
		server_in.sin_port=htons(UDP_SERVER_PORT);
		//server_in.sin_addr.s_addr=inet_addr(iplocal);   
		inet_pton(AF_INET,iplocal,&server_in.sin_addr.s_addr);
		
		if(bind(sockfd,(struct sockaddr *)&server_in,sizeof(server_in))==SOCKET_ERROR){  //SOCKET se asocia una dirección local
			printf("SERVIDOR UDP> Error en la asignacion de ip local %d:\r\n",GetLastError());
		}else{
			printf("SERVIDOR UDP> Bienvenido al Servidor de Eco Sencillo UDP\r\n");

			while(1){//Bucle infinito de servicio
				input_l=sizeof(input_in);
				recibidos=recvfrom(sockfd,buffer_in,2047,0,(struct sockaddr *)&input_in,&input_l);  //SOCKET recibe los datos del cliente
				if(recibidos!=SOCKET_ERROR){
					char peer[32]=" ";
					buffer_in[recibidos]=0;
					inet_ntop(AF_INET, &input_in.sin_addr, peer, sizeof(peer));
					printf("SERVIDOR UDP> Recibido de %s %d: %s\r\n",peer/*inet_ntoa(input_in.sin_addr)*/,ntohs(input_in.sin_port),buffer_in);
					if(ntohs(input_in.sin_port)==UDP_CLIENT_PORT){// Se comprueba que el mensaje llegue desde el puerto típico para
																  // este servicio, el 6001. Si no es así no se lleva a cabo ninguna
																  // acción.
						sscanf_s(buffer_in,"%s %d %s %[^\r]s\r\n",command,sizeof(command),&n_secuencia,user_input,sizeof(user_input));

						   //-------------------------------------------------------------
						    if (strcmp(command,"MENSAJE")==0){  //Si le llega el comando mensaje, lee la cadena y envía una respuesta
							sscanf_s(user_input, "%s %s %s\r\n", remitente, sizeof(remitente), destinatario, sizeof(destinatario), texto, sizeof(texto));
							sprintf_s(buffer_out,sizeof(buffer_out), "RESPUESTA %d %s %s\r\n",n_secuencia, remitente, texto); //Respuesta al cliente
						   }

						   else {
							printf("ERROR");
							//sprinf_s(buffer_out, sizeof(buffer_out),"ER");   
							
						   }
						    //-------------------------------------------------------------

							//sprintf_s(buffer_out, sizeof(buffer_out), "RESPUESTA %d %s %s\r\n", n_secuencia, remitente, texto); //Respuesta al cliente
							enviados=sendto(sockfd,buffer_out,(int)strlen(buffer_out),0,(struct sockaddr *)&input_in,sizeof(input_in));  //SOCKET envía los datos del cliente
					}
				}//Si hay un error de recepción se silencia
			}//Fin bucle del servicio
		}		
	}//fin sockfd==INVALID_SOCKET
}//fin main