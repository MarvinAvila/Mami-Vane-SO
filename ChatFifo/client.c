#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>

#define TAM_BUFFER 256
#define FIFO_SERVIDOR "/tmp/chat_servidor"

int cliente_fifo_lectura;
char fifo_lectura[50];
char fifo_escritura[50];
pid_t mi_pid;

void manejar_senal(int sig) {
    (void)sig; // evitar warning

    printf("\nSaliendo del chat...\n");

    char mensaje_salida[TAM_BUFFER];
    snprintf(mensaje_salida, TAM_BUFFER, "%d|/salir", mi_pid);

    int fd = open(FIFO_SERVIDOR, O_WRONLY);
    if (fd != -1) {
        write(fd, mensaje_salida, strlen(mensaje_salida) + 1);
        close(fd);
    }

    close(cliente_fifo_lectura);

    unlink(fifo_lectura);
    unlink(fifo_escritura);

    exit(0);
}

int main() {
    char buffer[TAM_BUFFER];
    mi_pid = getpid();

    signal(SIGINT, manejar_senal);

    printf("=== Cliente de Chat FIFO ===\n");
    printf("PID del cliente: %d\n", mi_pid);
    printf("Escribe tu mensaje (o '/salir' para salir):\n\n");

    // Nombres de FIFO según PID
    snprintf(fifo_lectura, 50, "/tmp/chat_cliente_%d_lectura", mi_pid);
    snprintf(fifo_escritura, 50, "/tmp/chat_cliente_%d_escritura", mi_pid);

    // Crear FIFOs propios
    mkfifo(fifo_lectura, 0666);
    mkfifo(fifo_escritura, 0666);

    pid_t hijo = fork();

    if (hijo == 0) {
        // Hijo: se encarga de escuchar mensajes entrantes
        cliente_fifo_lectura = open(fifo_lectura, O_RDONLY);
        if (cliente_fifo_lectura == -1) {
            perror("Error al abrir FIFO de lectura");
            exit(1);
        }

        char buffer_hijo[TAM_BUFFER];
        while (1) {
            memset(buffer_hijo, 0, TAM_BUFFER);
            int bytes_leidos = read(cliente_fifo_lectura, buffer_hijo, TAM_BUFFER);

            if (bytes_leidos > 0) {
                printf("\n%s\n", buffer_hijo);
                printf("Tú: ");
                fflush(stdout);
            }

            sleep(1);
        }

        close(cliente_fifo_lectura);
        exit(0);

    } else {
        // Padre: se encarga de leer del teclado y mandar mensajes al servidor
        sleep(1);

        while (1) {
            printf("Tú: ");
            fflush(stdout);

            if (fgets(buffer, TAM_BUFFER, stdin) != NULL) {
                buffer[strcspn(buffer, "\n")] = 0;

                if (strlen(buffer) > 200) {
                    printf("Mensaje demasiado largo. Máximo 200 caracteres.\n");
                    continue;
                }

                char mensaje[TAM_BUFFER];
                int longitud = snprintf(mensaje, TAM_BUFFER, "%d|%s", mi_pid, buffer);

                if (longitud >= TAM_BUFFER) {
                    printf("Error: Mensaje demasiado largo\n");
                    continue;
                }

                int fd = open(FIFO_SERVIDOR, O_WRONLY);
                if (fd != -1) {
                    write(fd, mensaje, strlen(mensaje) + 1);
                    close(fd);
                } else {
                    perror("Error al conectar con el servidor");
                }

                if (strcmp(buffer, "/salir") == 0) {
                    break;
                }
            }
        }

        printf("Cerrando cliente...\n");
        kill(hijo, SIGTERM);
        wait(NULL);

        unlink(fifo_lectura);
        unlink(fifo_escritura);
    }

    return 0;
}
