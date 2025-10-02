#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "receta.h"

void crear_receta()
{
    int num_exp, num_ced;
    char fecha_receta[11];

    printf("Número de expediente del paciente: ");
    scanf("%d", &num_exp);
    while (getchar() != '\n')
        ;

    printf("Número de cédula del médico: ");
    scanf("%d", &num_ced);
    while (getchar() != '\n')
        ;

    printf("Fecha de la receta (YYYY-MM-DD): ");
    scanf("%s", fecha_receta);
    while (getchar() != '\n')
        ;

    char consulta[500];
    snprintf(consulta, sizeof(consulta),
             "INSERT INTO receta (num_exp, num_ced, fecha_receta, lugar) "
             "VALUES (%d, %d, '%s', 'Tapachula, Chiapas')",
             num_exp, num_ced, fecha_receta);

    if (ejecutar_consulta(consulta))
        printf("Receta creada exitosamente.\n");
    else
        printf("Error: Verifique que paciente y médico existan.\n");
}

void listar_recetas()
{
    const char *consulta =
        "SELECT r.folio, r.fecha_receta, p.nombre || ' ' || p.apellidos as paciente, "
        "m.nombre || ' ' || m.apellidos as medico, r.lugar "
        "FROM receta r "
        "JOIN paciente p ON r.num_exp = p.num_exp "
        "JOIN medico m ON r.num_ced = m.num_ced "
        "ORDER BY r.folio";

    PGresult *res = ejecutar_consulta_con_resultado(consulta);
    if (res == NULL)
        return;

    int rows = PQntuples(res);
    printf("\n=== LISTA DE RECETAS ===\n");
    for (int i = 0; i < rows; i++)
    {
        printf("Folio: %s, Fecha: %s, Paciente: %s, Médico: %s, Lugar: %s\n",
               PQgetvalue(res, i, 0),  // folio
               PQgetvalue(res, i, 1),  // fecha_receta
               PQgetvalue(res, i, 2),  // paciente
               PQgetvalue(res, i, 3),  // medico
               PQgetvalue(res, i, 4)); // lugar
    }
    PQclear(res);
}

void buscar_receta()
{
    int folio;
    printf("Ingrese el folio de la receta a buscar: ");
    scanf("%d", &folio);

    char consulta[500];
    snprintf(consulta, sizeof(consulta),
             "SELECT r.folio, r.fecha_receta, p.nombre || ' ' || p.apellidos as paciente, "
             "m.nombre || ' ' || m.apellidos as medico, r.lugar "
             "FROM receta r "
             "JOIN paciente p ON r.num_exp = p.num_exp "
             "JOIN medico m ON r.num_ced = m.num_ced "
             "WHERE r.folio = %d",
             folio);

    PGresult *res = ejecutar_consulta_con_resultado(consulta);

    if (res == NULL || PQntuples(res) == 0)
    {
        printf("Receta con folio %d no encontrada.\n", folio);
    }
    else
    {
        printf("\n=== RECETA ENCONTRADA ===\n");
        printf("Folio: %s, Fecha: %s, Paciente: %s, Médico: %s, Lugar: %s\n",
               PQgetvalue(res, 0, 0),  // folio
               PQgetvalue(res, 0, 1),  // fecha_receta
               PQgetvalue(res, 0, 2),  // paciente
               PQgetvalue(res, 0, 3),  // medico
               PQgetvalue(res, 0, 4)); // lugar
    }

    if (res != NULL)
        PQclear(res);
}

void agregar_medicamento_receta()
{
    int folio, num_med, cantidad;

    printf("Folio de la receta: ");
    scanf("%d", &folio);

    printf("Número de medicamento: ");
    scanf("%d", &num_med);

    printf("Cantidad: ");
    scanf("%d", &cantidad);
    while (getchar() != '\n')
        ;

    char consulta[500];
    snprintf(consulta, sizeof(consulta),
             "INSERT INTO receta_medicamento (folio, num_med, cantidad) "
             "VALUES (%d, %d, %d)",
             folio, num_med, cantidad);

    if (ejecutar_consulta(consulta))
        printf("Medicamento agregado a la receta exitosamente.\n");
    else
        printf("Error: Verifique que la receta y medicamento existan.\n");
}

