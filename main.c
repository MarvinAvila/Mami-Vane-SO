#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "database.h"
#include "paciente.h"
#include "medico.h"
#include "medicamento.h"
#include "receta.h"

void mostrar_menu_principal();
void gestionar_pacientes();
void gestionar_medicos();
void gestionar_medicamentos();
void gestionar_recetas();

int main() {
    int opcion;

    printf("Proceso PADRE (PID=%d) inicializado.\n", getpid());

    do {
        mostrar_menu_principal();
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);
        while(getchar() != '\n'); // Limpiar buffer

        pid_t pid;

        switch(opcion) {
            case 1: // Pacientes
                pid = fork();
                if (pid == 0) { // Hijo
                    printf("Proceso HIJO (Pacientes) PID=%d, PPID=%d\n", getpid(), getppid());
                    if (!conectar_base_datos()) {
                        printf("Error al conectar a la base de datos en hijo Pacientes.\n");
                        exit(1);
                    }
                    gestionar_pacientes();
                    desconectar_base_datos();
                    exit(0);
                } else if (pid > 0) {
                    wait(NULL);
                }
                break;

            case 2: // Médicos
                pid = fork();
                if (pid == 0) { // Hijo
                    printf("Proceso HIJO (Medicos) PID=%d, PPID=%d\n", getpid(), getppid());
                    if (!conectar_base_datos()) {
                        printf("Error al conectar a la base de datos en hijo Medicos.\n");
                        exit(1);
                    }
                    gestionar_medicos();
                    desconectar_base_datos();
                    exit(0);
                } else if (pid > 0) {
                    wait(NULL);
                }
                break;

            case 3: // Medicamentos
                pid = fork();
                if (pid == 0) { // Hijo
                    printf("Proceso HIJO (Medicamentos) PID=%d, PPID=%d\n", getpid(), getppid());
                    if (!conectar_base_datos()) {
                        printf("Error al conectar a la base de datos en hijo Medicamentos.\n");
                        exit(1);
                    }
                    gestionar_medicamentos();
                    desconectar_base_datos();
                    exit(0);
                } else if (pid > 0) {
                    wait(NULL);
                }
                break;

            case 4: // Recetas
                pid = fork();
                if (pid == 0) { // Hijo
                    printf("Proceso HIJO (Recetas) PID=%d, PPID=%d\n", getpid(), getppid());
                    if (!conectar_base_datos()) {
                        printf("Error al conectar a la base de datos en hijo Recetas.\n");
                        exit(1);
                    }
                    gestionar_recetas();
                    desconectar_base_datos();
                    exit(0);
                } else if (pid > 0) {
                    wait(NULL);
                }
                break;

            case 0:
                printf("Proceso PADRE (PID=%d) finalizando...\n", getpid());
                break;

            default:
                printf("Opción no válida. Intente nuevamente.\n");
        }
    } while (opcion != 0);

    return 0;
}

void mostrar_menu_principal() {
    printf("\n===== MENU PRINCIPAL =====\n");
    printf("1. Gestionar Pacientes\n");
    printf("2. Gestionar Medicos\n");
    printf("3. Gestionar Medicamentos\n");
    printf("4. Gestionar Recetas\n");
    printf("0. Salir\n");
}


/* ===============================
   SUBMENÚS - Cada operación = NIETO
   =============================== */

void gestionar_pacientes() {
    int opcion;
    do {
        printf("\n=== GESTIÓN DE PACIENTES ===\n");
        printf("1. Crear paciente\n");
        printf("2. Listar pacientes\n");
        printf("3. Buscar paciente por ID\n");
        printf("4. Actualizar paciente\n");
        printf("5. Eliminar paciente\n");
        printf("0. Volver\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);
        while(getchar() != '\n');

        pid_t pid = fork();
        if (pid == 0) { // Nieto
            printf("Proceso NIETO (Pacientes - Opción %d) PID=%d, PPID=%d\n", opcion, getpid(), getppid());
            switch(opcion) {
                case 1: crear_paciente(); break;
                case 2: listar_pacientes(); break;
                case 3: buscar_paciente(); break;
                case 4: actualizar_paciente(); break;
                case 5: eliminar_paciente(); break;
            }
            exit(0);
        } else if (pid > 0) {
            wait(NULL);
        }
    } while (opcion != 0);
}

void gestionar_medicos() {
    int opcion;
    do {
        printf("\n=== GESTIÓN DE MÉDICOS ===\n");
        printf("1. Crear médico\n");
        printf("2. Listar médicos\n");
        printf("3. Buscar médico por ID\n");
        printf("4. Actualizar médico\n");
        printf("5. Eliminar médico\n");
        printf("0. Volver\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);
        while(getchar() != '\n');

        pid_t pid = fork();
        if (pid == 0) { // Nieto
            printf("Proceso NIETO (Medicos - Opción %d) PID=%d, PPID=%d\n", opcion, getpid(), getppid());
            switch(opcion) {
                case 1: crear_medico(); break;
                case 2: listar_medicos(); break;
                case 3: buscar_medico(); break;
                case 4: actualizar_medico(); break;
                case 5: eliminar_medico(); break;
            }
            exit(0);
        } else if (pid > 0) {
            wait(NULL);
        }
    } while (opcion != 0);
}

void gestionar_medicamentos() {
    int opcion;
    do {
        printf("\n=== GESTIÓN DE MEDICAMENTOS ===\n");
        printf("1. Crear medicamento\n");
        printf("2. Listar medicamentos\n");
        printf("3. Buscar medicamento por ID\n");
        printf("4. Actualizar medicamento\n");
        printf("5. Eliminar medicamento\n");
        printf("0. Volver\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);
        while(getchar() != '\n');

        pid_t pid = fork();
        if (pid == 0) { // Nieto
            printf("Proceso NIETO (Medicamentos - Opción %d) PID=%d, PPID=%d\n", opcion, getpid(), getppid());
            switch(opcion) {
                case 1: crear_medicamento(); break;
                case 2: listar_medicamentos(); break;
                case 3: buscar_medicamento(); break;
                case 4: actualizar_medicamento(); break;
                case 5: eliminar_medicamento(); break;
            }
            exit(0);
        } else if (pid > 0) {
            wait(NULL);
        }
    } while (opcion != 0);
}

void gestionar_recetas() {
    int opcion;
    do {
        printf("\n=== GESTIÓN DE RECETAS ===\n");
        printf("1. Crear receta\n");
        printf("2. Listar recetas\n");
        printf("3. Buscar receta por ID\n");
        printf("4. Agregar medicamento a receta\n");
        printf("5. Ver medicamentos de receta\n");
        printf("6. Eliminar receta\n");
        printf("0. Volver\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);
        while(getchar() != '\n');

        pid_t pid = fork();
        if (pid == 0) { // Nieto
            printf("Proceso NIETO (Recetas - Opción %d) PID=%d, PPID=%d\n", opcion, getpid(), getppid());
            switch(opcion) {
                case 1: crear_receta(); break;
                case 2: listar_recetas(); break;
                case 3: buscar_receta(); break;
                case 4: agregar_medicamento_receta(); break;
                case 5: listar_medicamentos_receta(); break;
                case 6: eliminar_receta(); break;
            }
            exit(0);
        } else if (pid > 0) {
            wait(NULL);
        }
    } while (opcion != 0);
}
