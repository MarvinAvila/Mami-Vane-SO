#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

	
int main ()
{	int fd;
    char cad[30];
    if(mkfifo("fifo1",0666)==-1)
       {printf("Error al crear el fifo");
        exit(0);
       }
    fd=open("fifo1",O_WRONLY);
    write( fd,"Hola vecino" , 12);
    write( fd,"Hola que haces" , 15);
    write( fd,"vamos por el cafe" , 18);

    close(fd);
	
}
