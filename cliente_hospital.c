#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/wait.h> 

#define TAM_BUFFER 2048
#define FIFO_SERVIDOR "/tmp/hospital_servidor"

int cliente_fifo_respuesta;
char fifo_respuesta[50];
pid_t mi_pid;

// ===============================
// FUNCIONES DE MANEJO DE SEÑALES Y COMUNICACIÓN
// ===============================

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
        printf("📤 Comando enviado: %s\n", comando);
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

// ===============================
// FUNCIONES DE FORMATEO COMPACTO
// ===============================

void mostrar_lista_compacta_pacientes(const char* respuesta_servidor) {
    printf("\n");
    printf("┌────────────────────────────────────────────────────────────────────────────────────┐\n");
    printf("│                                📋 LISTA DE PACIENTES                               │\n");
    printf("├─────┬────────────────────┬────────────┬──────────┬────────┬──────────┬─────────────┤\n");
    printf("│ ID  │ Nombre Completo    │ F.Nacimiento│ Peso(kg)│ Estatura│ Sexo │ Dirección      │\n");
    printf("├─────┼────────────────────┼────────────┼──────────┼────────┼──────────┼─────────────┤\n");
    
    char* copia = strdup(respuesta_servidor);
    char* linea = strtok(copia, "\n");
    int pacientes_mostrados = 0;
    
    while (linea != NULL) {
        if (strstr(linea, "===") != NULL || strstr(linea, "LISTA") != NULL || 
            strlen(linea) < 10 || strstr(linea, "No hay") != NULL) {
            linea = strtok(NULL, "\n");
            continue;
        }
        
        if (strstr(linea, "ID:") != NULL) {
            int id = 0;
            char nombre[50] = "", apellidos[50] = "", nombre_completo[30] = "";
            char fecha_nac[20] = "", direccion[30] = "", direccion_corta[15] = "";
            char sexo = ' ';
            float peso = 0.0, estatura = 0.0;
            
            char linea_copia[500];
            strcpy(linea_copia, linea);
            
            // ID
            char* token = strstr(linea_copia, "ID: ");
            if (token) sscanf(token + 4, "%d", &id);
            
            // Nombre
            token = strstr(linea_copia, "Nombre: ");
            if (token) {
                sscanf(token + 8, "%49[^,]", nombre);
                if (nombre[0] == '"' && strlen(nombre) > 2) {
                    memmove(nombre, nombre + 1, strlen(nombre));
                    nombre[strlen(nombre) - 1] = '\0';
                }
            }
            
            // Apellidos
            token = strstr(linea_copia, "Apellidos: ");
            if (token) {
                sscanf(token + 11, "%49[^,]", apellidos);
                if (apellidos[0] == '"' && strlen(apellidos) > 2) {
                    memmove(apellidos, apellidos + 1, strlen(apellidos));
                    apellidos[strlen(apellidos) - 1] = '\0';
                }
            }
            
            // Fecha Nacimiento
            token = strstr(linea_copia, "Fecha Nac: ");
            if (token) sscanf(token + 11, "%19[^,]", fecha_nac);
            
            // Dirección
            token = strstr(linea_copia, "Dirección: ");
            if (token) {
                sscanf(token + 11, "%29[^,]", direccion);
                strncpy(direccion_corta, direccion, sizeof(direccion_corta) - 1);
                direccion_corta[sizeof(direccion_corta) - 1] = '\0';
                if (strlen(direccion_corta) > 11) {
                    direccion_corta[8] = direccion_corta[9] = direccion_corta[10] = '.';
                    direccion_corta[11] = '\0';
                }
            }
            
            // Peso
            token = strstr(linea_copia, "Peso: ");
            if (token) sscanf(token + 6, "%f", &peso);
            
            // Estatura
            token = strstr(linea_copia, "Estatura: ");
            if (token) sscanf(token + 10, "%f", &estatura);
            
            // Sexo
            token = strstr(linea_copia, "Sexo: ");
            if (token) sscanf(token + 6, " %c", &sexo);
            
            // Crear nombre completo (manteniendo la versión que funciona)
            snprintf(nombre_completo, sizeof(nombre_completo), "%.15s %.15s", nombre, apellidos);
            if (strlen(nombre_completo) > 18) {
                nombre_completo[15] = nombre_completo[16] = nombre_completo[17] = '.';
                nombre_completo[18] = '\0';
            }
            
            printf("│ %3d │ %-18s │ %-10s │ %7.1f  │ %6.2f  │   %c   │ %-11s │\n", 
                   id, nombre_completo, fecha_nac, peso, estatura, sexo, direccion_corta);
            pacientes_mostrados++;
        }
        
        linea = strtok(NULL, "\n");
    }
    
    if (pacientes_mostrados == 0) {
        printf("│                               No hay pacientes registrados                            │\n");
    }
    
    printf("└─────┴────────────────────┴────────────┴──────────┴────────┴──────────┴─────────────┘\n");
    free(copia);
}

