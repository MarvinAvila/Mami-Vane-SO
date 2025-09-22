#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

#define TAM_BUFFER 256
#define FIFO_SERVIDOR "/tmp/chat_servidor"

int cliente_fifo_lectura;
int cliente_fifo_escritura;
char fifo_lectura[50];
char fifo_escritura[50];
pid_t mi_pid;

void manejar_senal(int sig) {
    (void)sig;
    printf("\nSaliendo del chat...\n");
    
    char mensaje_salida[TAM_BUFFER];
    snprintf(mensaje_salida, TAM_BUFFER, "%d|/salir", mi_pid);
    
    printf("DEBUG: Enviando mensaje de salida: %s\n", mensaje_salida);
    int fd = open(FIFO_SERVIDOR, O_WRONLY);
    if (fd != -1) {
        write(fd, mensaje_salida, strlen(mensaje_salida) + 1);
        close(fd);
    } else {
        printf("DEBUG: Error abriendo FIFO servidor: %s\n", strerror(errno));
    }
    
    close(cliente_fifo_lectura);
    close(cliente_fifo_escritura);
    
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
    printf("FIFO Servidor: %s\n", FIFO_SERVIDOR);
    printf("Escribe tu mensaje (o '/salir' para salir):\n\n");
    
    snprintf(fifo_lectura, 50, "/tmp/chat_cliente_%d_lectura", mi_pid);
    snprintf(fifo_escritura, 50, "/tmp/chat_cliente_%d_escritura", mi_pid);
    
    printf("DEBUG: Creando FIFOs:\n");
    printf("DEBUG - Lectura: %s\n", fifo_lectura);
    printf("DEBUG - Escritura: %s\n", fifo_escritura);
    
    if (mkfifo(fifo_lectura, 0666) == -1) {
        printf("DEBUG: Error creando FIFO lectura: %s\n", strerror(errno));
    }
    if (mkfifo(fifo_escritura, 0666) == -1) {
        printf("DEBUG: Error creando FIFO escritura: %s\n", strerror(errno));
    }
    
    cliente_fifo_escritura = open(fifo_escritura, O_WRONLY);
    
    if (cliente_fifo_escritura == -1) {
        printf("DEBUG: Error abriendo FIFO escritura: %s\n", strerror(errno));
        perror("Error al abrir FIFO de escritura del cliente");
        exit(1);
    }
    
    pid_t hijo = fork();
    
    if (hijo == 0) {
        // Proceso hijo - lectura
        close(cliente_fifo_escritura);
        
        printf("DEBUG [HIJO]: Abriendo FIFO lectura: %s\n", fifo_lectura);
        cliente_fifo_lectura = open(fifo_lectura, O_RDONLY);
        if (cliente_fifo_lectura == -1) {
            printf("DEBUG [HIJO]: Error abriendo FIFO lectura: %s\n", strerror(errno));
            exit(1);
        }
        
        printf("DEBUG [HIJO]: Listo para recibir mensajes...\n");
        char buffer_hijo[TAM_BUFFER];
        while (1) {
            memset(buffer_hijo, 0, TAM_BUFFER);
            int bytes_leidos = read(cliente_fifo_lectura, buffer_hijo, TAM_BUFFER);
            
            if (bytes_leidos > 0) {
                printf("\n--- MENSAJE RECIBIDO ---\n%s\n-----------------------\n", buffer_hijo);
                printf("Tú: ");
                fflush(stdout);
            } else if (bytes_leidos == 0) {
                printf("DEBUG [HIJO]: FIFO cerrado\n");
                break;
            } else {
                printf("DEBUG [HIJO]: Error leyendo: %s\n", strerror(errno));
            }
            
            sleep(1);
        }
        
        close(cliente_fifo_lectura);
        exit(0);
        
    } else {
        // Proceso padre - escritura
        close(cliente_fifo_lectura);
        
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
                
                printf("DEBUG [PADRE]: Enviando mensaje: %s\n", mensaje);
                int fd = open(FIFO_SERVIDOR, O_WRONLY);
                if (fd != -1) {
                    ssize_t bytes = write(fd, mensaje, strlen(mensaje) + 1);
                    printf("DEBUG [PADRE]: Mensaje enviado (%zd bytes)\n", bytes);
                    close(fd);
                } else {
                    printf("DEBUG [PADRE]: Error conectando al servidor: %s\n", strerror(errno));
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
        
        close(cliente_fifo_escritura);
        unlink(fifo_lectura);
        unlink(fifo_escritura);
    }
    
    return 0;
}