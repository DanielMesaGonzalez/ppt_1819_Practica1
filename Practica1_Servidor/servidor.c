/*******************************************************
Protocolos de Transporte
Grado en Ingeniería Telemática
Dpto. Ingeníería de Telecomunicación
Univerisdad de Jaén

Fichero: servidor.c
Versión: 1.0
Fecha: 09/2018
Descripción:
	Servidor de eco sencillo TCP sobre IPv6.

Autores: 
         Javier Sánchez Samper
         Daniel Mesa González

*******************************************************/
#include <stdio.h>
#include <ws2tcpip.h> //Necesaria para las funciones IPv6
#include <conio.h>
#include "protocol.h" //Sirve para declarar constantes y funciones
#include <string.h>

#pragma comment(lib, "Ws2_32.lib")

int main(int *argc, char *argv[])
{

	WORD wVersionRequested;
	WSADATA wsaData;
	SOCKET sockfd,nuevosockfd;
	struct sockaddr *server_in = NULL;
	struct sockaddr *remote_addr = NULL;
	struct sockaddr_in server_in4;
	struct sockaddr_in6  server_in6;
	int address_size = sizeof(struct sockaddr_in6);
	char remoteaddress[128] = "";
	unsigned short remoteport = 0;
	char buffer_out[1024],buffer_in[1024], cmd[10], usr[10], pas[10];
	int err,tamanio;
	int fin=0, fin_conexion=0;
	int recibidos=0,enviados=0;
	int estado=0;
	int ipversion = AF_INET;//IPv4 por defecto
	char ipdest[256];
	char opcion[256];
	//-------------------------------------------
	int numero1, numero2;
	int total;
	//-------------------------------------------

	/** INICIALIZACION DE BIBLIOTECA WINSOCK2 **
	 ** OJO!: SOLO WINDOWS                    **/
	wVersionRequested=MAKEWORD(1,1);
	err=WSAStartup(wVersionRequested,&wsaData);
	if(err!=0){
		return(-1);
	}
	if(LOBYTE(wsaData.wVersion)!=1||HIBYTE(wsaData.wVersion)!=1){
		WSACleanup() ;
		return(-2);
	}
	/** FIN INICIALIZACION DE BIBLIOTECA WINSOCK2 **/
	printf("SERVIDOR> ¿Qué versión de IP desea usar?\r\n\t 6 para IPv6, 4 para IPv4 [por defecto] ");
	gets_s(opcion, sizeof(ipdest));

	if (strcmp(opcion, "6") == 0) {
		ipversion = AF_INET6;

	} else { //Distinto de 6 se elige la versión 4
		ipversion = AF_INET;
	}


	sockfd=socket(ipversion,SOCK_STREAM,IPPROTO_TCP);    //SOCKET creamos el socket

	if(sockfd==INVALID_SOCKET){       //SOCKET comprobamos los errores
		DWORD error = GetLastError();
		printf("Error %d\r\n", error);
		return (-1);
	}
	else {
		if (ipversion == AF_INET6) {
			memset(&server_in6, 0, sizeof(server_in6));
			server_in6.sin6_family = AF_INET6; // Familia de protocolos IPv6 de Internet
			server_in6.sin6_port = htons(TCP_SERVICE_PORT);// Puerto del servidor
			//inet_pton(ipversion, "::1", &server_in6.sin6_addr);	// Direccion IP del servidor
																	// Se debe cambiar para que conincida con la de la interfaz
																	// del host que se quiera usar
			server_in6.sin6_addr = in6addr_any;//Conexiones de cualquier interfaz y de IPv4 o IPv6
			server_in6.sin6_flowinfo = 0;
			server_in = (struct sockaddr*)&server_in6;
			address_size = sizeof(server_in6);
		}
		else {
			//ipversion == AF_INET
			memset(&server_in4, 0, sizeof(server_in4));
			server_in4.sin_family = AF_INET; // Familia de protocolos IPv4 de Internet
			server_in4.sin_port = htons(TCP_SERVICE_PORT);// Puerto del servidor
			server_in4.sin_addr.s_addr=INADDR_ANY;
			//inet_pton(ipversion, "127.0.0.1", &server_in4.sin_addr.s_addr);//Dirección de loopback
			server_in = (struct sockaddr*)&server_in4;
			address_size = sizeof(server_in4);
		}

	}
	
	if (bind(sockfd, (struct sockaddr*)server_in, address_size) < 0) {  //SOCKET asigna dirección local con un socket 
		DWORD error = GetLastError();
		printf("Error %d\r\n", error);
		return (-2);
	}

	if (listen(sockfd, 5) != 0) {    //SOCKET crea cola de espera para almacenar solicitudes de conexión
		DWORD error = GetLastError();
		printf("Error %d\r\n", error);
		return (-3);
	}
		
	
	

	do{
		printf ("SERVIDOR> ESPERANDO NUEVA CONEXION DE TRANSPORTE\r\n");
		tamanio = address_size;
		remote_addr = malloc(tamanio);

		nuevosockfd=accept(sockfd,(struct sockaddr*)remote_addr,&tamanio);  //SOCKET espera solicitud de conexión
		if(nuevosockfd==INVALID_SOCKET){   //SOCKET comprueba errores
			DWORD error = GetLastError();
			printf("Error %d\r\n", error);
			return (-4);
		}

		if (ipversion == AF_INET6) {
			struct sockaddr_in6  *temp = (struct sockaddr_in6 *)remote_addr;
			inet_ntop(AF_INET6,temp, remoteaddress, sizeof(remoteaddress));
			remoteport = ntohs(temp->sin6_port);
		}
		else {
			struct sockaddr_in  *temp = (struct sockaddr_in *)remote_addr;
			inet_ntop(AF_INET,temp, remoteaddress, sizeof(remoteaddress));
			remoteport = ntohs(temp->sin_port);
		}

		printf ("SERVIDOR> CLIENTE CONECTADO DESDE %s:%u\r\n",remoteaddress,remoteport);

		//Mensaje de Bienvenida
		sprintf_s (buffer_out, sizeof(buffer_out), "%s Bienvenido al servidor Sencillo%s",OK,CRLF);
		
		enviados=send(nuevosockfd,buffer_out,(int)strlen(buffer_out),0);  //SOCKET envia datos al cliente
 
		//Se reestablece el estado inicial
		estado = S_USER;
		fin_conexion = 0;

		printf ("SERVIDOR> Esperando conexion de aplicacion\r\n");
		do{
			//Se espera un comando del cliente
			recibidos = recv(nuevosockfd,buffer_in,1023,0);    //SOCKET recibe datos del cliente

			//DEBEMOS COMPROBAR LOS ERRORES AQUI PARA QUE NO DE LA EXCEPCION
			
			//--------------------------------------------------------------
			/*int salida;
			if (recibidos == -1 || recibidos == 0){
				printf("Error en la lectura de datos (recibidos)");
				estado = S_QUIT;
			}
			else {
				estado = S_HELO;
			}*/

			//--------------------------------------------------------------

			buffer_in[recibidos] = 0x00;// Dado que los datos recibidos se tratan como cadenas
										// se debe introducir el carácter 0x00 para finalizarla
										// ya que es así como se representan las cadenas de caracteres
										// en el lenguaje C

			printf ("SERVIDOR RECIBIDO [%d bytes]>%s\r\n", recibidos, buffer_in);
			
			//SE analiza el formato de la PDU de aplicación (APDU)
			strncpy_s ( cmd, sizeof(cmd),buffer_in, 4);
			cmd[4]=0x00; // Se finaliza la cadena
			printf ("SERVIDOR COMANDO RECIBIDO>%s\r\n",cmd);

			//Máquina de estados del servidor para seguir el protocolo
			//En función del estado en el que se encuentre habrá unos comandos permitidos
			switch (estado){
				case S_USER:
					if ( strcmp(cmd,SC)==0 ){ // si recibido es solicitud de conexion de aplicacion
					
						sscanf_s (buffer_in,"USER %s\r\n",usr,sizeof(usr));
						
						// envia OK acepta todos los usuarios hasta que tenga la clave
						sprintf_s (buffer_out, sizeof(buffer_out), "%s%s", OK,CRLF);
						
						estado = S_PASS;
						printf ("SERVIDOR> Esperando clave\r\n");
					} else
					if ( strcmp(cmd,SD)==0 ){
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Fin de la conexión%s", OK,CRLF);
						fin_conexion=1;
					}
					else{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Comando incorrecto%s",ERORR,CRLF);
					}
				break;

				case S_PASS:
					if ( strcmp(cmd,PW)==0 ){ // si comando recibido es password
					
						sscanf_s (buffer_in,"PASS %s\r\n",pas,sizeof(usr));

						if ( (strcmp(usr,USER)==0) && (strcmp(pas,PASSWORD)==0) ){ // si password recibido es correcto
						
							// envia aceptacion de la conexion de aplicacion, nombre de usuario y
							// la direccion IP desde donde se ha conectado
							sprintf_s (buffer_out, sizeof(buffer_out), "%s %s IP(%s)%s", OK, usr, remoteaddress,CRLF);
							estado = S_DATA;
							printf ("SERVIDOR> Esperando comando\r\n");
						}
						else{
							sprintf_s (buffer_out, sizeof(buffer_out), "%s Autenticación errónea%s",ERORR,CRLF);
						}
					} else	if ( strcmp(cmd,SD)==0 ){
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Fin de la conexión%s", OK,CRLF);
						fin_conexion=1;
					}
					else{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Comando incorrecto%s",ERORR,CRLF);
					}
				break;

				case S_DATA: /***********************************************************/				
					buffer_in[recibidos] = 0x00;
					strncpy_s(cmd, sizeof(cmd), buffer_in, 4);
					
					if ( strcmp(cmd,SD)==0 ){
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Fin de la conexión%s", OK,CRLF);
						fin_conexion=1;
					}
					else if (strcmp(cmd,SD2)==0){
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Finalizando servidor%s", OK,CRLF);
						fin_conexion=1;
						fin=1;
					}
					// ---------------------------------
					else if (strcmp(cmd, "SUM ") == 0) {
						sscanf_s(buffer_in, "SUM %d %d%s", &numero1, &numero2, CRLF);
						if (numero1 < 10000 && numero2 < 10000) {
							total = numero1 + numero2;
							printf("Suma= %d\r\n", total);
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s%d%s%s", OK, SP, total, SP, CRLF);
						}
						//---------------------
						else {
							sprintf_s(buffer_out, sizeof(buffer_out), "%s Comando incorrecto: %s", ERORR, CRLF);
						}
					}
					break;

				default:
					break;
					
			} // switch

			enviados=send(nuevosockfd,buffer_out,(int)strlen(buffer_out),0);
			//--------------------------------------------------------------
			if (enviados <= 0) {
				if (enviados < 0) {
					DWORD error = GetLastError();
					printf("Error al enviar datos");
					fin_conexion = 1;
				}
				else {
					fin_conexion = 1;
				}
			}
			//--------------------------------------------------------------

		} while (!fin_conexion);
		printf ("SERVIDOR> CERRANDO CONEXION DE TRANSPORTE\r\n");
		shutdown(nuevosockfd,SD_SEND);
		closesocket(nuevosockfd);

	}while(!fin);

	printf ("SERVIDOR> CERRANDO SERVIDOR\r\n");

	return(0);
} 