void mostrar_lista_compacta_medicos(const char* respuesta_servidor) {
    printf("\n");
    printf("┌──────────────────────────────────────────────────────────────────┐\n");
    printf("│                         👨‍⚕️ LISTA DE MÉDICOS                     │\n");
    printf("├─────────┬────────────────────┬───────────────────────────────────┤\n");
    printf("│ Cédula  │ Nombre Completo    │ Especialidad                      │\n");
    printf("├─────────┼────────────────────┼───────────────────────────────────┤\n");
    
    char* copia = strdup(respuesta_servidor);
    char* linea = strtok(copia, "\n");
    int medicos_mostrados = 0;
    
    while (linea != NULL) {
        if (strstr(linea, "===") != NULL || strstr(linea, "LISTA") != NULL || 
            strlen(linea) < 10 || strstr(linea, "No hay") != NULL) {
            linea = strtok(NULL, "\n");
            continue;
        }
        
        if (strstr(linea, "Cédula:") != NULL) {
            int cedula = 0;
            char nombre[50] = "", apellidos[50] = "", nombre_completo[25] = "";
            char especialidad[40] = "", especialidad_corta[35] = "";
            
            char linea_copia[500];
            strcpy(linea_copia, linea);
            
            // Cédula
            char* token = strstr(linea_copia, "Cédula: ");
            if (token) sscanf(token + 8, "%d", &cedula);
            
            // Nombre
            token = strstr(linea_copia, "Nombre: ");
            if (token) {
                sscanf(token + 8, "%49[^,]", nombre);
                if (nombre[0] == '"') {
                    memmove(nombre, nombre + 1, strlen(nombre));
                    nombre[strlen(nombre) - 1] = '\0';
                }
            }
            
            // Apellidos
            token = strstr(linea_copia, "Apellidos: ");
            if (token) {
                sscanf(token + 11, "%49[^,]", apellidos);
                if (apellidos[0] == '"') {
                    memmove(apellidos, apellidos + 1, strlen(apellidos));
                    apellidos[strlen(apellidos) - 1] = '\0';
                }
            }
            
            // Especialidad
            token = strstr(linea_copia, "Especialidad: ");
            if (token) {
                sscanf(token + 14, "%39[^\n]", especialidad);
                strncpy(especialidad_corta, especialidad, sizeof(especialidad_corta) - 1);
                especialidad_corta[sizeof(especialidad_corta) - 1] = '\0';
                if (strlen(especialidad_corta) > 33) {
                    especialidad_corta[30] = especialidad_corta[31] = especialidad_corta[32] = '.';
                    especialidad_corta[33] = '\0';
                }
            }
            
            // Nombre completo
            snprintf(nombre_completo, sizeof(nombre_completo), "%.15s %.15s", nombre, apellidos);
            if (strlen(nombre_completo) > 18) {
                nombre_completo[15] = nombre_completo[16] = nombre_completo[17] = '.';
                nombre_completo[18] = '\0';
            }
            
            printf("│ %7d │ %-18s │ %-33s │\n", cedula, nombre_completo, especialidad_corta);
            medicos_mostrados++;
        }
        
        linea = strtok(NULL, "\n");
    }
    
    if (medicos_mostrados == 0) {
        printf("│                         No hay médicos registrados                         │\n");
    }
    
    printf("└─────────┴────────────────────┴───────────────────────────────────┘\n");
    free(copia);
}

void mostrar_lista_compacta_medicamentos(const char* respuesta_servidor) {
    printf("\n");
    printf("┌──────────────────────────────────────────────────────────────────────────────────┐\n");
    printf("│                             💊 LISTA DE MEDICAMENTOS                             │\n");
    printf("├─────┬────────────────────┬──────────────────────┬──────────────┬─────────────────┤\n");
    printf("│ ID  │ Nombre             │ Presentación         │ Caducidad    │ Stock           │\n");
    printf("├─────┼────────────────────┼──────────────────────┼──────────────┼─────────────────┤\n");
    
    char* copia = strdup(respuesta_servidor);
    char* linea = strtok(copia, "\n");
    int medicamentos_mostrados = 0;
    
    while (linea != NULL) {
        if (strstr(linea, "===") != NULL || strstr(linea, "LISTA") != NULL || 
            strlen(linea) < 10 || strstr(linea, "No hay") != NULL) {
            linea = strtok(NULL, "\n");
            continue;
        }
        
        if (strstr(linea, "ID:") != NULL && strstr(linea, "Medicamento") == NULL) {
            int id = 0, stock = 0;
            char nombre[50] = "", presentacion[30] = "", caducidad[20] = "";
            char nombre_corto[20] = "", presentacion_corta[22] = "";
            
            char linea_copia[500];
            strcpy(linea_copia, linea);
            
            // ID
            char* token = strstr(linea_copia, "ID: ");
            if (token) sscanf(token + 4, "%d", &id);
            
            // Nombre
            token = strstr(linea_copia, "Nombre: ");
            if (token) {
                sscanf(token + 8, "%49[^,]", nombre);
                strncpy(nombre_corto, nombre, sizeof(nombre_corto) - 1);
                nombre_corto[sizeof(nombre_corto) - 1] = '\0';
                if (strlen(nombre_corto) > 18) {
                    nombre_corto[15] = nombre_corto[16] = nombre_corto[17] = '.';
                    nombre_corto[18] = '\0';
                }
            }
            
            // Presentación
            token = strstr(linea_copia, "Presentación: ");
            if (token) {
                sscanf(token + 14, "%29[^,]", presentacion);
                strncpy(presentacion_corta, presentacion, sizeof(presentacion_corta) - 1);
                presentacion_corta[sizeof(presentacion_corta) - 1] = '\0';
                if (strlen(presentacion_corta) > 20) {
                    presentacion_corta[17] = presentacion_corta[18] = presentacion_corta[19] = '.';
                    presentacion_corta[20] = '\0';
                }
            }
            
            // Caducidad
            token = strstr(linea_copia, "Caducidad: ");
            if (token) sscanf(token + 11, "%19[^,]", caducidad);
            
            // Stock
            token = strstr(linea_copia, "Stock: ");
            if (token) sscanf(token + 7, "%d", &stock);
            
            printf("│ %3d │ %-18s │ %-20s │ %-12s │ %8d       │\n", 
                   id, nombre_corto, presentacion_corta, caducidad, stock);
            medicamentos_mostrados++;
        }
        
        linea = strtok(NULL, "\n");
    }
    
    if (medicamentos_mostrados == 0) {
        printf("│                          No hay medicamentos registrados                         │\n");
    }
    
    printf("└─────┴────────────────────┴──────────────────────┴──────────────┴─────────────────┘\n");
    free(copia);
}

