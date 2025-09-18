
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int fd, tubo[2]; //      | 0 | 1 |
    char cad[50];
    pipe(tubo);  //creando el tubo
    fd = fork();
    if(fd==-1)
       printf("Error");
    if(fd==0)
    {
      //hijo
        printf("\nEn hijo");
        read(tubo[0],cad,sizeof(cad));
        printf("\nCadena recibida de papá: %s",cad);
        
    }
    else
    {
        //papa
        printf("En papá\n");
        write(tubo[1],"Hola hijo",9);}
    
    
    return 0 ; 
}
    
    
    
    
    
    
    
    
    
    
    
    /*char cad[3];
    write(1,"lo que se escribe\n",18); //El write requiere 3 parametros es en antepasado de printf
    write(1,"Dame un dato:",14);
    read(0,cad,sizeof(cad));   //scanf
    write(1,"El dato capturado es:",22);
    write(1,cad,sizeof(cad));*/

