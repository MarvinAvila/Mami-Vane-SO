#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>

#define MAX_CLIENTES 2  // ✅ Límite REAL de clientes
#define TAM_BUFFER 2048
#define FIFO_SERVIDOR "/tmp/hospital_servidor"

// Incluir headers de tu CRUD
#include "database.h"
#include "paciente.h"
#include "medico.h"
#include "medicamento.h"
#include "receta.h"

typedef struct {
    pid_t pid;
    char fifo_respuesta[50];
    time_t ultima_conexion;  // ✅ Para limpieza de clientes inactivos
} Cliente;

Cliente clientes[MAX_CLIENTES];
int num_clientes = 0;

// ✅ FUNCIÓN: Buscar cliente por PID
int buscar_cliente_por_pid(pid_t pid) {
    for (int i = 0; i < num_clientes; i++) {
        if (clientes[i].pid == pid) {
            return i;  // Encontrado
        }
    }
    return -1;  // No encontrado
}

// ✅ FUNCIÓN: Registrar nuevo cliente
int registrar_cliente(pid_t pid, const char *fifo_respuesta) {
    if (num_clientes >= MAX_CLIENTES) {
        return -1;  // ❌ Límite alcanzado
    }
    
    // Verificar si el cliente ya está registrado
    if (buscar_cliente_por_pid(pid) != -1) {
        return 0;  // ✅ Ya registrado, no hay problema
    }
    
    // Registrar nuevo cliente
    clientes[num_clientes].pid = pid;
    strcpy(clientes[num_clientes].fifo_respuesta, fifo_respuesta);
    clientes[num_clientes].ultima_conexion = time(NULL);
    num_clientes++;
    
    printf("✅ Nuevo cliente registrado: PID %d (Total: %d/%d)\n", pid, num_clientes, MAX_CLIENTES);
    return 1;  // ✅ Nuevo cliente registrado
}

// ✅ FUNCIÓN: Eliminar cliente (por desconexión)
void eliminar_cliente(pid_t pid) {
    int index = buscar_cliente_por_pid(pid);
    if (index == -1) return;
    
    printf("🔌 Cliente desconectado: PID %d\n", pid);
    
    // Mover los clientes posteriores una posición hacia atrás
    for (int i = index; i < num_clientes - 1; i++) {
        clientes[i] = clientes[i + 1];
    }
    num_clientes--;
    
    printf("📊 Clientes conectados: %d/%d\n", num_clientes, MAX_CLIENTES);
}

// ✅ FUNCIÓN: Limpiar clientes inactivos
void limpiar_clientes_inactivos() {
    time_t ahora = time(NULL);
    for (int i = num_clientes - 1; i >= 0; i--) {
        // Si el cliente no se ha comunicado en 5 minutos, se considera desconectado
        if (ahora - clientes[i].ultima_conexion > 300) {  // 300 segundos = 5 minutos
            printf("🕒 Eliminando cliente inactivo: PID %d\n", clientes[i].pid);
            eliminar_cliente(clientes[i].pid);
        }
    }
}

void manejar_senal(int sig) {
    (void)sig;
    printf("\nServidor del Hospital apagado.\n");
    printf("📊 Clientes conectados al apagar: %d/%d\n", num_clientes, MAX_CLIENTES);
    desconectar_base_datos();
    unlink(FIFO_SERVIDOR);
    exit(0);
}

// FUNCIÓN: acepta FIFO temporal como parámetro
void enviar_respuesta(const char *fifo_respuesta, const char *respuesta) {
    int fd = open(fifo_respuesta, O_WRONLY);
    if (fd != -1) {
        write(fd, respuesta, strlen(respuesta) + 1);
        close(fd);
    } else {
        printf("❌ Error: No se pudo enviar respuesta al FIFO: %s\n", fifo_respuesta);
    }
}