void mostrar_lista_compacta_recetas(const char* respuesta_servidor) {
    printf("\n");
    printf("┌─────────────────────────────────────────────────────────────────────────────────────┐\n");
    printf("│                                📄 LISTA DE RECETAS                                  │\n");
    printf("├────────┬────────────┬──────────────────────────┬─────────────────────┬──────────────┤\n");
    printf("│ Folio  │ Fecha      │ Paciente                 │ Médico              │ Lugar        │\n");
    printf("├────────┼────────────┼──────────────────────────┼─────────────────────┼──────────────┤\n");
    
    char* copia = strdup(respuesta_servidor);
    char* linea = strtok(copia, "\n");
    int recetas_mostradas = 0;
    
    while (linea != NULL) {
        if (strstr(linea, "===") != NULL || strstr(linea, "LISTA") != NULL || 
            strlen(linea) < 10 || strstr(linea, "No hay") != NULL) {
            linea = strtok(NULL, "\n");
            continue;
        }
        
        if (strstr(linea, "Folio:") != NULL) {
            int folio = 0;
            char fecha[20] = "", paciente[30] = "", medico[25] = "", lugar[20] = "";
            char paciente_corto[28] = "", medico_corto[23] = "", lugar_corto[14] = "";
            
            char linea_copia[500];
            strcpy(linea_copia, linea);
            
            // Folio
            char* token = strstr(linea_copia, "Folio: ");
            if (token) sscanf(token + 7, "%d", &folio);
            
            // Fecha
            token = strstr(linea_copia, "Fecha: ");
            if (token) sscanf(token + 7, "%19[^,]", fecha);
            
            // Paciente
            token = strstr(linea_copia, "Paciente: ");
            if (token) {
                sscanf(token + 10, "%29[^,]", paciente);
                strncpy(paciente_corto, paciente, sizeof(paciente_corto) - 1);
                paciente_corto[sizeof(paciente_corto) - 1] = '\0';
                if (strlen(paciente_corto) > 26) {
                    paciente_corto[23] = paciente_corto[24] = paciente_corto[25] = '.';
                    paciente_corto[26] = '\0';
                }
            }
            
            // Médico
            token = strstr(linea_copia, "Médico: ");
            if (token) {
                sscanf(token + 8, "%24[^,]", medico);
                strncpy(medico_corto, medico, sizeof(medico_corto) - 1);
                medico_corto[sizeof(medico_corto) - 1] = '\0';
                if (strlen(medico_corto) > 21) {
                    medico_corto[18] = medico_corto[19] = medico_corto[20] = '.';
                    medico_corto[21] = '\0';
                }
            }
            
            // Lugar
            token = strstr(linea_copia, "Lugar: ");
            if (token) {
                sscanf(token + 7, "%19[^\n]", lugar);
                strncpy(lugar_corto, lugar, sizeof(lugar_corto) - 1);
                lugar_corto[sizeof(lugar_corto) - 1] = '\0';
                if (strlen(lugar_corto) > 12) {
                    lugar_corto[9] = lugar_corto[10] = lugar_corto[11] = '.';
                    lugar_corto[12] = '\0';
                }
            }
            
            printf("│ %6d │ %-10s │ %-26s │ %-21s │ %-12s │\n", 
                   folio, fecha, paciente_corto, medico_corto, lugar_corto);
            recetas_mostradas++;
        }
        
        linea = strtok(NULL, "\n");
    }
    
    if (recetas_mostradas == 0) {
        printf("│                          No hay recetas registradas                           │\n");
    }
    
    printf("└────────┴────────────┴──────────────────────────┴─────────────────────┴──────────────┘\n");
    free(copia);
}

void mostrar_detalle_compacto(const char* respuesta_servidor, const char* titulo) {
    printf("\n");
    printf("┌─────────────────────────────────────────────────────────────────────────────────────┐\n");
    printf("│ %-83s │\n", titulo);
    printf("├─────────────────────────────────────────────────────────────────────────────────────┤\n");
    
    char* copia = strdup(respuesta_servidor);
    char* linea = strtok(copia, "\n");
    
    while (linea != NULL) {
        if (strstr(linea, "===") == NULL && strlen(linea) > 5) {
            // Dividir líneas largas
            if (strlen(linea) > 80) {
                char temp[85];
                strncpy(temp, linea, 80);
                temp[80] = '\0';
                printf("│ %-83s │\n", temp);
                
                if (strlen(linea) > 80) {
                    strncpy(temp, linea + 80, 80);
                    temp[80] = '\0';
                    printf("│ %-83s │\n", temp);
                }
            } else {
                printf("│ %-83s │\n", linea);
            }
        }
        linea = strtok(NULL, "\n");
    }
    
    printf("└─────────────────────────────────────────────────────────────────────────────────────┘\n");
    free(copia);
}

