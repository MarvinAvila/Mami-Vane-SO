#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <errno.h>


#define MAX_CLIENTES 10
#define TAM_BUFFER 256
#define FIFO_SERVIDOR "/tmp/chat_servidor"
#define FIFO_BASE_CLIENTE "/tmp/chat_cliente_"

typedef struct {
    int id;
    char fifo_lectura[50];
    char fifo_escritura[50];
    pid_t pid;
} Cliente;

Cliente clientes[MAX_CLIENTES];
int num_clientes = 0;
int servidor_fifo;

void obtener_tiempo(char* buffer) {
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    strftime(buffer, 20, "%H:%M:%S", tm_info);
}

void broadcast_mensaje(const char* mensaje, int id_remitente) {
    char mensaje_completo[TAM_BUFFER];
    char tiempo[20];
    
    obtener_tiempo(tiempo);
    snprintf(mensaje_completo, TAM_BUFFER, "[%s] %s", tiempo, mensaje);
    
    printf("DEBUG: Enviando mensaje a %d clientes: %s\n", num_clientes, mensaje_completo);
    
    for (int i = 0; i < num_clientes; i++) {
        if (clientes[i].id != id_remitente) {
            printf("DEBUG: Enviando a cliente %d (FIFO: %s)\n", clientes[i].id, clientes[i].fifo_escritura);
            int fd = open(clientes[i].fifo_escritura, O_WRONLY);
            if (fd == -1) {
                printf("DEBUG: Error abriendo FIFO %s: %s\n", clientes[i].fifo_escritura, strerror(errno));
            } else {
                ssize_t bytes = write(fd, mensaje_completo, strlen(mensaje_completo) + 1);
                printf("DEBUG: Escritos %zd bytes al cliente %d\n", bytes, clientes[i].id);
                close(fd);
            }
        }
    }
}

void manejar_senal(int sig) {
    (void)sig;
    printf("\nCerrando servidor...\n");
    
    for (int i = 0; i < num_clientes; i++) {
        int fd = open(clientes[i].fifo_escritura, O_WRONLY);
        if (fd != -1) {
            const char* mensaje_cierre = "SERVIDOR: Servidor cerrado. Hasta pronto!";
            write(fd, mensaje_cierre, strlen(mensaje_cierre) + 1);
            close(fd);
        }
    }
    
    unlink(FIFO_SERVIDOR);
    for (int i = 0; i < num_clientes; i++) {
        unlink(clientes[i].fifo_lectura);
        unlink(clientes[i].fifo_escritura);
    }
    
    close(servidor_fifo);
    exit(0);
}

int agregar_cliente(pid_t pid_cliente) {
    if (num_clientes >= MAX_CLIENTES) {
        printf("DEBUG: No se pueden agregar más clientes (límite: %d)\n", MAX_CLIENTES);
        return -1;
    }
    
    Cliente nuevo_cliente;
    nuevo_cliente.id = num_clientes + 1;
    nuevo_cliente.pid = pid_cliente;
    
    snprintf(nuevo_cliente.fifo_lectura, 50, "%s%d_lectura", FIFO_BASE_CLIENTE, nuevo_cliente.id);
    snprintf(nuevo_cliente.fifo_escritura, 50, "%s%d_escritura", FIFO_BASE_CLIENTE, nuevo_cliente.id);
    
    printf("DEBUG: Creando FIFOs para cliente %d:\n", nuevo_cliente.id);
    printf("DEBUG - Lectura: %s\n", nuevo_cliente.fifo_lectura);
    printf("DEBUG - Escritura: %s\n", nuevo_cliente.fifo_escritura);
    
    if (mkfifo(nuevo_cliente.fifo_lectura, 0666) == -1) {
        printf("DEBUG: Error creando FIFO lectura: %s\n", strerror(errno));
    }
    if (mkfifo(nuevo_cliente.fifo_escritura, 0666) == -1) {
        printf("DEBUG: Error creando FIFO escritura: %s\n", strerror(errno));
    }
    
    clientes[num_clientes] = nuevo_cliente;
    num_clientes++;
    
    printf("DEBUG: Cliente %d agregado exitosamente. Total: %d\n", nuevo_cliente.id, num_clientes);
    return nuevo_cliente.id;
}