void listar_medicamentos_receta()
{
    int folio;
    printf("Ingrese el folio de la receta: ");
    scanf("%d", &folio);

    char consulta[500];
    snprintf(consulta, sizeof(consulta),
             "SELECT m.num_med, m.nombre, m.presentacion, rm.cantidad "
             "FROM receta_medicamento rm "
             "JOIN medicamento m ON rm.num_med = m.num_med "
             "WHERE rm.folio = %d "
             "ORDER BY m.nombre",
             folio);

    PGresult *res = ejecutar_consulta_con_resultado(consulta);
    if (res == NULL)
        return;

    int rows = PQntuples(res);
    printf("\n=== MEDICAMENTOS EN LA RECETA %d ===\n", folio);

    if (rows == 0)
    {
        printf("No hay medicamentos en esta receta.\n");
    }
    else
    {
        for (int i = 0; i < rows; i++)
        {
            printf("Medicamento: %s (%s), Cantidad: %s, Presentación: %s\n",
                   PQgetvalue(res, i, 1),  // nombre
                   PQgetvalue(res, i, 0),  // num_med
                   PQgetvalue(res, i, 3),  // cantidad
                   PQgetvalue(res, i, 2)); // presentacion
        }
    }
    PQclear(res);
}

void eliminar_receta()
{
    int folio;
    printf("Ingrese el folio de la receta a eliminar: ");
    scanf("%d", &folio);

    printf("¿Está seguro de eliminar la receta con folio %d? (s/n): ", folio);
    char confirm;
    scanf(" %c", &confirm);

    if (confirm == 's' || confirm == 'S')
    {
        char consulta[200];
        snprintf(consulta, sizeof(consulta), "DELETE FROM receta WHERE folio = %d", folio);

        if (ejecutar_consulta(consulta))
            printf("Receta eliminada exitosamente.\n");
        else
            printf("Error al eliminar la receta.\n");
    }
    else
    {
        printf("Operación cancelada.\n");
    }
}

char *receta_to_string(PGresult *res, int row)
{
    static char resultado[512];
    snprintf(resultado, sizeof(resultado),
             "Folio: %s, Fecha: %s, Paciente: %s, Médico: %s, Lugar: %s",
             PQgetvalue(res, row, 0),  // folio
             PQgetvalue(res, row, 1),  // fecha_receta
             PQgetvalue(res, row, 2),  // paciente
             PQgetvalue(res, row, 3),  // medico
             PQgetvalue(res, row, 4)); // lugar
    return resultado;
}

// Formato: "num_exp|num_ced|fecha_receta"
char *crear_receta_fifo(const char *datos)
{
    static char resultado[512];
    int num_exp, num_ced;
    char fecha_receta[11];

    if (sscanf(datos, "%d|%d|%10[^\n]", &num_exp, &num_ced, fecha_receta) != 3)
    {
        strcpy(resultado, "Error: Formato de datos inválido");
        return resultado;
    }

    char consulta[500];
    snprintf(consulta, sizeof(consulta),
             "INSERT INTO receta (num_exp, num_ced, fecha_receta, lugar) "
             "VALUES (%d, %d, '%s', 'Tapachula, Chiapas')",
             num_exp, num_ced, fecha_receta);

    if (ejecutar_consulta(consulta))
    {
        snprintf(resultado, sizeof(resultado), "✅ Receta creada exitosamente para paciente %d", num_exp);
    }
    else
    {
        strcpy(resultado, "❌ Error al crear receta");
    }

    return resultado;
}

