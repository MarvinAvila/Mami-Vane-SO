#include "cliente_formatos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
            
            // Crear nombre completo
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