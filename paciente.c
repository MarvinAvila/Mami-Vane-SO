#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "paciente.h"

void crear_paciente()
{
    char nombre[100], apellidos[100], direccion[200], sexo;
    char fecha_nac[11];
    float peso, estatura;
    int num_ced_medico;

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

    printf("Peso (kg) : ");
    scanf("%f", &peso);

    printf("Estatura (Metros) : ");
    scanf("%f", &estatura);

    printf("Sexo (M/F): ");
    scanf(" %c", &sexo);

    printf("Número de cédula del médico (0 si no tiene): ");
    scanf("%d", &num_ced_medico);

    char consulta[1000];
    if (num_ced_medico == 0)
    {
        snprintf(consulta, sizeof(consulta),
                 "INSERT INTO paciente (nombre, apellidos, fecha_nac, direccion, peso, estatura, sexo) "
                 "VALUES ('%s', '%s', '%s', '%s', %.2f, %.2f, '%c')",
                 nombre, apellidos, fecha_nac, direccion, peso, estatura, sexo);
    }
    else
    {
        snprintf(consulta, sizeof(consulta),
                 "INSERT INTO paciente (nombre, apellidos, fecha_nac, direccion, peso, estatura, sexo, num_ced) "
                 "VALUES ('%s', '%s', '%s', '%s', %.2f, %.2f, '%c', %d)",
                 nombre, apellidos, fecha_nac, direccion, peso, estatura, sexo, num_ced_medico);
    }

    ejecutar_consulta(consulta);
    printf("Paciente creado exitosamente.\n");
}

void listar_pacientes()
{
    const char *consulta = "SELECT * FROM paciente ORDER BY num_exp";
    PGresult *res = ejecutar_consulta_con_resultado(consulta);

    if (res == NULL)
        return;

    int rows = PQntuples(res);
    printf("\n=== LISTA DE PACIENTES ===\n");
    for (int i = 0; i < rows; i++)
    {
        printf("ID: %s, Nombre: %s %s, Edad: %s, Peso: %s, Estatura: %s\n",
               PQgetvalue(res, i, 0),  // num_exp
               PQgetvalue(res, i, 1),  // nombre
               PQgetvalue(res, i, 2),  // apellidos
               PQgetvalue(res, i, 4),  // edad
               PQgetvalue(res, i, 6),  // peso
               PQgetvalue(res, i, 7)); // estatura
    }

    PQclear(res);
}

// Función para buscar paciente por ID
void buscar_paciente()
{
    int num_exp;
    printf("Ingrese el ID del paciente a buscar: ");
    scanf("%d", &num_exp);

    char consulta[100];
    snprintf(consulta, sizeof(consulta), "SELECT * FROM paciente WHERE num_exp = %d", num_exp);

    PGresult *res = ejecutar_consulta_con_resultado(consulta);

    if (res == NULL || PQntuples(res) == 0)
    {
        printf("Paciente con ID %d no encontrado.\n", num_exp);
    }
    else
    {
        printf("\n=== PACIENTE ENCONTRADO ===\n");
        printf("ID: %s, Nombre: %s %s, Edad: %s, Peso: %s, Estatura: %s\n",
               PQgetvalue(res, 0, 0),  // num_exp
               PQgetvalue(res, 0, 1),  // nombre
               PQgetvalue(res, 0, 2),  // apellidos
               PQgetvalue(res, 0, 4),  // edad
               PQgetvalue(res, 0, 6),  // peso
               PQgetvalue(res, 0, 7)); // estatura
    }

    if (res != NULL)
        PQclear(res);
}

