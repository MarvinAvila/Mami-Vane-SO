#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "medico.h"

void crear_medico() {
    char nombre[100], apellidos[100], direccion[200];
    char fecha_nac[11], especialidad[50];
    
    printf("Nombre: ");
    fgets(nombre, sizeof(nombre), stdin);
    nombre[strcspn(nombre, "\n")] = 0;

    printf("Apellidos: ");
    fgets(apellidos, sizeof(apellidos), stdin);
    apellidos[strcspn(apellidos, "\n")] = 0;

    printf("Fecha de nacimiento (YYYY-MM-DD): ");
    scanf("%s", fecha_nac);
    while(getchar() != '\n');

    printf("Dirección: ");
    fgets(direccion, sizeof(direccion), stdin);
    direccion[strcspn(direccion, "\n")] = 0;

    printf("Especialidad (medicina general, traumatologia, cardiologia, ginecologia): ");
    fgets(especialidad, sizeof(especialidad), stdin);
    especialidad[strcspn(especialidad, "\n")] = 0;

    char consulta[1000];
    snprintf(consulta, sizeof(consulta),
             "INSERT INTO medico (nombre, apellidos, fecha_nac, direccion, especialidad) "
             "VALUES ('%s', '%s', '%s', '%s', '%s')",
             nombre, apellidos, fecha_nac, direccion, especialidad);

    if (ejecutar_consulta(consulta) == 0)
        printf("Médico creado exitosamente.\n");
    else
        printf("Error: Verifique que cumple las restricciones (edad >=25, especialidad válida).\n");
}

void listar_medicos() {
    const char *consulta = "SELECT * FROM medico ORDER BY num_ced";
    PGresult *res = ejecutar_consulta_con_resultado(consulta);

    if (res == NULL) return;

    int rows = PQntuples(res);
    printf("\n=== LISTA DE MEDICOS ===\n");
    for (int i = 0; i < rows; i++) {
        printf("ID: %s, Nombre: %s %s, Edad: %s, Especialidad: %s\n",
               PQgetvalue(res, i, 0), // num_ced
               PQgetvalue(res, i, 1), // nombre
               PQgetvalue(res, i, 2), // apellidos
               PQgetvalue(res, i, 4), // edad
               PQgetvalue(res, i, 6)); // especialidad
    }

    PQclear(res);
}

void buscar_medico() {
    int num_ced;
    printf("Ingrese el ID del médico a buscar: ");
    scanf("%d", &num_ced);

    char consulta[200];
    snprintf(consulta, sizeof(consulta), "SELECT * FROM medico WHERE num_ced = %d", num_ced);

    PGresult *res = ejecutar_consulta_con_resultado(consulta);

    if (res == NULL || PQntuples(res) == 0) {
        printf("Médico con ID %d no encontrado.\n", num_ced);
    } else {
        printf("\n=== MÉDICO ENCONTRADO ===\n");
        printf("ID: %s, Nombre: %s %s, Edad: %s, Especialidad: %s\n",
               PQgetvalue(res, 0, 0),
               PQgetvalue(res, 0, 1),
               PQgetvalue(res, 0, 2),
               PQgetvalue(res, 0, 4),
               PQgetvalue(res, 0, 6));
    }

    if (res != NULL) PQclear(res);
}

