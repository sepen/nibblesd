#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "defines.h"

int main(void)
{
    fd_set master;   // conjunto maestro de descriptores de fichero
    fd_set read_fds; // conjunto temporal de descriptores de fichero para select()
    struct sockaddr_in myaddr;     // direcci�n del servidor
    struct sockaddr_in remoteaddr; // direcci�n del cliente
    int fdmax;        // n�mero m�ximo de descriptores de fichero
    int listener;     // descriptor de socket a la escucha
    int newfd;        // descriptor de socket de nueva conexi�n aceptada
    char buf[256];    // buffer para datos del cliente 
    int nbytes;
    int yes=1;        // para setsockopt() SO_REUSEADDR, m�s abajo
    int addrlen;
    int i, j;
    FD_ZERO(&master);    // borra los conjuntos maestro y temporal
    FD_ZERO(&read_fds);

    // obtener socket a la escucha
    if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
	perror("socket");
	exit(1);
    }

    // obviar el mensaje "address already in use" (la direcci�n ya se est� usando)
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
	perror("setsockopt");
	exit(1);
    }

    // enlazar
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = INADDR_ANY;
    myaddr.sin_port = htons(PORT);
    memset(&(myaddr.sin_zero), '\0', 8);

    if (bind(listener, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1)
    {
	perror("bind");
	exit(1);
    }

    // escuchar
    if (listen(listener, 10) == -1)
    {
	perror("listen");
	exit(1);
    }

    // a�adir listener al conjunto maestro
    FD_SET(listener, &master);
    // seguir la pista del descriptor de fichero mayor
    fdmax = listener; // por ahora es �ste

    printf ("[nibblesd]\nServer listening on port %d\n", PORT);

    // bucle principal
    while (1)
    {
	read_fds = master; // copialo
	if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)
	{
	    perror("select");
	    exit(1);
	}
	// explorar conexiones existentes en busca de datos que leer
	for(i = 0; i <= fdmax; i++)
	{
	    if (FD_ISSET(i, &read_fds))
	    { // ��tenemos datos!!
		if (i == listener)
		{
		    // gestionar nuevas conexiones
		    addrlen = sizeof(remoteaddr);
		    if ((newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen)) == -1)
		    { 
			perror("accept");
		    } 
		    else
		    {
			FD_SET(newfd, &master); // a�adir al conjunto maestro
			if (newfd > fdmax)
			{ // actualizar el m�ximo
			    fdmax = newfd;
			}
			printf("New connection from %s on socket %d\n", inet_ntoa(remoteaddr.sin_addr), newfd);
		    }
		} 
		else
		{
		    // gestionar datos de un cliente
		    if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0)
		    {
			// error o conexi�n cerrada por el cliente
			if (nbytes == 0)
			{
			    // conexi�n cerrada
			    printf("SelectServer: socket %d hung up\n", i);
			} 
			else
			{
			    perror("recv");
			}	
			close(i); // �Hasta luego!
			FD_CLR(i, &master); // eliminar del conjunto maestro
		    } 
		    else {
			// tenemos datos de alg�n cliente
			for(j = 0; j <= fdmax; j++)
			{
			    // �enviar a todo el mundo!
			    if (FD_ISSET(j, &master))
			    {
				// excepto al listener y a nosotros mismos
				if (j != listener && j != i)
				{
				    if (send(j, buf, nbytes, 0) == -1)
				    {
					perror("send");
				    }
				}
			    }
			}
		    }
		}
	    }
	}
    }
    return 0;
} 