// Función para actualizar paciente
void actualizar_paciente()
{
    int num_exp, opcion;
    printf("Ingrese el ID del paciente a actualizar: ");
    scanf("%d", &num_exp);
    while (getchar() != '\n')
        ; // limpiar buffer

    // Verificar si existe
    char consulta_verificar[200];
    snprintf(consulta_verificar, sizeof(consulta_verificar),
             "SELECT * FROM paciente WHERE num_exp = %d", num_exp);
    PGresult *res_verificar = ejecutar_consulta_con_resultado(consulta_verificar);

    if (res_verificar == NULL || PQntuples(res_verificar) == 0)
    {
        printf("Paciente con ID %d no encontrado.\n", num_exp);
        if (res_verificar != NULL)
            PQclear(res_verificar);
        return;
    }
    PQclear(res_verificar);

    // Menú de actualización
    do
    {
        printf("\n--- Campos disponibles para actualizar ---\n");
        printf("1. Nombre\n");
        printf("2. Apellidos\n");
        printf("3. Fecha de nacimiento\n");
        printf("4. Dirección\n");
        printf("5. Peso\n");
        printf("6. Estatura\n");
        printf("7. Sexo\n");
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
                     "UPDATE paciente SET nombre = '%s' WHERE num_exp = %d",
                     nuevo_valor, num_exp);
            ejecutar_consulta(consulta);
            printf("Nombre actualizado correctamente.\n");
            break;

        case 2:
            printf("Nuevos apellidos: ");
            fgets(nuevo_valor, sizeof(nuevo_valor), stdin);
            nuevo_valor[strcspn(nuevo_valor, "\n")] = 0;
            snprintf(consulta, sizeof(consulta),
                     "UPDATE paciente SET apellidos = '%s' WHERE num_exp = %d",
                     nuevo_valor, num_exp);
            ejecutar_consulta(consulta);
            printf("Apellidos actualizados correctamente.\n");
            break;

        case 3:
            printf("Nueva fecha de nacimiento (YYYY-MM-DD): ");
            scanf("%s", nuevo_valor);
            while (getchar() != '\n')
                ;
            snprintf(consulta, sizeof(consulta),
                     "UPDATE paciente SET fecha_nac = '%s' WHERE num_exp = %d",
                     nuevo_valor, num_exp);
            if (ejecutar_consulta(consulta) == 0)
                printf("Fecha de nacimiento actualizada.\n");
            else
                printf("Error: la nueva fecha no cumple con la restricción (>=18 años).\n");
            break;

        case 4:
            printf("Nueva dirección: ");
            fgets(nuevo_valor, sizeof(nuevo_valor), stdin);
            nuevo_valor[strcspn(nuevo_valor, "\n")] = 0;
            snprintf(consulta, sizeof(consulta),
                     "UPDATE paciente SET direccion = '%s' WHERE num_exp = %d",
                     nuevo_valor, num_exp);
            ejecutar_consulta(consulta);
            printf("Dirección actualizada.\n");
            break;

        case 5:
        {
            float peso;
            printf("Nuevo peso: ");
            scanf("%f", &peso);
            snprintf(consulta, sizeof(consulta),
                     "UPDATE paciente SET peso = %.2f WHERE num_exp = %d",
                     peso, num_exp);
            ejecutar_consulta(consulta);
            printf("Peso actualizado.\n");
            break;
        }

        case 6:
        {
            float estatura;
            printf("Nueva estatura: ");
            scanf("%f", &estatura);
            snprintf(consulta, sizeof(consulta),
                     "UPDATE paciente SET estatura = %.2f WHERE num_exp = %d",
                     estatura, num_exp);
            ejecutar_consulta(consulta);
            printf("Estatura actualizada.\n");
            break;
        }

        case 7:
        {
            char sexo;
            printf("Nuevo sexo (M/F): ");
            scanf(" %c", &sexo);
            snprintf(consulta, sizeof(consulta),
                     "UPDATE paciente SET sexo = '%c' WHERE num_exp = %d",
                     sexo, num_exp);
            if (ejecutar_consulta(consulta) == 0)
                printf("Sexo actualizado.\n");
            else
                printf("Error: valor no válido (solo M/F).\n");
            break;
        }

        case 0:
            printf("Saliendo de actualización...\n");
            break;

        default:
            printf("Opción no válida.\n");
        }

    } while (opcion != 0);
}

// Función para eliminar paciente
void eliminar_paciente()
{
    int num_exp;
    printf("Ingrese el ID del paciente a eliminar: ");
    scanf("%d", &num_exp);

    // Confirmar eliminación
    printf("¿Está seguro de eliminar al paciente con ID %d? (s/n): ", num_exp);
    char confirmacion;
    scanf(" %c", &confirmacion);

    if (confirmacion == 's' || confirmacion == 'S')
    {
        char consulta[100];
        snprintf(consulta, sizeof(consulta), "DELETE FROM paciente WHERE num_exp = %d", num_exp);

        ejecutar_consulta(consulta);
        printf("Paciente eliminado exitosamente.\n");
    }
    else
    {
        printf("Eliminación cancelada.\n");
    }
}