char *listar_recetas_fifo()
{
    static char resultado[2048];
    resultado[0] = '\0';

    const char *consulta =
        "SELECT r.folio, r.fecha_receta, p.nombre || ' ' || p.apellidos as paciente, "
        "m.nombre || ' ' || m.apellidos as medico, r.lugar "
        "FROM receta r "
        "JOIN paciente p ON r.num_exp = p.num_exp "
        "JOIN medico m ON r.num_ced = m.num_ced "
        "ORDER BY r.folio";

    PGresult *res = ejecutar_consulta_con_resultado(consulta);
    if (res == NULL)
    {
        strcpy(resultado, "Error al ejecutar consulta");
        return resultado;
    }

    int rows = PQntuples(res);
    if (rows == 0)
    {
        strcpy(resultado, "No hay recetas registradas");
        PQclear(res);
        return resultado;
    }

    snprintf(resultado, sizeof(resultado), "=== LISTA DE RECETAS (%d registros) ===\n", rows);

    for (int i = 0; i < rows; i++)
    {
        char linea[512];
        char *receta_str = receta_to_string(res, i);
        snprintf(linea, sizeof(linea), "%d. %s\n", i + 1, receta_str);

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

char *buscar_receta_fifo(int folio)
{
    static char resultado[512];

    char consulta[500];
    snprintf(consulta, sizeof(consulta),
             "SELECT r.folio, r.fecha_receta, p.nombre || ' ' || p.apellidos as paciente, "
             "m.nombre || ' ' || m.apellidos as medico, r.lugar "
             "FROM receta r "
             "JOIN paciente p ON r.num_exp = p.num_exp "
             "JOIN medico m ON r.num_ced = m.num_ced "
             "WHERE r.folio = %d",
             folio);

    PGresult *res = ejecutar_consulta_con_resultado(consulta);

    if (res == NULL || PQntuples(res) == 0)
    {
        snprintf(resultado, sizeof(resultado), "❌ Receta con folio %d no encontrada", folio);
    }
    else
    {
        char *receta_str = receta_to_string(res, 0);
        snprintf(resultado, sizeof(resultado), "✅ RECETA ENCONTRADA\n%s", receta_str);
    }

    if (res != NULL)
        PQclear(res);
    return resultado;
}

char *agregar_medicamento_receta_fifo(int folio, int num_med, int cantidad)
{
    static char resultado[512];

    char consulta[500];
    snprintf(consulta, sizeof(consulta),
             "INSERT INTO receta_medicamento (folio, num_med, cantidad) "
             "VALUES (%d, %d, %d)",
             folio, num_med, cantidad);

    if (ejecutar_consulta(consulta))
    {
        snprintf(resultado, sizeof(resultado), "✅ Medicamento %d agregado a receta %d (cantidad: %d)",
                 num_med, folio, cantidad);
    }
    else
    {
        strcpy(resultado, "❌ Error al agregar medicamento a la receta");
    }

    return resultado;
}

char *listar_medicamentos_receta_fifo(int folio)
{
    static char resultado[2048];
    resultado[0] = '\0';

    char consulta[500];
    snprintf(consulta, sizeof(consulta),
             "SELECT m.num_med, m.nombre, m.presentacion, rm.cantidad "
             "FROM receta_medicamento rm "
             "JOIN medicamento m ON rm.num_med = m.num_med "
             "WHERE rm.folio = %d "
             "ORDER BY m.nombre",
             folio);

    PGresult *res = ejecutar_consulta_con_resultado(consulta);
    if (res == NULL)
    {
        strcpy(resultado, "Error al ejecutar consulta");
        return resultado;
    }

    int rows = PQntuples(res);
    snprintf(resultado, sizeof(resultado), "=== MEDICAMENTOS EN LA RECETA %d (%d medicamentos) ===\n", folio, rows);

    if (rows == 0)
    {
        strcat(resultado, "No hay medicamentos en esta receta.");
    }
    else
    {
        for (int i = 0; i < rows; i++)
        {
            char linea[256];
            snprintf(linea, sizeof(linea), "ID: %s, Nombre: %s, Presentación: %s, Cantidad: %s\n",
                     PQgetvalue(res, i, 0),  // num_med
                     PQgetvalue(res, i, 1),  // nombre
                     PQgetvalue(res, i, 2),  // presentacion
                     PQgetvalue(res, i, 3)); // cantidad

            if (strlen(resultado) + strlen(linea) < sizeof(resultado) - 100)
            {
                strcat(resultado, linea);
            }
            else
            {
                strcat(resultado, "... (más medicamentos omitidos)\n");
                break;
            }
        }
    }
    PQclear(res);
    return resultado;
}

char *eliminar_receta_fifo(int folio)
{
    static char resultado[512];

    char consulta_verificar[200];
    snprintf(consulta_verificar, sizeof(consulta_verificar),
             "SELECT * FROM receta WHERE folio = %d", folio);
    PGresult *res_verificar = ejecutar_consulta_con_resultado(consulta_verificar);

    if (res_verificar == NULL || PQntuples(res_verificar) == 0)
    {
        snprintf(resultado, sizeof(resultado), "❌ Receta con folio %d no encontrada", folio);
        if (res_verificar != NULL)
            PQclear(res_verificar);
        return resultado;
    }
    PQclear(res_verificar);

    char consulta[200];
    snprintf(consulta, sizeof(consulta), "DELETE FROM receta WHERE folio = %d", folio);

    if (ejecutar_consulta(consulta))
    {
        snprintf(resultado, sizeof(resultado), "✅ Receta con folio %d eliminada exitosamente", folio);
    }
    else
    {
        snprintf(resultado, sizeof(resultado), "❌ Error al eliminar receta con folio %d", folio);
    }

    return resultado;
}