#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

int main() {
    int pipe_padre_hijo[2];  // Pipe para padre -> hijo
    int pipe_hijo_padre[2];  // Pipe para hijo -> padre
    pid_t pid;
    char buffer[BUFFER_SIZE];
    int bytes_leidos;

    // Crear los pipes
    if (pipe(pipe_padre_hijo) == -1 || pipe(pipe_hijo_padre) == -1) {
        perror("Error creando pipes");
        exit(EXIT_FAILURE);
    }

    // Crear proceso hijo
    pid = fork();
    
    if (pid == -1) {
        perror("Error en fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {  // Proceso hijo
        // Cerrar los extremos no utilizados
        close(pipe_padre_hijo[1]);  // Cerrar escritura del pipe padre->hijo
        close(pipe_hijo_padre[0]);  // Cerrar lectura del pipe hijo->padre

        printf("Hijo: Chat iniciado. Escribe 'exit' para terminar.\n");
        
        while (1) {
            // Leer mensaje del padre
            bytes_leidos = read(pipe_padre_hijo[0], buffer, BUFFER_SIZE - 1);
            if (bytes_leidos > 0) {
                buffer[bytes_leidos] = '\0';
                printf("Padre dice: %s", buffer);
                
                if (strncmp(buffer, "exit", 4) == 0) {
                    break;
                }
            }

            // Escribir respuesta al padre
            printf("Hijo: ");
            fgets(buffer, BUFFER_SIZE, stdin);
            
            write(pipe_hijo_padre[1], buffer, strlen(buffer));
            
            if (strncmp(buffer, "exit", 4) == 0) {
                break;
            }
        }

        // Cerrar pipes
        close(pipe_padre_hijo[0]);
        close(pipe_hijo_padre[1]);
        exit(EXIT_SUCCESS);

    } else {  // Proceso padre
        // Cerrar los extremos no utilizados
        close(pipe_padre_hijo[0]);  // Cerrar lectura del pipe padre->hijo
        close(pipe_hijo_padre[1]);  // Cerrar escritura del pipe hijo->padre

        printf("Padre: Chat iniciado. Escribe 'exit' para terminar.\n");
        
        while (1) {
            // Escribir mensaje al hijo
            printf("Padre: ");
            fgets(buffer, BUFFER_SIZE, stdin);
            
            write(pipe_padre_hijo[1], buffer, strlen(buffer));
            
            if (strncmp(buffer, "exit", 4) == 0) {
                break;
            }

            // Leer respuesta del hijo
            bytes_leidos = read(pipe_hijo_padre[0], buffer, BUFFER_SIZE - 1);
            if (bytes_leidos > 0) {
                buffer[bytes_leidos] = '\0';
                printf("Hijo dice: %s", buffer);
                
                if (strncmp(buffer, "exit", 4) == 0) {
                    break;
                }
            }
        }

        // Esperar a que el hijo termine
        wait(NULL);
        
        // Cerrar pipes
        close(pipe_padre_hijo[1]);
        close(pipe_hijo_padre[0]);
        
        printf("Chat finalizado.\n");
    }

    return 0;
}