// ===============================
// FUNCIÓN DE RESPUESTA FORMATEADA
// ===============================

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
    
    printf("🔍 Select retornó: %d\n", ret);
    
    if (ret > 0) {
        memset(buffer, 0, TAM_BUFFER);
        
        int bytes_leidos = read(cliente_fifo_respuesta, buffer, TAM_BUFFER - 1);
        printf("🔍 Bytes leídos: %d\n", bytes_leidos);
        
        if (bytes_leidos > 0) {
            printf("\n════════════════════════════════════════\n");
            printf("📥 RESPUESTA DEL SERVIDOR (%d bytes):\n", bytes_leidos);
            
            // MOSTRAR CONTENIDO RAW PARA DEBUG
            printf("\n--- CONTENIDO RAW ---\n");
            printf("%s\n", buffer);
            printf("--- FIN CONTENIDO RAW ---\n\n");
            
            // Aplicar formato según el tipo
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
        } else if (bytes_leidos == 0) {
            printf("❌ Error: El servidor cerró la conexión (EOF)\n");
        } else {
            perror("❌ Error leyendo del FIFO");
        }
    } else if (ret == 0) {
        printf("⏰ Timeout: No se recibió respuesta del servidor en %d segundos\n", timeout_sec);
        printf("💡 Posibles causas:\n");
        printf("   - El servidor no está ejecutándose\n");
        printf("   - El comando no es reconocido por el servidor\n");
        printf("   - Hay un error en el servidor al procesar el comando\n");
    } else {
        perror("❌ Error en select()");
    }
    
    return 0;
}

// ===============================
// FUNCIONES DE INTERFAZ - MENÚS PRINCIPALES
// ===============================

void mostrar_menu_principal() {
    printf("\n");
    printf("╔═════════════════════════════════════════════╗\n");
    printf("║            🏥 SISTEMA HOSPITAL             ║\n");
    printf("╠═════════════════════════════════════════════╣\n");
    printf("║  1. 📋  Gestión de Pacientes               ║\n");
    printf("║  2. 👨‍⚕️   Gestión de Médicos                 ║\n");
    printf("║  3. 💊  Gestión de Medicamentos            ║\n");
    printf("║  4. 📄  Gestión de Recetas                 ║\n");
    printf("║  0. 🚪  Salir del Sistema                  ║\n");
    printf("╚═════════════════════════════════════════════╝\n");
    printf("\n🔹 Seleccione una opción (0-4): ");
}

void mostrar_submenu_pacientes() {
    printf("\n");
    printf("╔═════════════════════════════════════════════╗\n");
    printf("║           📋 GESTIÓN DE PACIENTES          ║\n");
    printf("╠═════════════════════════════════════════════╣\n");
    printf("║  1. 👤  Crear nuevo paciente               ║\n");
    printf("║  2. 📋  Listar todos los pacientes         ║\n");
    printf("║  3. 🔍  Buscar paciente por ID             ║\n");
    printf("║  4. ✏️   Actualizar datos de paciente      ║\n");
    printf("║  5. 🗑️   Eliminar paciente                 ║\n");
    printf("║  0. ↩️   Volver al menú principal          ║\n");
    printf("╚═════════════════════════════════════════════╝\n");
    printf("\n🔹 Seleccione una opción (0-5): ");
}

void mostrar_submenu_medicos() {
    printf("\n");
    printf("╔═════════════════════════════════════════════╗\n");
    printf("║           👨‍⚕️  GESTIÓN DE MÉDICOS           ║\n");
    printf("╠═════════════════════════════════════════════╣\n");
    printf("║  1. 🆕  Registrar nuevo médico             ║\n");
    printf("║  2. 📊  Listar todos los médicos           ║\n");
    printf("║  3. 🔎  Buscar médico por cédula           ║\n");
    printf("║  4. 📝  Actualizar datos de médico         ║\n");
    printf("║  5. ❌  Eliminar médico                    ║\n");
    printf("║  0. ↩️   Volver al menú principal          ║\n");
    printf("╚═════════════════════════════════════════════╝\n");
    printf("\n🔹 Seleccione una opción (0-5): ");
}

void mostrar_submenu_medicamentos() {
    printf("\n");
    printf("╔═════════════════════════════════════════════╗\n");
    printf("║          💊 GESTIÓN DE MEDICAMENTOS        ║\n");
    printf("╠═════════════════════════════════════════════╣\n");
    printf("║  1. 💊  Crear nuevo medicamento            ║\n");
    printf("║  2. 📦  Listar todos los medicamentos      ║\n");
    printf("║  3. 🔎  Buscar medicamento por ID          ║\n");
    printf("║  4. ✏️   Actualizar medicamento            ║\n");
    printf("║  5. 🗑️   Eliminar medicamento              ║\n");
    printf("║  0. ↩️   Volver al menú principal          ║\n");
    printf("╚═════════════════════════════════════════════╝\n");
    printf("\n🔹 Seleccione una opción (0-5): ");
}

void mostrar_submenu_recetas() {
    printf("\n");
    printf("╔═════════════════════════════════════════════╗\n");
    printf("║           📄 GESTIÓN DE RECETAS            ║\n");
    printf("╠═════════════════════════════════════════════╣\n");
    printf("║  1. 📄  Crear nueva receta                 ║\n");
    printf("║  2. 📋  Listar todas las recetas           ║\n");
    printf("║  3. 🔍  Buscar receta por folio            ║\n");
    printf("║  4. 💊  Agregar medicamento a receta       ║\n");
    printf("║  5. 📦  Ver medicamentos de receta         ║\n");
    printf("║  6. 🗑️   Eliminar receta                   ║\n");
    printf("║  0. ↩️   Volver al menú principal          ║\n");
    printf("╚═════════════════════════════════════════════╝\n");
    printf("\n🔹 Seleccione una opción (0-6): ");
}

// ===============================
// FUNCIONES INTERACTIVAS COMPLETAS
// ===============================

