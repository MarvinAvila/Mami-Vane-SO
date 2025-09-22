#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>

#define MAX_CLIENTES 10
#define TAM_BUFFER 256
#define FIFO_SERVIDOR "/tmp/chat_servidor"

typedef struct {
    pid_t pid;
    char fifo_lectura[50];
} Cliente;

Cliente clientes[MAX_CLIENTES];
int num_clientes = 0;

void manejar_senal(int sig) {
    (void)sig;
    printf("\nServidor apagado.\n");
    unlink(FIFO_SERVIDOR);
    exit(0);
}

void registrar_cliente(pid_t pid) {
    if (num_clientes >= MAX_CLIENTES) return;

    snprintf(clientes[num_clientes].fifo_lectura, 50, "/tmp/chat_cliente_%d_lectura", pid);
    clientes[num_clientes].pid = pid;
    num_clientes++;

    printf("Cliente %d se ha unido al chat\n", pid);

    char aviso[512];
    snprintf(aviso, sizeof(aviso), ">>> Cliente %d se ha unido al chat <<<", pid);

    for (int i = 0; i < num_clientes; i++) {
        if (clientes[i].pid != pid) {
            int fd = open(clientes[i].fifo_lectura, O_WRONLY | O_NONBLOCK);
            if (fd != -1) {
                write(fd, aviso, strlen(aviso) + 1);
                close(fd);
            }
        }
    }
}

void reenviar_a_todos(pid_t remitente, const char *mensaje) {
    char mensaje_final[512];
    snprintf(mensaje_final, sizeof(mensaje_final), "Cliente %d: %s", remitente, mensaje);

    for (int i = 0; i < num_clientes; i++) {
        if (clientes[i].pid != remitente) {
            int fd = open(clientes[i].fifo_lectura, O_WRONLY | O_NONBLOCK);
            if (fd != -1) {
                write(fd, mensaje_final, strlen(mensaje_final) + 1);
                close(fd);
            }
        }
    }
}

int main() {
    char buffer[TAM_BUFFER];

    signal(SIGINT, manejar_senal);

    mkfifo(FIFO_SERVIDOR, 0666);

    printf("Servidor de chat iniciado.\n");
    printf("FIFO del servidor: %s\n", FIFO_SERVIDOR);
    printf("Esperando conexiones...\n\n");

    while (1) {
        int fd_servidor = open(FIFO_SERVIDOR, O_RDONLY);
        if (fd_servidor == -1) {
            perror("Error al abrir FIFO del servidor");
            exit(1);
        }

        memset(buffer, 0, TAM_BUFFER);
        if (read(fd_servidor, buffer, TAM_BUFFER) > 0) {
            pid_t pid;
            char mensaje[TAM_BUFFER];
            if (sscanf(buffer, "%d|%255[^\n]", &pid, mensaje) == 2) {
                int cliente_registrado = 0;
                for (int i = 0; i < num_clientes; i++) {
                    if (clientes[i].pid == pid) {
                        cliente_registrado = 1;
                        break;
                    }
                }

                if (!cliente_registrado) {
                    registrar_cliente(pid);
                }

                if (strcmp(mensaje, "/salir") == 0) {
                    printf("Cliente %d ha salido.\n", pid);
                } else {
                    printf("Cliente %d: %s\n", pid, mensaje);
                    reenviar_a_todos(pid, mensaje);
                }
            }
        }
        close(fd_servidor);

        // Reemplazo de usleep(100000) por nanosleep(100ms)
        struct timespec ts = {0, 100 * 1000000L}; // 100ms
        nanosleep(&ts, NULL);
    }

    return 0;
}
