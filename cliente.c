#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/wait.h>
	
int main ()
{	int fd;
    char cad[30];
   fd=open("fifo1",O_RDONLY);
   read( fd,cad, 12);
   printf("%s\n",cad);
   read( fd,cad, 15);
   printf("%s\n",cad);
   read( fd,cad, 18);
   printf("%s\n",cad);
   close(fd);
}