void crear_paciente_interactivo() {
    char nombre[100], apellidos[100], fecha_nac[11], direccion[200], sexo;
    float peso, estatura;
    int num_ced_medico;
    
    printf("\n");
    printf("┌─────────────────────────────────────────────┐\n");
    printf("│             👤 CREAR PACIENTE              │\n");
    printf("└─────────────────────────────────────────────┘\n");
    
    printf("🔹 Nombre: ");
    fgets(nombre, sizeof(nombre), stdin);
    nombre[strcspn(nombre, "\n")] = 0;
    
    printf("🔹 Apellidos: ");
    fgets(apellidos, sizeof(apellidos), stdin);
    apellidos[strcspn(apellidos, "\n")] = 0;
    
    printf("🔹 Fecha de nacimiento (YYYY-MM-DD): ");
    scanf("%s", fecha_nac);
    while(getchar() != '\n');
    
    printf("🔹 Dirección: ");
    fgets(direccion, sizeof(direccion), stdin);
    direccion[strcspn(direccion, "\n")] = 0;
    
    printf("🔹 Peso (kg): ");
    scanf("%f", &peso);
    
    printf("🔹 Estatura (metros): ");
    scanf("%f", &estatura);
    
    printf("🔹 Sexo (M/F): ");
    scanf(" %c", &sexo);
    
    printf("🔹 Número de cédula del médico (0 si no tiene): ");
    scanf("%d", &num_ced_medico);
    while(getchar() != '\n');
    
    char comando[1000];
    snprintf(comando, sizeof(comando), "crear_paciente|%s|%s|%s|%s|%.2f|%.2f|%c|%d", 
             nombre, apellidos, fecha_nac, direccion, peso, estatura, sexo, num_ced_medico);
    
    enviar_comando(comando);
    esperar_respuesta_con_timeout(5);
}

void buscar_paciente_interactivo() {
    int id;
    printf("\n🔍 ID del paciente a buscar: ");
    scanf("%d", &id);
    while(getchar() != '\n');
    
    char comando[100];
    snprintf(comando, sizeof(comando), "buscar_paciente|%d", id);
    
    enviar_comando(comando);
    esperar_respuesta_formateada(5, "detalle");
}

void actualizar_paciente_interactivo() {
    int id, campo;
    char nuevo_valor[200];
    
    printf("\n");
    printf("┌─────────────────────────────────────────────┐\n");
    printf("│          ✏️  ACTUALIZAR PACIENTE           │\n");
    printf("└─────────────────────────────────────────────┘\n");
    
    printf("🔹 ID del paciente a actualizar: ");
    scanf("%d", &id);
    while(getchar() != '\n');
    
    printf("\n📝 Campos disponibles para actualizar:\n");
    printf("1. Nombre\n");
    printf("2. Apellidos\n");
    printf("3. Fecha de nacimiento\n");
    printf("4. Dirección\n");
    printf("5. Peso\n");
    printf("6. Estatura\n");
    printf("7. Sexo\n");
    
    printf("\n🔹 Seleccione el campo a actualizar (1-7): ");
    scanf("%d", &campo);
    while(getchar() != '\n');
    
    printf("🔹 Nuevo valor: ");
    fgets(nuevo_valor, sizeof(nuevo_valor), stdin);
    nuevo_valor[strcspn(nuevo_valor, "\n")] = 0;
    
    char comando[500];
    snprintf(comando, sizeof(comando), "actualizar_paciente|%d|%d|%s", id, campo, nuevo_valor);
    
    enviar_comando(comando);
    esperar_respuesta_con_timeout(5);
}

void eliminar_paciente_interactivo() {
    int id;
    char confirmacion;
    
    printf("\n");
    printf("┌─────────────────────────────────────────────┐\n");
    printf("│           🗑️  ELIMINAR PACIENTE            │\n");
    printf("└─────────────────────────────────────────────┘\n");
    
    printf("🔹 ID del paciente a eliminar: ");
    scanf("%d", &id);
    while(getchar() != '\n');
    
    printf("⚠️  ¿Está seguro de eliminar al paciente con ID %d? (s/n): ", id);
    scanf(" %c", &confirmacion);
    while(getchar() != '\n');
    
    if (confirmacion == 's' || confirmacion == 'S') {
        char comando[100];
        snprintf(comando, sizeof(comando), "eliminar_paciente|%d", id);
        
        enviar_comando(comando);
        esperar_respuesta_con_timeout(5);
    } else {
        printf("❌ Operación cancelada\n");
    }
}

void crear_medico_interactivo() {
    char nombre[100], apellidos[100], especialidad[100];
    int num_ced;
    
    printf("\n");
    printf("┌─────────────────────────────────────────────┐\n");
    printf("│            🆕 CREAR MÉDICO                 │\n");
    printf("└─────────────────────────────────────────────┘\n");
    
    printf("🔹 Número de cédula: ");
    scanf("%d", &num_ced);
    while(getchar() != '\n');
    
    printf("🔹 Nombre: ");
    fgets(nombre, sizeof(nombre), stdin);
    nombre[strcspn(nombre, "\n")] = 0;
    
    printf("🔹 Apellidos: ");
    fgets(apellidos, sizeof(apellidos), stdin);
    apellidos[strcspn(apellidos, "\n")] = 0;
    
    printf("🔹 Especialidad: ");
    fgets(especialidad, sizeof(especialidad), stdin);
    especialidad[strcspn(especialidad, "\n")] = 0;
    
    char comando[500];
    snprintf(comando, sizeof(comando), "crear_medico|%d|%s|%s|%s", 
             num_ced, nombre, apellidos, especialidad);
    
    enviar_comando(comando);
    esperar_respuesta_con_timeout(5);
}

