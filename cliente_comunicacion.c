#include "cliente_comunicacion.h"
#include "cliente_formatos.h"

// Variables globales
int cliente_fifo_respuesta;
char fifo_respuesta[50];
pid_t mi_pid;

void manejar_senal(int sig) {
    (void)sig;
    printf("\nSaliendo del sistema del hospital...\n");

    char mensaje_salida[TAM_BUFFER];
    snprintf(mensaje_salida, TAM_BUFFER, "%d|salir", mi_pid);

    int fd = open(FIFO_SERVIDOR, O_WRONLY);
    if (fd != -1) {
        write(fd, mensaje_salida, strlen(mensaje_salida) + 1);
        close(fd);
    }

    if (cliente_fifo_respuesta > 0) {
        close(cliente_fifo_respuesta);
    }
    unlink(fifo_respuesta);
    exit(0);
}

void enviar_comando(const char* comando) {
    char mensaje[TAM_BUFFER];
    snprintf(mensaje, sizeof(mensaje), "%d|%s", mi_pid, comando);
    
    int fd = open(FIFO_SERVIDOR, O_WRONLY);
    if (fd != -1) {
        write(fd, mensaje, strlen(mensaje) + 1);
        close(fd);
    } else {
        printf("❌ Error: No se pudo conectar al servidor\n");
    }
}

int esperar_respuesta_con_timeout(int timeout_sec) {
    printf("⏳ Esperando respuesta del servidor...\n");
    
    char buffer[TAM_BUFFER];
    memset(buffer, 0, TAM_BUFFER);
    
    fd_set readfds;
    struct timeval timeout;
    
    FD_ZERO(&readfds);
    FD_SET(cliente_fifo_respuesta, &readfds);
    timeout.tv_sec = timeout_sec;
    timeout.tv_usec = 0;
    
    int ret = select(cliente_fifo_respuesta + 1, &readfds, NULL, NULL, &timeout);
    
    if (ret > 0) {
        int bytes_leidos = read(cliente_fifo_respuesta, buffer, TAM_BUFFER - 1);
        if (bytes_leidos > 0 && strlen(buffer) > 0) {
            printf("\n════════════════════════════════════════\n");
            printf("📥 RESPUESTA DEL SERVIDOR:\n");
            printf("%s\n", buffer);
            printf("════════════════════════════════════════\n");
            return 1;
        }
    } else if (ret == 0) {
        printf("⏰ Timeout: No se recibió respuesta del servidor\n");
    } else {
        printf("❌ Error en select()\n");
    }
    
    return 0;
}

int esperar_respuesta_formateada(int timeout_sec, const char* tipo_formato) {
    printf("⏳ Esperando respuesta del servidor (%d segundos)...\n", timeout_sec);
    
    char buffer[TAM_BUFFER];
    memset(buffer, 0, TAM_BUFFER);
    
    fd_set readfds;
    struct timeval timeout;
    
    FD_ZERO(&readfds);
    FD_SET(cliente_fifo_respuesta, &readfds);
    timeout.tv_sec = timeout_sec;
    timeout.tv_usec = 0;
    
    int ret = select(cliente_fifo_respuesta + 1, &readfds, NULL, NULL, &timeout);
    
    if (ret > 0) {
        memset(buffer, 0, TAM_BUFFER);
        int bytes_leidos = read(cliente_fifo_respuesta, buffer, TAM_BUFFER - 1);
        
        if (bytes_leidos > 0) {
            printf("\n════════════════════════════════════════\n");
            printf("📥 RESPUESTA DEL SERVIDOR (%d bytes):\n", bytes_leidos);
            
            if (strcmp(tipo_formato, "pacientes") == 0) {
                mostrar_lista_compacta_pacientes(buffer);
            }
            else if (strcmp(tipo_formato, "medicos") == 0) {
                mostrar_lista_compacta_medicos(buffer);
            }
            else if (strcmp(tipo_formato, "medicamentos") == 0) {
                mostrar_lista_compacta_medicamentos(buffer);
            }
            else if (strcmp(tipo_formato, "recetas") == 0) {
                mostrar_lista_compacta_recetas(buffer);
            }
            else if (strcmp(tipo_formato, "detalle") == 0) {
                const char* titulo = "DETALLES";
                if (strstr(buffer, "PACIENTE") != NULL || strstr(buffer, "paciente") != NULL) 
                    titulo = "👤 DETALLES DEL PACIENTE";
                else if (strstr(buffer, "MÉDICO") != NULL || strstr(buffer, "médico") != NULL) 
                    titulo = "👨‍⚕️ DETALLES DEL MÉDICO";
                else if (strstr(buffer, "MEDICAMENTO") != NULL || strstr(buffer, "medicamento") != NULL) 
                    titulo = "💊 DETALLES DEL MEDICAMENTO";
                else if (strstr(buffer, "RECETA") != NULL || strstr(buffer, "receta") != NULL) 
                    titulo = "📄 DETALLES DE LA RECETA";
                
                mostrar_detalle_compacto(buffer, titulo);
            }
            else {
                printf("%s\n", buffer);
            }
            
            printf("════════════════════════════════════════\n");
            return 1;
        }
    } else if (ret == 0) {
        printf("⏰ Timeout: No se recibió respuesta del servidor en %d segundos\n", timeout_sec);
    }
    
    return 0;
}

void inicializar_comunicacion() {
    mi_pid = getpid();
    signal(SIGINT, manejar_senal);

    printf("=== CLIENTE DEL SISTEMA HOSPITAL ===\n");
    printf("PID del cliente: %d\n", mi_pid);

    snprintf(fifo_respuesta, 50, "/tmp/hospital_cliente_%d_respuesta", mi_pid);
    unlink(fifo_respuesta);
    
    if (mkfifo(fifo_respuesta, 0666) == -1) {
        perror("❌ Error al crear FIFO de respuesta");
        exit(1);
    }

    printf("✅ FIFO de respuesta creado: %s\n", fifo_respuesta);
    printf("Abriendo FIFO para lectura (modo no bloqueante)...\n");
    
    cliente_fifo_respuesta = open(fifo_respuesta, O_RDONLY | O_NONBLOCK);
    if (cliente_fifo_respuesta == -1) {
        perror("❌ Error al abrir FIFO de respuesta");
        exit(1);
    }
    
    printf("✅ FIFO abierto correctamente (fd: %d)\n", cliente_fifo_respuesta);
    printf("🟢 Cliente listo para operar\n\n");
}

void limpiar_comunicacion() {
    printf("🔒 Cerrando cliente...\n");
    if (cliente_fifo_respuesta > 0) {
        close(cliente_fifo_respuesta);
    }
    unlink(fifo_respuesta);
}