void actualizar_medico() {
    int num_ced, opcion;
    printf("Ingrese el ID del médico a actualizar: ");
    scanf("%d", &num_ced);
    while(getchar() != '\n');

    char consulta_verificar[200];
    snprintf(consulta_verificar, sizeof(consulta_verificar),
             "SELECT * FROM medico WHERE num_ced = %d", num_ced);
    PGresult *res_verificar = ejecutar_consulta_con_resultado(consulta_verificar);

    if (res_verificar == NULL || PQntuples(res_verificar) == 0) {
        printf("Médico con ID %d no encontrado.\n", num_ced);
        if (res_verificar != NULL) PQclear(res_verificar);
        return;
    }
    PQclear(res_verificar);

    do {
        printf("\n--- Campos disponibles para actualizar ---\n");
        printf("1. Nombre\n");
        printf("2. Apellidos\n");
        printf("3. Fecha de nacimiento\n");
        printf("4. Dirección\n");
        printf("5. Especialidad\n");
        printf("0. Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);
        while(getchar() != '\n');

        char nuevo_valor[200];
        char consulta[1000];

        switch(opcion) {
            case 1:
                printf("Nuevo nombre: ");
                fgets(nuevo_valor, sizeof(nuevo_valor), stdin);
                nuevo_valor[strcspn(nuevo_valor, "\n")] = 0;
                snprintf(consulta, sizeof(consulta),
                         "UPDATE medico SET nombre = '%s' WHERE num_ced = %d",
                         nuevo_valor, num_ced);
                ejecutar_consulta(consulta);
                printf("Nombre actualizado.\n");
                break;
            case 2:
                printf("Nuevos apellidos: ");
                fgets(nuevo_valor, sizeof(nuevo_valor), stdin);
                nuevo_valor[strcspn(nuevo_valor, "\n")] = 0;
                snprintf(consulta, sizeof(consulta),
                         "UPDATE medico SET apellidos = '%s' WHERE num_ced = %d",
                         nuevo_valor, num_ced);
                ejecutar_consulta(consulta);
                printf("Apellidos actualizados.\n");
                break;
            case 3:
                printf("Nueva fecha de nacimiento (YYYY-MM-DD): ");
                scanf("%s", nuevo_valor);
                while(getchar() != '\n');
                snprintf(consulta, sizeof(consulta),
                         "UPDATE medico SET fecha_nac = '%s' WHERE num_ced = %d",
                         nuevo_valor, num_ced);
                if (ejecutar_consulta(consulta) == 0)
                    printf("Fecha de nacimiento actualizada.\n");
                else
                    printf("Error: médico debe tener >=25 años.\n");
                break;
            case 4:
                printf("Nueva dirección: ");
                fgets(nuevo_valor, sizeof(nuevo_valor), stdin);
                nuevo_valor[strcspn(nuevo_valor, "\n")] = 0;
                snprintf(consulta, sizeof(consulta),
                         "UPDATE medico SET direccion = '%s' WHERE num_ced = %d",
                         nuevo_valor, num_ced);
                ejecutar_consulta(consulta);
                printf("Dirección actualizada.\n");
                break;
            case 5:
                printf("Nueva especialidad: ");
                fgets(nuevo_valor, sizeof(nuevo_valor), stdin);
                nuevo_valor[strcspn(nuevo_valor, "\n")] = 0;
                snprintf(consulta, sizeof(consulta),
                         "UPDATE medico SET especialidad = '%s' WHERE num_ced = %d",
                         nuevo_valor, num_ced);
                if (ejecutar_consulta(consulta) == 0)
                    printf("Especialidad actualizada.\n");
                else
                    printf("Error: especialidad no válida.\n");
                break;
            case 0:
                printf("Saliendo...\n");
                break;
            default:
                printf("Opción no válida.\n");
        }
    } while(opcion != 0);
}

void eliminar_medico() {
    int num_ced;
    printf("Ingrese el ID del médico a eliminar: ");
    scanf("%d", &num_ced);

    printf("¿Está seguro de eliminar al médico con ID %d? (s/n): ", num_ced);
    char confirm;
    scanf(" %c", &confirm);

    if (confirm == 's' || confirm == 'S') {
        char consulta[200];
        snprintf(consulta, sizeof(consulta), "DELETE FROM medico WHERE num_ced = %d", num_ced);
        ejecutar_consulta(consulta);
        printf("Médico eliminado.\n");
    } else {
        printf("Operación cancelada.\n");
    }
}
