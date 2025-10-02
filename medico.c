#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "medico.h"

void crear_medico()
{
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
    while (getchar() != '\n')
        ;

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

void listar_medicos()
{
    const char *consulta = "SELECT * FROM medico ORDER BY num_ced";
    PGresult *res = ejecutar_consulta_con_resultado(consulta);

    if (res == NULL)
        return;

    int rows = PQntuples(res);
    printf("\n=== LISTA DE MEDICOS ===\n");
    for (int i = 0; i < rows; i++)
    {
        printf("ID: %s, Nombre: %s %s, Edad: %s, Especialidad: %s\n",
               PQgetvalue(res, i, 0),  // num_ced
               PQgetvalue(res, i, 1),  // nombre
               PQgetvalue(res, i, 2),  // apellidos
               PQgetvalue(res, i, 4),  // edad
               PQgetvalue(res, i, 6)); // especialidad
    }

    PQclear(res);
}

void buscar_medico()
{
    int num_ced;
    printf("Ingrese el ID del médico a buscar: ");
    scanf("%d", &num_ced);

    char consulta[200];
    snprintf(consulta, sizeof(consulta), "SELECT * FROM medico WHERE num_ced = %d", num_ced);

    PGresult *res = ejecutar_consulta_con_resultado(consulta);

    if (res == NULL || PQntuples(res) == 0)
    {
        printf("Médico con ID %d no encontrado.\n", num_ced);
    }
    else
    {
        printf("\n=== MÉDICO ENCONTRADO ===\n");
        printf("ID: %s, Nombre: %s %s, Edad: %s, Especialidad: %s\n",
               PQgetvalue(res, 0, 0),
               PQgetvalue(res, 0, 1),
               PQgetvalue(res, 0, 2),
               PQgetvalue(res, 0, 4),
               PQgetvalue(res, 0, 6));
    }

    if (res != NULL)
        PQclear(res);
}

void actualizar_medico()
{
    int num_ced, opcion;
    printf("Ingrese el ID del médico a actualizar: ");
    scanf("%d", &num_ced);
    while (getchar() != '\n')
        ;

    char consulta_verificar[200];
    snprintf(consulta_verificar, sizeof(consulta_verificar),
             "SELECT * FROM medico WHERE num_ced = %d", num_ced);
    PGresult *res_verificar = ejecutar_consulta_con_resultado(consulta_verificar);

    if (res_verificar == NULL || PQntuples(res_verificar) == 0)
    {
        printf("Médico con ID %d no encontrado.\n", num_ced);
        if (res_verificar != NULL)
            PQclear(res_verificar);
        return;
    }
    PQclear(res_verificar);

    do
    {
        printf("\n--- Campos disponibles para actualizar ---\n");
        printf("1. Nombre\n");
        printf("2. Apellidos\n");
        printf("3. Fecha de nacimiento\n");
        printf("4. Dirección\n");
        printf("5. Especialidad\n");
        printf("0. Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);
        while (getchar() != '\n')
            ;

        char nuevo_valor[200];
        char consulta[1000];

        switch (opcion)
        {
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
            while (getchar() != '\n')
                ;
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
    } while (opcion != 0);
}

void eliminar_medico()
{
    int num_ced;
    printf("Ingrese el ID del médico a eliminar: ");
    scanf("%d", &num_ced);

    printf("¿Está seguro de eliminar al médico con ID %d? (s/n): ", num_ced);
    char confirm;
    scanf(" %c", &confirm);

    if (confirm == 's' || confirm == 'S')
    {
        char consulta[200];
        snprintf(consulta, sizeof(consulta), "DELETE FROM medico WHERE num_ced = %d", num_ced);
        ejecutar_consulta(consulta);
        printf("Médico eliminado.\n");
    }
    else
    {
        printf("Operación cancelada.\n");
    }
}

char *medico_to_string(PGresult *res, int row)
{
    static char resultado[512];
    snprintf(resultado, sizeof(resultado),
             "Cédula: %s, Nombre: %s %s, Especialidad: %s",
             PQgetvalue(res, row, 0),  // num_ced
             PQgetvalue(res, row, 1),  // nombre
             PQgetvalue(res, row, 2),  // apellidos
             PQgetvalue(res, row, 3)); // especialidad
    return resultado;
}

// Formato: "num_ced|nombre|apellidos|especialidad"
char *crear_medico_fifo(const char *datos)
{
    static char resultado[512];
    char nombre[100], apellidos[100], especialidad[100];
    int num_ced;

    if (sscanf(datos, "%d|%99[^|]|%99[^|]|%99[^\n]",
               &num_ced, nombre, apellidos, especialidad) != 4)
    {
        strcpy(resultado, "Error: Formato de datos inválido");
        return resultado;
    }

    char consulta[500];
    snprintf(consulta, sizeof(consulta),
             "INSERT INTO medico (num_ced, nombre, apellidos, especialidad) "
             "VALUES (%d, '%s', '%s', '%s')",
             num_ced, nombre, apellidos, especialidad);

    if (ejecutar_consulta(consulta) == 0)
    {
        snprintf(resultado, sizeof(resultado), "✅ Médico creado exitosamente: %s %s (Cédula: %d)",
                 nombre, apellidos, num_ced);
    }
    else
    {
        strcpy(resultado, "❌ Error al crear médico");
    }

    return resultado;
}

char *listar_medicos_fifo()
{
    static char resultado[2048];
    resultado[0] = '\0';

    const char *consulta = "SELECT * FROM medico ORDER BY num_ced";
    PGresult *res = ejecutar_consulta_con_resultado(consulta);

    if (res == NULL)
    {
        strcpy(resultado, "Error al ejecutar consulta");
        return resultado;
    }

    int rows = PQntuples(res);
    if (rows == 0)
    {
        strcpy(resultado, "No hay médicos registrados");
        PQclear(res);
        return resultado;
    }

    snprintf(resultado, sizeof(resultado), "=== LISTA DE MÉDICOS (%d registros) ===\n", rows);

    for (int i = 0; i < rows; i++)
    {
        char linea[512];
        char *medico_str = medico_to_string(res, i);
        snprintf(linea, sizeof(linea), "%d. %s\n", i + 1, medico_str);

        if (strlen(resultado) + strlen(linea) < sizeof(resultado) - 100)
        {
            strcat(resultado, linea);
        }
        else
        {
            strcat(resultado, "... (más registros omitidos)\n");
            break;
        }
    }

    PQclear(res);
    return resultado;
}

char *buscar_medico_fifo(int num_ced)
{
    static char resultado[512];

    char consulta[100];
    snprintf(consulta, sizeof(consulta), "SELECT * FROM medico WHERE num_ced = %d", num_ced);

    PGresult *res = ejecutar_consulta_con_resultado(consulta);

    if (res == NULL || PQntuples(res) == 0)
    {
        snprintf(resultado, sizeof(resultado), "❌ Médico con cédula %d no encontrado", num_ced);
    }
    else
    {
        char *medico_str = medico_to_string(res, 0);
        snprintf(resultado, sizeof(resultado), "✅ MÉDICO ENCONTRADO\n%s", medico_str);
    }

    if (res != NULL)
        PQclear(res);
    return resultado;
}

char *actualizar_medico_fifo(int num_ced, int campo, const char *nuevo_valor)
{
    static char resultado[512];

    char consulta_verificar[200];
    snprintf(consulta_verificar, sizeof(consulta_verificar),
             "SELECT * FROM medico WHERE num_ced = %d", num_ced);
    PGresult *res_verificar = ejecutar_consulta_con_resultado(consulta_verificar);

    if (res_verificar == NULL || PQntuples(res_verificar) == 0)
    {
        snprintf(resultado, sizeof(resultado), "❌ Médico con cédula %d no encontrado", num_ced);
        if (res_verificar != NULL)
            PQclear(res_verificar);
        return resultado;
    }
    PQclear(res_verificar);

    char consulta[1000];
    const char *nombre_campo = "";

    switch (campo)
    {
    case 1: // Nombre
        nombre_campo = "nombre";
        snprintf(consulta, sizeof(consulta),
                 "UPDATE medico SET nombre = '%s' WHERE num_ced = %d", nuevo_valor, num_ced);
        break;

    case 2: // Apellidos
        nombre_campo = "apellidos";
        snprintf(consulta, sizeof(consulta),
                 "UPDATE medico SET apellidos = '%s' WHERE num_ced = %d", nuevo_valor, num_ced);
        break;

    case 3: // Especialidad
        nombre_campo = "especialidad";
        snprintf(consulta, sizeof(consulta),
                 "UPDATE medico SET especialidad = '%s' WHERE num_ced = %d", nuevo_valor, num_ced);
        break;

    default:
        strcpy(resultado, "❌ Campo inválido");
        return resultado;
    }

    if (ejecutar_consulta(consulta) == 0)
    {
        snprintf(resultado, sizeof(resultado), "✅ Campo '%s' actualizado correctamente para médico cédula %d",
                 nombre_campo, num_ced);
    }
    else
    {
        snprintf(resultado, sizeof(resultado), "❌ Error al actualizar campo '%s'", nombre_campo);
    }

    return resultado;
}

char *eliminar_medico_fifo(int num_ced)
{
    static char resultado[512];

    char consulta_verificar[200];
    snprintf(consulta_verificar, sizeof(consulta_verificar),
             "SELECT * FROM medico WHERE num_ced = %d", num_ced);
    PGresult *res_verificar = ejecutar_consulta_con_resultado(consulta_verificar);

    if (res_verificar == NULL || PQntuples(res_verificar) == 0)
    {
        snprintf(resultado, sizeof(resultado), "❌ Médico con cédula %d no encontrado", num_ced);
        if (res_verificar != NULL)
            PQclear(res_verificar);
        return resultado;
    }
    PQclear(res_verificar);

    char consulta[100];
    snprintf(consulta, sizeof(consulta), "DELETE FROM medico WHERE num_ced = %d", num_ced);

    if (ejecutar_consulta(consulta) == 0)
    {
        snprintf(resultado, sizeof(resultado), "✅ Médico con cédula %d eliminado exitosamente", num_ced);
    }
    else
    {
        snprintf(resultado, sizeof(resultado), "❌ Error al eliminar médico con cédula %d", num_ced);
    }

    return resultado;
}