void buscar_medico_interactivo() {
    int num_ced;
    printf("\n🔍 Cédula del médico a buscar: ");
    scanf("%d", &num_ced);
    while(getchar() != '\n');
    
    char comando[100];
    snprintf(comando, sizeof(comando), "buscar_medico|%d", num_ced);
    
    enviar_comando(comando);
    esperar_respuesta_formateada(5, "detalle");
}

void actualizar_medico_interactivo() {
    int num_ced, campo;
    char nuevo_valor[200];
    
    printf("\n");
    printf("┌─────────────────────────────────────────────┐\n");
    printf("│          📝 ACTUALIZAR MÉDICO              │\n");
    printf("└─────────────────────────────────────────────┘\n");
    
    printf("🔹 Cédula del médico a actualizar: ");
    scanf("%d", &num_ced);
    while(getchar() != '\n');
    
    printf("\n📝 Campos disponibles para actualizar:\n");
    printf("1. Nombre\n");
    printf("2. Apellidos\n");
    printf("3. Especialidad\n");
    
    printf("\n🔹 Seleccione el campo a actualizar (1-3): ");
    scanf("%d", &campo);
    while(getchar() != '\n');
    
    printf("🔹 Nuevo valor: ");
    fgets(nuevo_valor, sizeof(nuevo_valor), stdin);
    nuevo_valor[strcspn(nuevo_valor, "\n")] = 0;
    
    char comando[500];
    snprintf(comando, sizeof(comando), "actualizar_medico|%d|%d|%s", num_ced, campo, nuevo_valor);
    
    enviar_comando(comando);
    esperar_respuesta_con_timeout(5);
}

void eliminar_medico_interactivo() {
    int num_ced;
    char confirmacion;
    
    printf("\n");
    printf("┌─────────────────────────────────────────────┐\n");
    printf("│            ❌ ELIMINAR MÉDICO              │\n");
    printf("└─────────────────────────────────────────────┘\n");
    
    printf("🔹 Cédula del médico a eliminar: ");
    scanf("%d", &num_ced);
    while(getchar() != '\n');
    
    printf("⚠️  ¿Está seguro de eliminar al médico con cédula %d? (s/n): ", num_ced);
    scanf(" %c", &confirmacion);
    while(getchar() != '\n');
    
    if (confirmacion == 's' || confirmacion == 'S') {
        char comando[100];
        snprintf(comando, sizeof(comando), "eliminar_medico|%d", num_ced);
        
        enviar_comando(comando);
        esperar_respuesta_con_timeout(5);
    } else {
        printf("❌ Operación cancelada\n");
    }
}

void crear_medicamento_interactivo() {
    char nombre[100], descripcion[200], presentacion[50], fecha_caducidad[11];
    int stock;
    
    printf("\n");
    printf("┌─────────────────────────────────────────────┐\n");
    printf("│           💊 CREAR MEDICAMENTO             │\n");
    printf("└─────────────────────────────────────────────┘\n");
    
    printf("🔹 Nombre del medicamento: ");
    fgets(nombre, sizeof(nombre), stdin);
    nombre[strcspn(nombre, "\n")] = 0;
    
    printf("🔹 Descripción: ");
    fgets(descripcion, sizeof(descripcion), stdin);
    descripcion[strcspn(descripcion, "\n")] = 0;
    
    printf("🔹 Presentación: ");
    fgets(presentacion, sizeof(presentacion), stdin);
    presentacion[strcspn(presentacion, "\n")] = 0;
    
    printf("🔹 Fecha de caducidad (YYYY-MM-DD): ");
    scanf("%s", fecha_caducidad);
    while(getchar() != '\n');
    
    printf("🔹 Stock inicial: ");
    scanf("%d", &stock);
    while(getchar() != '\n');
    
    char comando[1000];
    snprintf(comando, sizeof(comando), "crear_medicamento|%s|%s|%s|%s|%d", 
             nombre, descripcion, presentacion, fecha_caducidad, stock);
    
    enviar_comando(comando);
    esperar_respuesta_con_timeout(5);
}

void buscar_medicamento_interactivo() {
    int id;
    printf("\n🔍 ID del medicamento a buscar: ");
    scanf("%d", &id);
    while(getchar() != '\n');
    
    char comando[100];
    snprintf(comando, sizeof(comando), "buscar_medicamento|%d", id);
    
    enviar_comando(comando);
    esperar_respuesta_formateada(5, "detalle");
}

void actualizar_medicamento_interactivo() {
    int id, campo;
    char nuevo_valor[200];
    
    printf("\n");
    printf("┌─────────────────────────────────────────────┐\n");
    printf("│         ✏️  ACTUALIZAR MEDICAMENTO         │\n");
    printf("└─────────────────────────────────────────────┘\n");
    
    printf("🔹 ID del medicamento a actualizar: ");
    scanf("%d", &id);
    while(getchar() != '\n');
    
    printf("\n📝 Campos disponibles para actualizar:\n");
    printf("1. Nombre\n");
    printf("2. Descripción\n");
    printf("3. Presentación\n");
    printf("4. Fecha de caducidad\n");
    printf("5. Stock\n");
    
    printf("\n🔹 Seleccione el campo a actualizar (1-5): ");
    scanf("%d", &campo);
    while(getchar() != '\n');
    
    printf("🔹 Nuevo valor: ");
    fgets(nuevo_valor, sizeof(nuevo_valor), stdin);
    nuevo_valor[strcspn(nuevo_valor, "\n")] = 0;
    
    char comando[500];
    snprintf(comando, sizeof(comando), "actualizar_medicamento|%d|%d|%s", id, campo, nuevo_valor);
    
    enviar_comando(comando);
    esperar_respuesta_con_timeout(5);
}

