#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main() {
    int pipefd[2]; // Array para los descriptores de archivo: pipefd[0] para leer, pipefd[1] para escribir
    pid_t pid;
    char mensaje[] = "Hola desde el proceso padre!";
    char buffer[100];

    // Crear el pipe
    if (pipe(pipefd) == -1) {
        perror("Error al crear el pipe");
        return 1;
    }

    // Crear un proceso hijo
    pid = fork();

    if (pid < 0) {
        perror("Error al crear el proceso hijo");
        return 1;
    } else if (pid == 0) { // Código del proceso hijo
        close(pipefd[1]); // Cerrar el extremo de escritura
        read(pipefd[0], buffer, sizeof(buffer)); // Leer del pipe
        printf("Proceso hijo recibió: %s\n", buffer);
        close(pipefd[0]); // Cerrar el extremo de lectura
    } else { // Código del proceso padre
        close(pipefd[0]); // Cerrar el extremo de lectura
        write(pipefd[1], mensaje, strlen(mensaje) + 1); // Escribir en el pipe
        close(pipefd[1]); // Cerrar el extremo de escritura
    }

    return 0;
}