// Función auxiliar para convertir fila de resultado a string
char *paciente_to_string(PGresult *res, int row)
{
    static char resultado[512];
    snprintf(resultado, sizeof(resultado),
             "ID: %s, Nombre: %s %s, Fecha Nac: %s, Edad: %s, Dirección: %s, Peso: %s kg, Estatura: %s m, Sexo: %s",
             PQgetvalue(res, row, 0),  // num_exp
             PQgetvalue(res, row, 1),  // nombre
             PQgetvalue(res, row, 2),  // apellidos
             PQgetvalue(res, row, 3),  // fecha_nac
             PQgetvalue(res, row, 4),  // edad
             PQgetvalue(res, row, 5),  // direccion
             PQgetvalue(res, row, 6),  // peso
             PQgetvalue(res, row, 7),  // estatura
             PQgetvalue(res, row, 8)); // sexo
    return resultado;
}

// Crear paciente desde datos recibidos por FIFO
// Formato: "nombre|apellidos|fecha_nac|direccion|peso|estatura|sexo|num_ced"
char *crear_paciente_fifo(const char *datos)
{
    static char resultado[512];
    char nombre[100], apellidos[100], fecha_nac[11], direccion[200], sexo;
    float peso, estatura;
    int num_ced_medico;

    // Parsear datos
    if (sscanf(datos, "%99[^|]|%99[^|]|%10[^|]|%199[^|]|%f|%f|%c|%d",
               nombre, apellidos, fecha_nac, direccion, &peso, &estatura, &sexo, &num_ced_medico) < 7)
    {
        strcpy(resultado, "Error: Formato de datos inválido");
        return resultado;
    }

    char consulta[1000];
    if (num_ced_medico == 0)
    {
        snprintf(consulta, sizeof(consulta),
                 "INSERT INTO paciente (nombre, apellidos, fecha_nac, direccion, peso, estatura, sexo) "
                 "VALUES ('%s', '%s', '%s', '%s', %.2f, %.2f, '%c')",
                 nombre, apellidos, fecha_nac, direccion, peso, estatura, sexo);
    }
    else
    {
        snprintf(consulta, sizeof(consulta),
                 "INSERT INTO paciente (nombre, apellidos, fecha_nac, direccion, peso, estatura, sexo, num_ced) "
                 "VALUES ('%s', '%s', '%s', '%s', %.2f, %.2f, '%c', %d)",
                 nombre, apellidos, fecha_nac, direccion, peso, estatura, sexo, num_ced_medico);
    }

    if (ejecutar_consulta(consulta) == 0)
    {
        snprintf(resultado, sizeof(resultado), "✅ Paciente creado exitosamente: %s %s", nombre, apellidos);
    }
    else
    {
        strcpy(resultado, "❌ Error al crear paciente");
    }

    return resultado;
}