void eliminar_medicamento_interactivo() {
    int id;
    char confirmacion;
    
    printf("\n");
    printf("┌─────────────────────────────────────────────┐\n");
    printf("│         🗑️  ELIMINAR MEDICAMENTO           │\n");
    printf("└─────────────────────────────────────────────┘\n");
    
    printf("🔹 ID del medicamento a eliminar: ");
    scanf("%d", &id);
    while(getchar() != '\n');
    
    printf("⚠️  ¿Está seguro de eliminar el medicamento con ID %d? (s/n): ", id);
    scanf(" %c", &confirmacion);
    while(getchar() != '\n');
    
    if (confirmacion == 's' || confirmacion == 'S') {
        char comando[100];
        snprintf(comando, sizeof(comando), "eliminar_medicamento|%d", id);
        
        enviar_comando(comando);
        esperar_respuesta_con_timeout(5);
    } else {
        printf("❌ Operación cancelada\n");
    }
}

void crear_receta_interactivo() {
    int num_exp, num_ced;
    char fecha_receta[11];
    
    printf("\n");
    printf("┌─────────────────────────────────────────────┐\n");
    printf("│            📄 CREAR RECETA                 │\n");
    printf("└─────────────────────────────────────────────┘\n");
    
    printf("🔹 Número de expediente del paciente: ");
    scanf("%d", &num_exp);
    while(getchar() != '\n');
    
    printf("🔹 Número de cédula del médico: ");
    scanf("%d", &num_ced);
    while(getchar() != '\n');
    
    printf("🔹 Fecha de la receta (YYYY-MM-DD): ");
    scanf("%s", fecha_receta);
    while(getchar() != '\n');
    
    char comando[500];
    snprintf(comando, sizeof(comando), "crear_receta|%d|%d|%s", 
             num_exp, num_ced, fecha_receta);
    
    enviar_comando(comando);
    esperar_respuesta_con_timeout(5);
}

void buscar_receta_interactivo() {
    int folio;
    printf("\n🔍 Folio de la receta a buscar: ");
    scanf("%d", &folio);
    while(getchar() != '\n');
    
    char comando[100];
    snprintf(comando, sizeof(comando), "buscar_receta|%d", folio);
    
    enviar_comando(comando);
    esperar_respuesta_formateada(5, "detalle");
}

void agregar_medicamento_receta_interactivo() {
    int folio, num_med, cantidad;
    
    printf("\n");
    printf("┌─────────────────────────────────────────────┐\n");
    printf("│      💊 AGREGAR MEDICAMENTO A RECETA       │\n");
    printf("└─────────────────────────────────────────────┘\n");
    
    printf("🔹 Folio de la receta: ");
    scanf("%d", &folio);
    while(getchar() != '\n');
    
    printf("🔹 ID del medicamento: ");
    scanf("%d", &num_med);
    while(getchar() != '\n');
    
    printf("🔹 Cantidad: ");
    scanf("%d", &cantidad);
    while(getchar() != '\n');
    
    char comando[500];
    snprintf(comando, sizeof(comando), "agregar_medicamento_receta|%d|%d|%d", 
             folio, num_med, cantidad);
    
    enviar_comando(comando);
    esperar_respuesta_con_timeout(5);
}

void listar_medicamentos_receta_interactivo() {
    int folio;
    printf("\n🔍 Folio de la receta: ");
    scanf("%d", &folio);
    while(getchar() != '\n');
    
    char comando[100];
    snprintf(comando, sizeof(comando), "listar_medicamentos_receta|%d", folio);
    
    enviar_comando(comando);
    esperar_respuesta_formateada(5, "detalle");
}

void eliminar_receta_interactivo() {
    int folio;
    char confirmacion;
    
    printf("\n");
    printf("┌─────────────────────────────────────────────┐\n");
    printf("│           🗑️  ELIMINAR RECETA              │\n");
    printf("└─────────────────────────────────────────────┘\n");
    
    printf("🔹 Folio de la receta a eliminar: ");
    scanf("%d", &folio);
    while(getchar() != '\n');
    
    printf("⚠️  ¿Está seguro de eliminar la receta con folio %d? (s/n): ", folio);
    scanf(" %c", &confirmacion);
    while(getchar() != '\n');
    
    if (confirmacion == 's' || confirmacion == 'S') {
        char comando[100];
        snprintf(comando, sizeof(comando), "eliminar_receta|%d", folio);
        
        enviar_comando(comando);
        esperar_respuesta_con_timeout(5);
    } else {
        printf("❌ Operación cancelada\n");
    }
}

// ===============================
// FUNCIONES DE PROCESAMIENTO DE SUBMENÚS COMPLETAS
// ===============================

void procesar_submenu_pacientes() {
    int opcion;
    do {
        mostrar_submenu_pacientes();
        
        if (scanf("%d", &opcion) != 1) {
            while(getchar() != '\n');
            printf("❌ Opción inválida\n");
            continue;
        }
        while(getchar() != '\n');
        
        switch(opcion) {
            case 1:
                crear_paciente_interactivo();
                break;
            case 2:
                enviar_comando("listar_pacientes");
                esperar_respuesta_formateada(5, "pacientes");
                break;
            case 3:
                buscar_paciente_interactivo();
                break;
            case 4:
                actualizar_paciente_interactivo();
                break;
            case 5:
                eliminar_paciente_interactivo();
                break;
            case 0:
                printf("↩️  Volviendo al menú principal...\n");
                break;
            default:
                printf("❌ Opción no válida\n");
        }
        
        if (opcion != 0) {
            printf("\n⏎ Presione Enter para continuar...");
            getchar();
        }
        
    } while (opcion != 0);
}