void mostrar_ayuda_completa() {
    printf("\n=== SISTEMA HOSPITAL - COMANDOS DISPONIBLES ===\n\n");
    
    printf("📋 PACIENTES:\n");
    printf("  crear_paciente|nombre|apellidos|fecha_nac|direccion|peso|estatura|sexo|num_ced\n");
    printf("  listar_pacientes\n");
    printf("  buscar_paciente|id\n");
    printf("  actualizar_paciente|id|campo|nuevo_valor\n");
    printf("  eliminar_paciente|id\n\n");
    
    printf("👨‍⚕️ MÉDICOS:\n");
    printf("  crear_medico|num_ced|nombre|apellidos|especialidad\n");
    printf("  listar_medicos\n");
    printf("  buscar_medico|num_ced\n");
    printf("  actualizar_medico|num_ced|campo|nuevo_valor\n");
    printf("  eliminar_medico|num_ced\n\n");
    
    printf("💊 MEDICAMENTOS:\n");
    printf("  crear_medicamento|nombre|descripcion|presentacion|fecha_caducidad|stock\n");
    printf("  listar_medicamentos\n");
    printf("  buscar_medicamento|id\n");
    printf("  actualizar_medicamento|id|campo|nuevo_valor\n");
    printf("  eliminar_medicamento|id\n\n");
    
    printf("📄 RECETAS:\n");
    printf("  crear_receta|num_exp|num_ced|fecha_receta\n");
    printf("  listar_recetas\n");
    printf("  buscar_receta|folio\n");
    printf("  agregar_medicamento_receta|folio|num_med|cantidad\n");
    printf("  listar_medicamentos_receta|folio\n");
    printf("  eliminar_receta|folio\n\n");
    
    printf("❓ OTROS:\n");
    printf("  ayuda\n");
    printf("  salir\n\n");
    
    printf("📝 CAMPOS PARA ACTUALIZAR:\n");
    printf("  Pacientes: 1=Nombre, 2=Apellidos, 3=Fecha Nac, 4=Dirección, 5=Peso, 6=Estatura, 7=Sexo\n");
    printf("  Médicos: 1=Nombre, 2=Apellidos, 3=Especialidad\n");
    printf("  Medicamentos: 1=Nombre, 2=Descripción, 3=Presentación, 4=Fecha Caducidad, 5=Stock\n");
}