// Listar pacientes y devolver resultado como string
char *listar_pacientes_fifo()
{
    static char resultado[2048];
    resultado[0] = '\0';

    const char *consulta = "SELECT * FROM paciente ORDER BY num_exp";
    PGresult *res = ejecutar_consulta_con_resultado(consulta);

    if (res == NULL)
    {
        strcpy(resultado, "Error al ejecutar consulta");
        return resultado;
    }

    int rows = PQntuples(res);
    if (rows == 0)
    {
        strcpy(resultado, "No hay pacientes registrados");
        PQclear(res);
        return resultado;
    }

    snprintf(resultado, sizeof(resultado), "=== LISTA DE PACIENTES (%d registros) ===\n", rows);

    for (int i = 0; i < rows; i++)
    {
        char linea[512];
        char *paciente_str = paciente_to_string(res, i);
        snprintf(linea, sizeof(linea), "%d. %s\n", i + 1, paciente_str);

        // Concatenar si hay espacio
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

// Buscar paciente por ID y devolver resultado como string
char *buscar_paciente_fifo(int num_exp)
{
    static char resultado[512];

    char consulta[100];
    snprintf(consulta, sizeof(consulta), "SELECT * FROM paciente WHERE num_exp = %d", num_exp);

    PGresult *res = ejecutar_consulta_con_resultado(consulta);

    if (res == NULL || PQntuples(res) == 0)
    {
        snprintf(resultado, sizeof(resultado), "❌ Paciente con ID %d no encontrado", num_exp);
    }
    else
    {
        char *paciente_str = paciente_to_string(res, 0);
        snprintf(resultado, sizeof(resultado), "✅ PACIENTE ENCONTRADO\n%s", paciente_str);
    }

    if (res != NULL)
        PQclear(res);
    return resultado;
}

// Actualizar paciente específico
char *actualizar_paciente_fifo(int num_exp, int campo, const char *nuevo_valor)
{
    static char resultado[512];

    // Verificar si existe
    char consulta_verificar[200];
    snprintf(consulta_verificar, sizeof(consulta_verificar),
             "SELECT * FROM paciente WHERE num_exp = %d", num_exp);
    PGresult *res_verificar = ejecutar_consulta_con_resultado(consulta_verificar);

    if (res_verificar == NULL || PQntuples(res_verificar) == 0)
    {
        snprintf(resultado, sizeof(resultado), "❌ Paciente con ID %d no encontrado", num_exp);
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
                 "UPDATE paciente SET nombre = '%s' WHERE num_exp = %d", nuevo_valor, num_exp);
        break;

    case 2: // Apellidos
        nombre_campo = "apellidos";
        snprintf(consulta, sizeof(consulta),
                 "UPDATE paciente SET apellidos = '%s' WHERE num_exp = %d", nuevo_valor, num_exp);
        break;

    case 3: // Fecha nacimiento
        nombre_campo = "fecha_nac";
        snprintf(consulta, sizeof(consulta),
                 "UPDATE paciente SET fecha_nac = '%s' WHERE num_exp = %d", nuevo_valor, num_exp);
        break;

    case 4: // Dirección
        nombre_campo = "direccion";
        snprintf(consulta, sizeof(consulta),
                 "UPDATE paciente SET direccion = '%s' WHERE num_exp = %d", nuevo_valor, num_exp);
        break;

    case 5: // Peso
        nombre_campo = "peso";
        snprintf(consulta, sizeof(consulta),
                 "UPDATE paciente SET peso = %s WHERE num_exp = %d", nuevo_valor, num_exp);
        break;

    case 6: // Estatura
        nombre_campo = "estatura";
        snprintf(consulta, sizeof(consulta),
                 "UPDATE paciente SET estatura = %s WHERE num_exp = %d", nuevo_valor, num_exp);
        break;

    case 7: // Sexo
        nombre_campo = "sexo";
        snprintf(consulta, sizeof(consulta),
                 "UPDATE paciente SET sexo = '%s' WHERE num_exp = %d", nuevo_valor, num_exp);
        break;

    default:
        strcpy(resultado, "❌ Campo inválido");
        return resultado;
    }

    if (ejecutar_consulta(consulta) == 0)
    {
        snprintf(resultado, sizeof(resultado), "✅ Campo '%s' actualizado correctamente para paciente ID %d",
                 nombre_campo, num_exp);
    }
    else
    {
        snprintf(resultado, sizeof(resultado), "❌ Error al actualizar campo '%s'", nombre_campo);
    }

    return resultado;
}

// Eliminar paciente
char *eliminar_paciente_fifo(int num_exp)
{
    static char resultado[512];

    // Verificar si existe
    char consulta_verificar[200];
    snprintf(consulta_verificar, sizeof(consulta_verificar),
             "SELECT * FROM paciente WHERE num_exp = %d", num_exp);
    PGresult *res_verificar = ejecutar_consulta_con_resultado(consulta_verificar);

    if (res_verificar == NULL || PQntuples(res_verificar) == 0)
    {
        snprintf(resultado, sizeof(resultado), "❌ Paciente con ID %d no encontrado", num_exp);
        if (res_verificar != NULL)
            PQclear(res_verificar);
        return resultado;
    }
    PQclear(res_verificar);

    char consulta[100];
    snprintf(consulta, sizeof(consulta), "DELETE FROM paciente WHERE num_exp = %d", num_exp);

    if (ejecutar_consulta(consulta) == 0)
    {
        snprintf(resultado, sizeof(resultado), "✅ Paciente con ID %d eliminado exitosamente", num_exp);
    }
    else
    {
        snprintf(resultado, sizeof(resultado), "❌ Error al eliminar paciente con ID %d", num_exp);
    }

    return resultado;
}