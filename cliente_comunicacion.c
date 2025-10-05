#include "cliente_comunicacion.h"
#include "cliente_formatos.h"
#include <time.h>

// Variables globales (solo mi_pid)
pid_t mi_pid;

void manejar_senal(int sig) {
    (void)sig;
    printf("\nSaliendo del sistema del hospital...\n");

    // Enviar mensaje de salida al servidor
    char mensaje_salida[TAM_BUFFER];
    snprintf(mensaje_salida, TAM_BUFFER, "%d|salir", mi_pid);

    int fd = open(FIFO_SERVIDOR, O_WRONLY);
    if (fd != -1) {
        write(fd, mensaje_salida, strlen(mensaje_salida) + 1);
        close(fd);
    }
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

// FUNCIÓN PRINCIPAL 
int operacion_segura(const char* comando, const char* tipo_formato) {
    // 1. Crear FIFO temporal único
    char fifo_temp[50];
    snprintf(fifo_temp, sizeof(fifo_temp), "/tmp/hospital_%d_%ld", getpid(), time(NULL));
    
    // Eliminar si existe y crear nuevo
    unlink(fifo_temp);
    if (mkfifo(fifo_temp, 0666) == -1) {
        perror("❌ Error creando FIFO temporal");
        return 0;
    }

    // 2. Enviar comando con FIFO temporal
    char mensaje[TAM_BUFFER];
    snprintf(mensaje, sizeof(mensaje), "%d|%s|%s", getpid(), fifo_temp, comando);
    
    int fd_servidor = open(FIFO_SERVIDOR, O_WRONLY);
    if (fd_servidor == -1) {
        unlink(fifo_temp);
        printf("❌ Error: No se pudo conectar al servidor\n");
        return 0;
    }
    write(fd_servidor, mensaje, strlen(mensaje) + 1);
    close(fd_servidor);

    // 3. Esperar respuesta en FIFO temporal
    printf("⏳ Esperando respuesta del servidor...\n");
    
    int fd_respuesta = open(fifo_temp, O_RDONLY);
    if (fd_respuesta == -1) {
        unlink(fifo_temp);
        perror("❌ Error abriendo FIFO de respuesta");
        return 0;
    }

    char buffer[TAM_BUFFER];
    memset(buffer, 0, TAM_BUFFER);
    
    // Usar select con timeout
    fd_set readfds;
    struct timeval timeout;
    
    FD_ZERO(&readfds);
    FD_SET(fd_respuesta, &readfds);
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    
    int ret = select(fd_respuesta + 1, &readfds, NULL, NULL, &timeout);
    
    if (ret > 0) {
        int bytes_leidos = read(fd_respuesta, buffer, TAM_BUFFER - 1);
        close(fd_respuesta);
        unlink(fifo_temp);
        
        if (bytes_leidos > 0) {
            printf("\n════════════════════════════════════════\n");
            printf("📥 RESPUESTA DEL SERVIDOR (%d bytes):\n", bytes_leidos);
            
            // Aplicar formato
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
        printf("⏰ Timeout: No se recibió respuesta del servidor\n");
    } else {
        perror("❌ Error en select()");
    }
    
    close(fd_respuesta);
    unlink(fifo_temp);
    return 0;
}

// Función simple para operaciones que no necesitan formato especial
int operacion_simple(const char* comando) {
    return operacion_segura(comando, "simple");
}

void inicializar_comunicacion() {
    mi_pid = getpid();
    signal(SIGINT, manejar_senal);

    printf("=== CLIENTE DEL SISTEMA HOSPITAL ===\n");
    printf("PID del cliente: %d\n", mi_pid);
    printf("🟢 Cliente listo para operar\n\n");
}

void limpiar_comunicacion() {
    printf("🔒 Cerrando cliente...\n");
    // No hay FIFO persistente que limpiar
}