// FUNCIÓN: recibe el FIFO de respuesta como parámetro
void procesar_comando(const char *fifo_respuesta, const char *comando_completo) {
    char comando[100];
    char parametros[900];
    
    // Separar comando y parámetros
    if (sscanf(comando_completo, "%99[^|]|%899[^\n]", comando, parametros) < 1) {
        enviar_respuesta(fifo_respuesta, "❌ Error: Formato de comando inválido. Use 'ayuda' para ver comandos disponibles.");
        return;
    }
    
    printf("Procesando comando: %s\n", comando);
    
    char respuesta[TAM_BUFFER];
    respuesta[0] = '\0';
    
    // ==================== PACIENTES ====================
    if (strcmp(comando, "crear_paciente") == 0) {
        char* resultado = crear_paciente_fifo(parametros);
        strncpy(respuesta, resultado, TAM_BUFFER - 1);
    }
    else if (strcmp(comando, "listar_pacientes") == 0) {
        char* resultado = listar_pacientes_fifo();
        strncpy(respuesta, resultado, TAM_BUFFER - 1);
    }
    else if (strcmp(comando, "buscar_paciente") == 0) {
        int id;
        if (sscanf(parametros, "%d", &id) == 1) {
            char* resultado = buscar_paciente_fifo(id);
            strncpy(respuesta, resultado, TAM_BUFFER - 1);
        } else {
            strcpy(respuesta, "❌ Error: ID de paciente inválido");
        }
    }
    else if (strcmp(comando, "actualizar_paciente") == 0) {
        int id, campo;
        char nuevo_valor[200];
        if (sscanf(parametros, "%d|%d|%199[^\n]", &id, &campo, nuevo_valor) == 3) {
            char* resultado = actualizar_paciente_fifo(id, campo, nuevo_valor);
            strncpy(respuesta, resultado, TAM_BUFFER - 1);
        } else {
            strcpy(respuesta, "❌ Error: Parámetros inválidos para actualizar paciente");
        }
    }
    else if (strcmp(comando, "eliminar_paciente") == 0) {
        int id;
        if (sscanf(parametros, "%d", &id) == 1) {
            char* resultado = eliminar_paciente_fifo(id);
            strncpy(respuesta, resultado, TAM_BUFFER - 1);
        } else {
            strcpy(respuesta, "❌ Error: ID de paciente inválido");
        }
    }
    
    // ==================== MÉDICOS ====================
    else if (strcmp(comando, "crear_medico") == 0) {
        char* resultado = crear_medico_fifo(parametros);
        strncpy(respuesta, resultado, TAM_BUFFER - 1);
    }
    else if (strcmp(comando, "listar_medicos") == 0) {
        char* resultado = listar_medicos_fifo();
        strncpy(respuesta, resultado, TAM_BUFFER - 1);
    }
    else if (strcmp(comando, "buscar_medico") == 0) {
        int num_ced;
        if (sscanf(parametros, "%d", &num_ced) == 1) {
            char* resultado = buscar_medico_fifo(num_ced);
            strncpy(respuesta, resultado, TAM_BUFFER - 1);
        } else {
            strcpy(respuesta, "❌ Error: Número de cédula inválido");
        }
    }
    else if (strcmp(comando, "actualizar_medico") == 0) {
        int num_ced, campo;
        char nuevo_valor[200];
        if (sscanf(parametros, "%d|%d|%199[^\n]", &num_ced, &campo, nuevo_valor) == 3) {
            char* resultado = actualizar_medico_fifo(num_ced, campo, nuevo_valor);
            strncpy(respuesta, resultado, TAM_BUFFER - 1);
        } else {
            strcpy(respuesta, "❌ Error: Parámetros inválidos para actualizar médico");
        }
    }
    else if (strcmp(comando, "eliminar_medico") == 0) {
        int num_ced;
        if (sscanf(parametros, "%d", &num_ced) == 1) {
            char* resultado = eliminar_medico_fifo(num_ced);
            strncpy(respuesta, resultado, TAM_BUFFER - 1);
        } else {
            strcpy(respuesta, "❌ Error: Número de cédula inválido");
        }
    }
    
    // ==================== MEDICAMENTOS ====================
    else if (strcmp(comando, "crear_medicamento") == 0) {
        char* resultado = crear_medicamento_fifo(parametros);
        strncpy(respuesta, resultado, TAM_BUFFER - 1);
    }
    else if (strcmp(comando, "listar_medicamentos") == 0) {
        char* resultado = listar_medicamentos_fifo();
        strncpy(respuesta, resultado, TAM_BUFFER - 1);
    }
    else if (strcmp(comando, "buscar_medicamento") == 0) {
        int id;
        if (sscanf(parametros, "%d", &id) == 1) {
            char* resultado = buscar_medicamento_fifo(id);
            strncpy(respuesta, resultado, TAM_BUFFER - 1);
        } else {
            strcpy(respuesta, "❌ Error: ID de medicamento inválido");
        }
    }
    else if (strcmp(comando, "actualizar_medicamento") == 0) {
        int id, campo;
        char nuevo_valor[200];
        if (sscanf(parametros, "%d|%d|%199[^\n]", &id, &campo, nuevo_valor) == 3) {
            char* resultado = actualizar_medicamento_fifo(id, campo, nuevo_valor);
            strncpy(respuesta, resultado, TAM_BUFFER - 1);
        } else {
            strcpy(respuesta, "❌ Error: Parámetros inválidos para actualizar medicamento");
        }
    }
    else if (strcmp(comando, "eliminar_medicamento") == 0) {
        int id;
        if (sscanf(parametros, "%d", &id) == 1) {
            char* resultado = eliminar_medicamento_fifo(id);
            strncpy(respuesta, resultado, TAM_BUFFER - 1);
        } else {
            strcpy(respuesta, "❌ Error: ID de medicamento inválido");
        }
    }
    
    // ==================== RECETAS ====================
    else if (strcmp(comando, "crear_receta") == 0) {
        char* resultado = crear_receta_fifo(parametros);
        strncpy(respuesta, resultado, TAM_BUFFER - 1);
    }
    else if (strcmp(comando, "listar_recetas") == 0) {
        char* resultado = listar_recetas_fifo();
        strncpy(respuesta, resultado, TAM_BUFFER - 1);
    }
    else if (strcmp(comando, "buscar_receta") == 0) {
        int folio;
        if (sscanf(parametros, "%d", &folio) == 1) {
            char* resultado = buscar_receta_fifo(folio);
            strncpy(respuesta, resultado, TAM_BUFFER - 1);
        } else {
            strcpy(respuesta, "❌ Error: Folio de receta inválido");
        }
    }
    else if (strcmp(comando, "agregar_medicamento_receta") == 0) {
        int folio, num_med, cantidad;
        if (sscanf(parametros, "%d|%d|%d", &folio, &num_med, &cantidad) == 3) {
            char* resultado = agregar_medicamento_receta_fifo(folio, num_med, cantidad);
            strncpy(respuesta, resultado, TAM_BUFFER - 1);
        } else {
            strcpy(respuesta, "❌ Error: Parámetros inválidos para agregar medicamento a receta");
        }
    }
    else if (strcmp(comando, "listar_medicamentos_receta") == 0) {
        int folio;
        if (sscanf(parametros, "%d", &folio) == 1) {
            char* resultado = listar_medicamentos_receta_fifo(folio);
            strncpy(respuesta, resultado, TAM_BUFFER - 1);
        } else {
            strcpy(respuesta, "❌ Error: Folio de receta inválido");
        }
    }
    else if (strcmp(comando, "eliminar_receta") == 0) {
        int folio;
        if (sscanf(parametros, "%d", &folio) == 1) {
            char* resultado = eliminar_receta_fifo(folio);
            strncpy(respuesta, resultado, TAM_BUFFER - 1);
        } else {
            strcpy(respuesta, "❌ Error: Folio de receta inválido");
        }
    }
    
    // ==================== OTROS COMANDOS ====================
    else if (strcmp(comando, "ayuda") == 0) {
        // Crear respuesta de ayuda
        char ayuda[TAM_BUFFER];
        ayuda[0] = '\0';
        
        strcpy(ayuda, "=== SISTEMA HOSPITAL - COMANDOS DISPONIBLES ===\n\n");
        
        strcat(ayuda, "📋 PACIENTES:\n");
        strcat(ayuda, "  crear_paciente|nombre|apellidos|fecha_nac|direccion|peso|estatura|sexo|num_ced\n");
        strcat(ayuda, "  listar_pacientes, buscar_paciente|id, actualizar_paciente|id|campo|valor, eliminar_paciente|id\n\n");
        
        strcat(ayuda, "👨‍⚕️ MÉDICOS:\n");
        strcat(ayuda, "  crear_medico|cedula|nombre|apellidos|especialidad\n");
        strcat(ayuda, "  listar_medicos, buscar_medico|cedula, actualizar_medico|cedula|campo|valor, eliminar_medico|cedula\n\n");
        
        strcat(ayuda, "💊 MEDICAMENTOS:\n");
        strcat(ayuda, "  crear_medicamento|nombre|descripcion|presentacion|fecha_caducidad|stock\n");
        strcat(ayuda, "  listar_medicamentos, buscar_medicamento|id, actualizar_medicamento|id|campo|valor, eliminar_medicamento|id\n\n");
        
        strcat(ayuda, "📄 RECETAS:\n");
        strcat(ayuda, "  crear_receta|expediente|cedula|fecha, listar_recetas, buscar_receta|folio\n");
        strcat(ayuda, "  agregar_medicamento_receta|folio|medicamento|cantidad, listar_medicamentos_receta|folio, eliminar_receta|folio\n\n");
        
        strcat(ayuda, "❓ OTROS: ayuda, salir\n");
        
        strcpy(respuesta, ayuda);
    }
    else if (strcmp(comando, "salir") == 0) {
        strcpy(respuesta, "👋 Desconectado del sistema del hospital. ¡Hasta pronto!");
    }
    else {
        snprintf(respuesta, sizeof(respuesta), 
                 "❌ Comando no reconocido: '%s'\n\nUse 'ayuda' para ver todos los comandos disponibles.", comando);
    }
    
    // Asegurar que la respuesta no esté vacía
    if (strlen(respuesta) == 0) {
        strcpy(respuesta, "✅ Comando procesado correctamente");
    }
    
    enviar_respuesta(fifo_respuesta, respuesta);
}