void procesar_submenu_medicos() {
    int opcion;
    do {
        mostrar_submenu_medicos();
        
        if (scanf("%d", &opcion) != 1) {
            while(getchar() != '\n');
            printf("❌ Opción inválida\n");
            continue;
        }
        while(getchar() != '\n');
        
        switch(opcion) {
            case 1:
                crear_medico_interactivo();
                break;
            case 2:
                enviar_comando("listar_medicos");
                esperar_respuesta_formateada(5, "medicos");
                break;
            case 3:
                buscar_medico_interactivo();
                break;
            case 4:
                actualizar_medico_interactivo();
                break;
            case 5:
                eliminar_medico_interactivo();
                break;
            case 0:
                printf("↩️  Volviendo al menú principal...\n");
                break;
            default:
                printf("❌ Opción no válida\n");
        }
        
        if (opcion != 0) {
            printf("\n⏎ Presione Enter para continuar...");
            getchar();
        }
        
    } while (opcion != 0);
}

void procesar_submenu_medicamentos() {
    int opcion;
    do {
        mostrar_submenu_medicamentos();
        
        if (scanf("%d", &opcion) != 1) {
            while(getchar() != '\n');
            printf("❌ Opción inválida\n");
            continue;
        }
        while(getchar() != '\n');
        
        switch(opcion) {
            case 1:
                crear_medicamento_interactivo();
                break;
            case 2:
                enviar_comando("listar_medicamentos");
                esperar_respuesta_formateada(5, "medicamentos");
                break;
            case 3:
                buscar_medicamento_interactivo();
                break;
            case 4:
                actualizar_medicamento_interactivo();
                break;
            case 5:
                eliminar_medicamento_interactivo();
                break;
            case 0:
                printf("↩️  Volviendo al menú principal...\n");
                break;
            default:
                printf("❌ Opción no válida\n");
        }
        
        if (opcion != 0) {
            printf("\n⏎ Presione Enter para continuar...");
            getchar();
        }
        
    } while (opcion != 0);
}

void procesar_submenu_recetas() {
    int opcion;
    do {
        mostrar_submenu_recetas();
        
        if (scanf("%d", &opcion) != 1) {
            while(getchar() != '\n');
            printf("❌ Opción inválida\n");
            continue;
        }
        while(getchar() != '\n');
        
        switch(opcion) {
            case 1:
                crear_receta_interactivo();
                break;
            case 2:
                enviar_comando("listar_recetas");
                esperar_respuesta_formateada(5, "recetas");
                break;
            case 3:
                buscar_receta_interactivo();
                break;
            case 4:
                agregar_medicamento_receta_interactivo();
                break;
            case 5:
                listar_medicamentos_receta_interactivo();
                break;
            case 6:
                eliminar_receta_interactivo();
                break;
            case 0:
                printf("↩️  Volviendo al menú principal...\n");
                break;
            default:
                printf("❌ Opción no válida\n");
        }
        
        if (opcion != 0) {
            printf("\n⏎ Presione Enter para continuar...");
            getchar();
        }
        
    } while (opcion != 0);
}

// ===============================
// FUNCIÓN PRINCIPAL
// ===============================

int main() {
    mi_pid = getpid();
    signal(SIGINT, manejar_senal);

    printf("=== CLIENTE DEL SISTEMA HOSPITAL ===\n");
    printf("PID del cliente: %d\n", mi_pid);

    // Crear FIFO para respuestas
    snprintf(fifo_respuesta, 50, "/tmp/hospital_cliente_%d_respuesta", mi_pid);
    
    // Eliminar FIFO si ya existe para evitar problemas
    unlink(fifo_respuesta);
    
    // Crear FIFO
    if (mkfifo(fifo_respuesta, 0666) == -1) {
        perror("❌ Error al crear FIFO de respuesta");
        exit(1);
    }

    printf("✅ FIFO de respuesta creado: %s\n", fifo_respuesta);

    // Abrir el FIFO de respuesta en modo NO BLOQUEANTE para inicialización
    printf("Abriendo FIFO para lectura (modo no bloqueante)...\n");
    cliente_fifo_respuesta = open(fifo_respuesta, O_RDONLY | O_NONBLOCK);
    if (cliente_fifo_respuesta == -1) {
        perror("❌ Error al abrir FIFO de respuesta");
        exit(1);
    }
    printf("✅ FIFO abierto correctamente (fd: %d)\n", cliente_fifo_respuesta);

    printf("🟢 Cliente listo para operar\n\n");

    int opcion_principal;
    do {
        mostrar_menu_principal();
        
        if (scanf("%d", &opcion_principal) != 1) {
            while(getchar() != '\n');
            printf("❌ Opción inválida\n");
            continue;
        }
        while(getchar() != '\n');
        
        switch(opcion_principal) {
            case 1:
                procesar_submenu_pacientes();
                break;
            case 2:
                procesar_submenu_medicos();
                break;
            case 3:
                procesar_submenu_medicamentos();
                break;
            case 4:
                procesar_submenu_recetas();
                break;
            case 0:
                printf("👋 Saliendo del sistema...\n");
                break;
            default:
                printf("❌ Opción no válida\n");
        }
        
    } while (opcion_principal != 0);

    // Limpieza
    printf("🔒 Cerrando cliente...\n");
    
    if (cliente_fifo_respuesta > 0) {
        close(cliente_fifo_respuesta);
    }
    unlink(fifo_respuesta);

    return 0;
}