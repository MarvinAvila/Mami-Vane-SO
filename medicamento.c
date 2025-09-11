#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "medicamento.h"

void crear_medicamento() {
    char nombre[100], descrip[200], presentacion[50], fecha_caducidad[11];
    int stock;

    printf("Nombre del medicamento: ");
    fgets(nombre, sizeof(nombre), stdin);
    nombre[strcspn(nombre, "\n")] = 0;

    printf("Descripción: ");
    fgets(descrip, sizeof(descrip), stdin);
    descrip[strcspn(descrip, "\n")] = 0;

    printf("Presentación: ");
    fgets(presentacion, sizeof(presentacion), stdin);
    presentacion[strcspn(presentacion, "\n")] = 0;

    printf("Fecha de caducidad (YYYY-MM-DD): ");
    scanf("%s", fecha_caducidad);
    while(getchar() != '\n');

    printf("Stock inicial: ");
    scanf("%d", &stock);
    while(getchar() != '\n');

    char consulta[500];
    snprintf(consulta, sizeof(consulta),
             "INSERT INTO medicamento (nombre, descrip, presentacion, fecha_caducidad, stock) "
             "VALUES ('%s', '%s', '%s', '%s', %d)",
             nombre, descrip, presentacion, fecha_caducidad, stock);
    if (ejecutar_consulta(consulta) == 0)
        printf("Medicamento creado exitosamente.\n");
    else
        printf("Error: Verifique que la presentación sea válida y el stock no negativo.\n");
}

void listar_medicamentos() {
    const char *consulta = "SELECT * FROM medicamento ORDER BY num_med";
    PGresult *res = ejecutar_consulta_con_resultado(consulta);

    if (res == NULL) return;

    int rows = PQntuples(res);
    printf("\n=== LISTA DE MEDICAMENTOS ===\n");
    for (int i = 0; i < rows; i++) {
        printf("ID: %s, Nombre: %s, Presentación: %s, Caducidad: %s, Cantidad: %s\n",
               PQgetvalue(res, i, 0), // num_med
               PQgetvalue(res, i, 1), // nombre
               PQgetvalue(res, i, 3), // presentacion
               PQgetvalue(res, i, 4), // fecha_caducidad
               PQgetvalue(res, i, 5)); // cantidad
    }


    PQclear(res);
}

void buscar_medicamento() {
    int id;
    printf("Ingrese el ID del medicamento a buscar: ");
    scanf("%d", &id);

    char consulta[200];
    snprintf(consulta, sizeof(consulta), "SELECT * FROM medicamento WHERE num_med  = %d", id);

    PGresult *res = ejecutar_consulta_con_resultado(consulta);

    if (res == NULL || PQntuples(res) == 0) {
        printf("Medicamento con ID %d no encontrado.\n", id);
    } else {
        printf("\n=== MEDICAMENTO ENCONTRADO ===\n");
        printf("ID: %s, Nombre: %s, Descripción: %s\nPresentación: %s, Caducidad: %s, Cantidad: %s\n",
               PQgetvalue(res, 0, 0),
               PQgetvalue(res, 0, 1),
               PQgetvalue(res, 0, 2),
               PQgetvalue(res, 0, 3),
               PQgetvalue(res, 0, 4),
               PQgetvalue(res, 0, 5));
    }
    if (res != NULL) PQclear(res);
}