int main() {
    char buffer[TAM_BUFFER];

    signal(SIGINT, manejar_senal);

    // Crear FIFO del servidor
    mkfifo(FIFO_SERVIDOR, 0666);

    printf("=== SERVICIO HOSPITAL - SERVIDOR INICIADO ===\n");
    printf("FIFO del servidor: %s\n", FIFO_SERVIDOR);
    printf("🔒 Límite de clientes: %d\n", MAX_CLIENTES);
    
    // Conectar a la base de datos al iniciar
    printf("Conectando a la base de datos...\n");
    PGconn *conn = conectar_base_datos();
    if (conn != NULL && PQstatus(conn) == CONNECTION_OK) {
        printf("✅ Conectado a la base de datos correctamente\n");
    } else {
        printf("❌ Error al conectar a la base de datos\n");
        return 1;
    }
    
    mostrar_ayuda_completa();
    printf("\n🟢 Servidor listo. Esperando comandos de clientes...\n\n");

    while (1) {
        int fd_servidor = open(FIFO_SERVIDOR, O_RDONLY);
        if (fd_servidor == -1) {
            perror("Error al abrir FIFO del servidor");
            sleep(1);
            continue;
        }

        memset(buffer, 0, TAM_BUFFER);
        int bytes_leidos = read(fd_servidor, buffer, TAM_BUFFER - 1);
        
        if (bytes_leidos > 0) {
            pid_t pid;
            char fifo_respuesta_temp[50];
            char comando[TAM_BUFFER];
            
            // ✅ CAMBIO PRINCIPAL: Nuevo formato PID|FIFO_TEMP|COMANDO con control de clientes
            if (sscanf(buffer, "%d|%49[^|]|%1023[^\n]", &pid, fifo_respuesta_temp, comando) == 3) {
                printf("📨 Comando recibido de PID %d via FIFO: %s\n", pid, fifo_respuesta_temp);
                
                // ✅ VERIFICAR Y REGISTRAR CLIENTE
                int resultado_registro = registrar_cliente(pid, fifo_respuesta_temp);
                
                if (resultado_registro == -1) {
                    // ❌ LÍMITE DE CLIENTES ALCANZADO
                    printf("❌ Límite de clientes alcanzado. Rechazando PID %d\n", pid);
                    char respuesta_error[TAM_BUFFER];
                    snprintf(respuesta_error, sizeof(respuesta_error),
                             "❌ Servidor lleno. Límite de %d clientes alcanzado. Intente más tarde.", 
                             MAX_CLIENTES);
                    enviar_respuesta(fifo_respuesta_temp, respuesta_error);
                } else {
                    // ✅ CLIENTE ACEPTADO - Actualizar tiempo de conexión
                    int index = buscar_cliente_por_pid(pid);
                    if (index != -1) {
                        clientes[index].ultima_conexion = time(NULL);
                    }
                    
                    // Procesar el comando
                    procesar_comando(fifo_respuesta_temp, comando);
                    
                    // ✅ Verificar si es comando de salida para eliminar cliente
                    if (strncmp(comando, "salir", 5) == 0) {
                        eliminar_cliente(pid);
                    }
                }
            } 
            // ✅ Compatibilidad con formato antiguo (por si acaso)
            else if (sscanf(buffer, "%d|%1023[^\n]", &pid, comando) == 2) {
                printf("⚠️  Formato antiguo detectado de PID %d\n", pid);
                int index = buscar_cliente_por_pid(pid);
                if (index != -1) {
                    clientes[index].ultima_conexion = time(NULL);
                    procesar_comando(clientes[index].fifo_respuesta, comando);
                    
                    if (strncmp(comando, "salir", 5) == 0) {
                        eliminar_cliente(pid);
                    }
                } else {
                    printf("❌ Cliente no registrado: %d\n", pid);
                    // No respondemos porque no tenemos su FIFO
                }
            } else {
                printf("❌ Error: No se pudo parsear el comando: %s\n", buffer);
            }
        }
        close(fd_servidor);

        // ✅ LIMPIAR CLIENTES INACTIVOS CADA 10 ITERACIONES
        static int contador_limpieza = 0;
        if (++contador_limpieza >= 10) {
            limpiar_clientes_inactivos();
            contador_limpieza = 0;
        }

        // Pequeña pausa para no saturar la CPU
        struct timespec ts = {0, 100 * 1000000L}; // 100ms
        nanosleep(&ts, NULL);
    }

    return 0;
}