void eliminar_cliente(int id) {
    for (int i = 0; i < num_clientes; i++) {
        if (clientes[i].id == id) {
            printf("DEBUG: Eliminando cliente %d\n", id);
            for (int j = i; j < num_clientes - 1; j++) {
                clientes[j] = clientes[j + 1];
            }
            num_clientes--;
            break;
        }
    }
}

int main() {
    char buffer[TAM_BUFFER];
    
    signal(SIGINT, manejar_senal);
    
    printf("DEBUG: Creando FIFO del servidor: %s\n", FIFO_SERVIDOR);
    if (mkfifo(FIFO_SERVIDOR, 0666) == -1) {
        printf("DEBUG: Error creando FIFO servidor: %s\n", strerror(errno));
    }
    
    printf("Servidor de chat iniciado.\n");
    printf("FIFO del servidor: %s\n", FIFO_SERVIDOR);
    printf("Esperando conexiones...\n\n");
    
    while (1) {
        printf("DEBUG: Esperando mensajes...\n");
        servidor_fifo = open(FIFO_SERVIDOR, O_RDWR);
        if (servidor_fifo == -1) {
            printf("DEBUG: Error al abrir FIFO del servidor: %s\n", strerror(errno));
            sleep(2);
            continue;
        }
        
        memset(buffer, 0, TAM_BUFFER);
        ssize_t bytes_leidos = read(servidor_fifo, buffer, TAM_BUFFER);
        
        if (bytes_leidos > 0) {
            printf("DEBUG: Mensaje recibido (%zd bytes): %s\n", bytes_leidos, buffer);
            
            char* token = strtok(buffer, "|");
            if (token != NULL) {
                pid_t pid_cliente = atoi(token);
                char* mensaje = strtok(NULL, "|");
                
                printf("DEBUG: PID Cliente: %d, Mensaje: %s\n", pid_cliente, mensaje);
                
                if (mensaje != NULL) {
                    int id_cliente = -1;
                    for (int i = 0; i < num_clientes; i++) {
                        if (clientes[i].pid == pid_cliente) {
                            id_cliente = clientes[i].id;
                            break;
                        }
                    }
                    
                    if (id_cliente == -1) {
                        printf("DEBUG: Nuevo cliente detectado (PID: %d)\n", pid_cliente);
                        id_cliente = agregar_cliente(pid_cliente);
                        if (id_cliente != -1) {
                            char mensaje_bienvenida[TAM_BUFFER];
                            snprintf(mensaje_bienvenida, TAM_BUFFER, 
                                   "Cliente %d se ha unido al chat", id_cliente);
                            printf("%s\n", mensaje_bienvenida);
                            broadcast_mensaje(mensaje_bienvenida, id_cliente);
                        }
                    }
                    
                    if (strcmp(mensaje, "/salir") == 0) {
                        char mensaje_desconexion[TAM_BUFFER];
                        snprintf(mensaje_desconexion, TAM_BUFFER, 
                               "Cliente %d ha abandonado el chat", id_cliente);
                        printf("%s\n", mensaje_desconexion);
                        broadcast_mensaje(mensaje_desconexion, id_cliente);
                        eliminar_cliente(id_cliente);
                    } else if (strlen(mensaje) > 0) {
                        char mensaje_formateado[TAM_BUFFER];
                        snprintf(mensaje_formateado, TAM_BUFFER, 
                               "Cliente %d: %s", id_cliente, mensaje);
                        printf("%s\n", mensaje_formateado);
                        broadcast_mensaje(mensaje_formateado, id_cliente);
                    }
                }
            }
        } else if (bytes_leidos == 0) {
            printf("DEBUG: Cliente cerró la conexión\n");
        } else {
            printf("DEBUG: Error leyendo: %s\n", strerror(errno));
        }
        
        close(servidor_fifo);
        sleep(1);
    }
    
    return 0;
}