void actualizar_medicamento() {
    char num_med[10];
    printf("Ingrese el ID del medicamento a actualizar: ");
    fgets(num_med, sizeof(num_med), stdin);
    num_med[strcspn(num_med, "\n")] = 0;

    char consulta_verificar[200];
    snprintf(consulta_verificar, sizeof(consulta_verificar),
             "SELECT * FROM medicamento WHERE num_med = '%s'", num_med);
    PGresult *res_verificar = ejecutar_consulta_con_resultado(consulta_verificar);

    if (res_verificar == NULL || PQntuples(res_verificar) == 0) {
        printf("Medicamento con ID %s no encontrado.\n", num_med);
        if (res_verificar != NULL) PQclear(res_verificar);
        return;
    }
    PQclear(res_verificar);

    int opcion;
    do {
        printf("\n--- Campos disponibles para actualizar ---\n");
        printf("1. Nombre\n");
        printf("2. Descripción\n");
        printf("3. Presentación\n");
        printf("4. Fecha de caducidad\n");
        printf("5. Cantidad\n");
        printf("0. Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);
        while(getchar() != '\n');

        char nuevo_valor[200];
        char consulta[500];

        switch(opcion) {
            case 1:
                printf("Nuevo nombre: ");
                fgets(nuevo_valor, sizeof(nuevo_valor), stdin);
                nuevo_valor[strcspn(nuevo_valor, "\n")] = 0;
                snprintf(consulta, sizeof(consulta),
                         "UPDATE medicamento SET nombre = '%s' WHERE num_med = '%s'",
                         nuevo_valor, num_med);
                ejecutar_consulta(consulta);
                printf("Nombre actualizado.\n");
                break;

            case 2:
                printf("Nueva descripción: ");
                fgets(nuevo_valor, sizeof(nuevo_valor), stdin);
                nuevo_valor[strcspn(nuevo_valor, "\n")] = 0;
                snprintf(consulta, sizeof(consulta),
                         "UPDATE medicamento SET descrip = '%s' WHERE num_med = '%s'",
                         nuevo_valor, num_med);
                ejecutar_consulta(consulta);
                printf("Descripción actualizada.\n");
                break;

            case 3:
                printf("Nueva presentación: ");
                fgets(nuevo_valor, sizeof(nuevo_valor), stdin);
                nuevo_valor[strcspn(nuevo_valor, "\n")] = 0;
                snprintf(consulta, sizeof(consulta),
                         "UPDATE medicamento SET presentacion = '%s' WHERE num_med = '%s'",
                         nuevo_valor, num_med);
                ejecutar_consulta(consulta);
                printf("Presentación actualizada.\n");
                break;

            case 4:
                printf("Nueva fecha de caducidad (YYYY-MM-DD): ");
                scanf("%s", nuevo_valor);
                while(getchar() != '\n');
                snprintf(consulta, sizeof(consulta),
                         "UPDATE medicamento SET fecha_caducidad = '%s' WHERE num_med = '%s'",
                         nuevo_valor, num_med);
                if (ejecutar_consulta(consulta))
                    printf("Fecha de caducidad actualizada.\n");
                else
                    printf("Error: La fecha debe ser futura.\n");
                break;

            case 5:
                printf("Nueva cantidad: ");
                scanf("%s", nuevo_valor);
                while(getchar() != '\n');
                snprintf(consulta, sizeof(consulta),
                         "UPDATE medicamento SET cantidad = %s WHERE num_med = '%s'",
                         nuevo_valor, num_med);
                if (ejecutar_consulta(consulta))
                    printf("Cantidad actualizada.\n");
                else
                    printf("Error: Cantidad debe ser >= 0.\n");
                break;

            case 0:
                printf("Saliendo...\n");
                break;

            default:
                printf("Opción no válida.\n");
        }
    } while(opcion != 0);
}


void eliminar_medicamento() {
    int id;
    printf("Ingrese el ID del medicamento a eliminar: ");
    scanf("%d", &id);

    printf("¿Está seguro de eliminar el medicamento con ID %d? (s/n): ", id);
    char confirm;
    scanf(" %c", &confirm);

    if (confirm == 's' || confirm == 'S') {
        char consulta[200];
        snprintf(consulta, sizeof(consulta), "DELETE FROM medicamento WHERE num_med << = %d", id);
        ejecutar_consulta(consulta);
        printf("Medicamento eliminado.\n");
    } else {
        printf("Operación cancelada.\n");
    }
}
