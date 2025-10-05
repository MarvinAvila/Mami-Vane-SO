#include "cliente_ui.h"
#include "cliente_comunicacion.h"
#include "cliente_formatos.h"
#include <stdio.h>
#include <string.h>

// ===============================
// FUNCIONES DE MENÚS
// ===============================

void mostrar_menu_principal() {
    printf("\n");
    printf("╔════════════════════════════════════════════╗\n");
    printf("║            🏥 SISTEMA HOSPITAL             ║\n");
    printf("╠════════════════════════════════════════════╣\n");
    printf("║  1. 📋  Gestión de Pacientes               ║\n");
    printf("║  2. 👨‍⚕️   Gestión de Médicos                ║\n");
    printf("║  3. 💊  Gestión de Medicamentos            ║\n");
    printf("║  4. 📄  Gestión de Recetas                 ║\n");
    printf("║  5. ❓  Ayuda de comandos                  ║\n");  // ✅ NUEVO
    printf("║  0. 🚪  Salir del Sistema                  ║\n");
    printf("╚═════════════════════════════════════════════╝\n");
    printf("\n🔹 Seleccione una opción (0-5): ");  // Cambiar de 0-4 a 0-5
}
void mostrar_submenu_pacientes()
{
    printf("\n");
    printf("╔════════════════════════════════════════════╗\n");
    printf("║           📋 GESTIÓN DE PACIENTES          ║\n");
    printf("╠════════════════════════════════════════════╣\n");
    printf("║  1. 👤  Crear nuevo paciente               ║\n");
    printf("║  2. 📋  Listar todos los pacientes         ║\n");
    printf("║  3. 🔍  Buscar paciente por ID             ║\n");
    printf("║  4. ✏️   Actualizar datos de paciente      ║\n");
    printf("║  5. 🗑️   Eliminar paciente                 ║\n");
    printf("║  0. ↩️   Volver al menú principal          ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    printf("\n🔹 Seleccione una opción (0-5): ");
}

void mostrar_submenu_medicos()
{
    printf("\n");
    printf("╔════════════════════════════════════════════╗\n");
    printf("║           👨‍⚕️  GESTIÓN DE MÉDICOS           ║\n");
    printf("╠════════════════════════════════════════════╣\n");
    printf("║  1. 🆕  Registrar nuevo médico             ║\n");
    printf("║  2. 📊  Listar todos los médicos           ║\n");
    printf("║  3. 🔎  Buscar médico por cédula           ║\n");
    printf("║  4. 📝  Actualizar datos de médico         ║\n");
    printf("║  5. ❌  Eliminar médico                    ║\n");
    printf("║  0. ↩️   Volver al menú principal          ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    printf("\n🔹 Seleccione una opción (0-5): ");
}

void mostrar_submenu_medicamentos()
{
    printf("\n");
    printf("╔════════════════════════════════════════════╗\n");
    printf("║          💊 GESTIÓN DE MEDICAMENTOS        ║\n");
    printf("╠════════════════════════════════════════════╣\n");
    printf("║  1. 💊  Crear nuevo medicamento            ║\n");
    printf("║  2. 📦  Listar todos los medicamentos      ║\n");
    printf("║  3. 🔎  Buscar medicamento por ID          ║\n");
    printf("║  4. ✏️   Actualizar medicamento            ║\n");
    printf("║  5. 🗑️   Eliminar medicamento              ║\n");
    printf("║  0. ↩️   Volver al menú principal          ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    printf("\n🔹 Seleccione una opción (0-5): ");
}

void mostrar_submenu_recetas()
{
    printf("\n");
    printf("╔════════════════════════════════════════════╗\n");
    printf("║           📄 GESTIÓN DE RECETAS            ║\n");
    printf("╠════════════════════════════════════════════╣\n");
    printf("║  1. 📄  Crear nueva receta                 ║\n");
    printf("║  2. 📋  Listar todas las recetas           ║\n");
    printf("║  3. 🔍  Buscar receta por folio            ║\n");
    printf("║  4. 💊  Agregar medicamento a receta       ║\n");
    printf("║  5. 📦  Ver medicamentos de receta         ║\n");
    printf("║  6. 🗑️   Eliminar receta                   ║\n");
    printf("║  0. ↩️   Volver al menú principal          ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    printf("\n🔹 Seleccione una opción (0-6): ");
}

// ===============================
// FUNCIONES INTERACTIVAS - PACIENTES (ACTUALIZADAS)
// ===============================

void crear_paciente_interactivo()
{
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
    while (getchar() != '\n')
        ;

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
    while (getchar() != '\n')
        ;

    char comando[1000];
    snprintf(comando, sizeof(comando), "crear_paciente|%s|%s|%s|%s|%.2f|%.2f|%c|%d",
             nombre, apellidos, fecha_nac, direccion, peso, estatura, sexo, num_ced_medico);

    // USAR NUEVO SISTEMA
    operacion_segura(comando, "simple");
}

void buscar_paciente_interactivo()
{
    int id;
    printf("\n🔍 ID del paciente a buscar: ");
    scanf("%d", &id);
    while (getchar() != '\n')
        ;

    char comando[100];
    snprintf(comando, sizeof(comando), "buscar_paciente|%d", id);

    // USAR NUEVO SISTEMA
    operacion_segura(comando, "detalle");
}

void actualizar_paciente_interactivo()
{
    int id, campo;
    char nuevo_valor[200];

    printf("\n");
    printf("┌─────────────────────────────────────────────┐\n");
    printf("│          ✏️  ACTUALIZAR PACIENTE           │\n");
    printf("└─────────────────────────────────────────────┘\n");

    printf("🔹 ID del paciente a actualizar: ");
    scanf("%d", &id);
    while (getchar() != '\n')
        ;

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
    while (getchar() != '\n')
        ;

    printf("🔹 Nuevo valor: ");
    fgets(nuevo_valor, sizeof(nuevo_valor), stdin);
    nuevo_valor[strcspn(nuevo_valor, "\n")] = 0;

    char comando[500];
    snprintf(comando, sizeof(comando), "actualizar_paciente|%d|%d|%s", id, campo, nuevo_valor);

    // USAR NUEVO SISTEMA
    operacion_segura(comando, "simple");
}

void eliminar_paciente_interactivo()
{
    int id;
    char confirmacion;

    printf("\n");
    printf("┌─────────────────────────────────────────────┐\n");
    printf("│           🗑️  ELIMINAR PACIENTE            │\n");
    printf("└─────────────────────────────────────────────┘\n");

    printf("🔹 ID del paciente a eliminar: ");
    scanf("%d", &id);
    while (getchar() != '\n')
        ;

    printf("⚠️  ¿Está seguro de eliminar al paciente con ID %d? (s/n): ", id);
    scanf(" %c", &confirmacion);
    while (getchar() != '\n')
        ;

    if (confirmacion == 's' || confirmacion == 'S')
    {
        char comando[100];
        snprintf(comando, sizeof(comando), "eliminar_paciente|%d", id);

        // USAR NUEVO SISTEMA
        operacion_segura(comando, "simple");
    }
    else
    {
        printf("❌ Operación cancelada\n");
    }
}

// ===============================
// FUNCIONES INTERACTIVAS - MÉDICOS (ACTUALIZADAS)
// ===============================

void crear_medico_interactivo()
{
    char nombre[100], apellidos[100], especialidad[100];
    int num_ced;

    printf("\n");
    printf("┌─────────────────────────────────────────────┐\n");
    printf("│            🆕 CREAR MÉDICO                 │\n");
    printf("└─────────────────────────────────────────────┘\n");

    printf("🔹 Número de cédula: ");
    scanf("%d", &num_ced);
    while (getchar() != '\n')
        ;

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

    // USAR NUEVO SISTEMA
    operacion_segura(comando, "simple");
}

void buscar_medico_interactivo()
{
    int num_ced;
    printf("\n🔍 Cédula del médico a buscar: ");
    scanf("%d", &num_ced);
    while (getchar() != '\n')
        ;

    char comando[100];
    snprintf(comando, sizeof(comando), "buscar_medico|%d", num_ced);

    // USAR NUEVO SISTEMA
    operacion_segura(comando, "detalle");
}

void actualizar_medico_interactivo()
{
    int num_ced, campo;
    char nuevo_valor[200];

    printf("\n");
    printf("┌─────────────────────────────────────────────┐\n");
    printf("│          📝 ACTUALIZAR MÉDICO              │\n");
    printf("└─────────────────────────────────────────────┘\n");

    printf("🔹 Cédula del médico a actualizar: ");
    scanf("%d", &num_ced);
    while (getchar() != '\n')
        ;

    printf("\n📝 Campos disponibles para actualizar:\n");
    printf("1. Nombre\n");
    printf("2. Apellidos\n");
    printf("3. Especialidad\n");

    printf("\n🔹 Seleccione el campo a actualizar (1-3): ");
    scanf("%d", &campo);
    while (getchar() != '\n')
        ;

    printf("🔹 Nuevo valor: ");
    fgets(nuevo_valor, sizeof(nuevo_valor), stdin);
    nuevo_valor[strcspn(nuevo_valor, "\n")] = 0;

    char comando[500];
    snprintf(comando, sizeof(comando), "actualizar_medico|%d|%d|%s", num_ced, campo, nuevo_valor);

    // USAR NUEVO SISTEMA
    operacion_segura(comando, "simple");
}

void eliminar_medico_interactivo()
{
    int num_ced;
    char confirmacion;

    printf("\n");
    printf("┌─────────────────────────────────────────────┐\n");
    printf("│            ❌ ELIMINAR MÉDICO              │\n");
    printf("└─────────────────────────────────────────────┘\n");

    printf("🔹 Cédula del médico a eliminar: ");
    scanf("%d", &num_ced);
    while (getchar() != '\n')
        ;

    printf("⚠️  ¿Está seguro de eliminar al médico con cédula %d? (s/n): ", num_ced);
    scanf(" %c", &confirmacion);
    while (getchar() != '\n')
        ;

    if (confirmacion == 's' || confirmacion == 'S')
    {
        char comando[100];
        snprintf(comando, sizeof(comando), "eliminar_medico|%d", num_ced);

        // USAR NUEVO SISTEMA
        operacion_segura(comando, "simple");
    }
    else
    {
        printf("❌ Operación cancelada\n");
    }
}

// ===============================
// FUNCIONES INTERACTIVAS - MEDICAMENTOS (ACTUALIZADAS)
// ===============================

void crear_medicamento_interactivo()
{
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
    while (getchar() != '\n')
        ;

    printf("🔹 Stock inicial: ");
    scanf("%d", &stock);
    while (getchar() != '\n')
        ;

    char comando[1000];
    snprintf(comando, sizeof(comando), "crear_medicamento|%s|%s|%s|%s|%d",
             nombre, descripcion, presentacion, fecha_caducidad, stock);

    // USAR NUEVO SISTEMA
    operacion_segura(comando, "simple");
}

void buscar_medicamento_interactivo()
{
    int id;
    printf("\n🔍 ID del medicamento a buscar: ");
    scanf("%d", &id);
    while (getchar() != '\n')
        ;

    char comando[100];
    snprintf(comando, sizeof(comando), "buscar_medicamento|%d", id);

    // USAR NUEVO SISTEMA
    operacion_segura(comando, "detalle");
}

void actualizar_medicamento_interactivo()
{
    int id, campo;
    char nuevo_valor[200];

    printf("\n");
    printf("┌─────────────────────────────────────────────┐\n");
    printf("│         ✏️  ACTUALIZAR MEDICAMENTO         │\n");
    printf("└─────────────────────────────────────────────┘\n");

    printf("🔹 ID del medicamento a actualizar: ");
    scanf("%d", &id);
    while (getchar() != '\n')
        ;

    printf("\n📝 Campos disponibles para actualizar:\n");
    printf("1. Nombre\n");
    printf("2. Descripción\n");
    printf("3. Presentación\n");
    printf("4. Fecha de caducidad\n");
    printf("5. Stock\n");

    printf("\n🔹 Seleccione el campo a actualizar (1-5): ");
    scanf("%d", &campo);
    while (getchar() != '\n')
        ;

    printf("🔹 Nuevo valor: ");
    fgets(nuevo_valor, sizeof(nuevo_valor), stdin);
    nuevo_valor[strcspn(nuevo_valor, "\n")] = 0;

    char comando[500];
    snprintf(comando, sizeof(comando), "actualizar_medicamento|%d|%d|%s", id, campo, nuevo_valor);

    // USAR NUEVO SISTEMA
    operacion_segura(comando, "simple");
}

void eliminar_medicamento_interactivo()
{
    int id;
    char confirmacion;

    printf("\n");
    printf("┌─────────────────────────────────────────────┐\n");
    printf("│         🗑️  ELIMINAR MEDICAMENTO           │\n");
    printf("└─────────────────────────────────────────────┘\n");

    printf("🔹 ID del medicamento a eliminar: ");
    scanf("%d", &id);
    while (getchar() != '\n')
        ;

    printf("⚠️  ¿Está seguro de eliminar el medicamento con ID %d? (s/n): ", id);
    scanf(" %c", &confirmacion);
    while (getchar() != '\n')
        ;

    if (confirmacion == 's' || confirmacion == 'S')
    {
        char comando[100];
        snprintf(comando, sizeof(comando), "eliminar_medicamento|%d", id);

        // USAR NUEVO SISTEMA
        operacion_segura(comando, "simple");
    }
    else
    {
        printf("❌ Operación cancelada\n");
    }
}

// ===============================
// FUNCIONES INTERACTIVAS - RECETAS (ACTUALIZADAS)
// ===============================

void crear_receta_interactivo()
{
    int num_exp, num_ced;
    char fecha_receta[11];

    printf("\n");
    printf("┌─────────────────────────────────────────────┐\n");
    printf("│            📄 CREAR RECETA                 │\n");
    printf("└─────────────────────────────────────────────┘\n");

    printf("🔹 Número de expediente del paciente: ");
    scanf("%d", &num_exp);
    while (getchar() != '\n')
        ;

    printf("🔹 Número de cédula del médico: ");
    scanf("%d", &num_ced);
    while (getchar() != '\n')
        ;

    printf("🔹 Fecha de la receta (YYYY-MM-DD): ");
    scanf("%s", fecha_receta);
    while (getchar() != '\n')
        ;

    char comando[500];
    snprintf(comando, sizeof(comando), "crear_receta|%d|%d|%s",
             num_exp, num_ced, fecha_receta);

    // USAR NUEVO SISTEMA
    operacion_segura(comando, "simple");
}

void buscar_receta_interactivo()
{
    int folio;
    printf("\n🔍 Folio de la receta a buscar: ");
    scanf("%d", &folio);
    while (getchar() != '\n')
        ;

    char comando[100];
    snprintf(comando, sizeof(comando), "buscar_receta|%d", folio);

    // USAR NUEVO SISTEMA
    operacion_segura(comando, "detalle");
}

void agregar_medicamento_receta_interactivo()
{
    int folio, num_med, cantidad;

    printf("\n");
    printf("┌─────────────────────────────────────────────┐\n");
    printf("│      💊 AGREGAR MEDICAMENTO A RECETA       │\n");
    printf("└─────────────────────────────────────────────┘\n");

    printf("🔹 Folio de la receta: ");
    scanf("%d", &folio);
    while (getchar() != '\n')
        ;

    printf("🔹 ID del medicamento: ");
    scanf("%d", &num_med);
    while (getchar() != '\n')
        ;

    printf("🔹 Cantidad: ");
    scanf("%d", &cantidad);
    while (getchar() != '\n')
        ;

    char comando[500];
    snprintf(comando, sizeof(comando), "agregar_medicamento_receta|%d|%d|%d",
             folio, num_med, cantidad);

    // USAR NUEVO SISTEMA
    operacion_segura(comando, "simple");
}

void listar_medicamentos_receta_interactivo()
{
    int folio;
    printf("\n🔍 Folio de la receta: ");
    scanf("%d", &folio);
    while (getchar() != '\n')
        ;

    char comando[100];
    snprintf(comando, sizeof(comando), "listar_medicamentos_receta|%d", folio);

    // USAR NUEVO SISTEMA
    operacion_segura(comando, "detalle");
}

void eliminar_receta_interactivo()
{
    int folio;
    char confirmacion;

    printf("\n");
    printf("┌─────────────────────────────────────────────┐\n");
    printf("│           🗑️  ELIMINAR RECETA              │\n");
    printf("└─────────────────────────────────────────────┘\n");

    printf("🔹 Folio de la receta a eliminar: ");
    scanf("%d", &folio);
    while (getchar() != '\n')
        ;

    printf("⚠️  ¿Está seguro de eliminar la receta con folio %d? (s/n): ", folio);
    scanf(" %c", &confirmacion);
    while (getchar() != '\n')
        ;

    if (confirmacion == 's' || confirmacion == 'S')
    {
        char comando[100];
        snprintf(comando, sizeof(comando), "eliminar_receta|%d", folio);

        // USAR NUEVO SISTEMA
        operacion_segura(comando, "simple");
    }
    else
    {
        printf("❌ Operación cancelada\n");
    }
}

// ===============================
// PROCESADORES DE SUBMENÚS (YA ESTÁN CORRECTOS)
// ===============================

void procesar_submenu_pacientes()
{
    int opcion;
    do
    {
        mostrar_submenu_pacientes();

        if (scanf("%d", &opcion) != 1)
        {
            while (getchar() != '\n')
                ;
            printf("❌ Opción inválida\n");
            continue;
        }
        while (getchar() != '\n')
            ;

        switch (opcion)
        {
        case 1:
            crear_paciente_interactivo();
            break;
        case 2:
            operacion_segura("listar_pacientes", "pacientes");
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

        if (opcion != 0)
        {
            printf("\n⏎ Presione Enter para continuar...");
            getchar();
        }

    } while (opcion != 0);
}

void procesar_submenu_medicos()
{
    int opcion;
    do
    {
        mostrar_submenu_medicos();

        if (scanf("%d", &opcion) != 1)
        {
            while (getchar() != '\n')
                ;
            printf("❌ Opción inválida\n");
            continue;
        }
        while (getchar() != '\n')
            ;

        switch (opcion)
        {
        case 1:
            crear_medico_interactivo();
            break;
        case 2:
            operacion_segura("listar_medicos", "medicos");
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

        if (opcion != 0)
        {
            printf("\n⏎ Presione Enter para continuar...");
            getchar();
        }

    } while (opcion != 0);
}

void procesar_submenu_medicamentos()
{
    int opcion;
    do
    {
        mostrar_submenu_medicamentos();

        if (scanf("%d", &opcion) != 1)
        {
            while (getchar() != '\n')
                ;
            printf("❌ Opción inválida\n");
            continue;
        }
        while (getchar() != '\n')
            ;

        switch (opcion)
        {
        case 1:
            crear_medicamento_interactivo();
            break;
        case 2:
            operacion_segura("listar_medicamentos", "medicamentos");
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

        if (opcion != 0)
        {
            printf("\n⏎ Presione Enter para continuar...");
            getchar();
        }

    } while (opcion != 0);
}

void procesar_submenu_recetas()
{
    int opcion;
    do
    {
        mostrar_submenu_recetas();

        if (scanf("%d", &opcion) != 1)
        {
            while (getchar() != '\n')
                ;
            printf("❌ Opción inválida\n");
            continue;
        }
        while (getchar() != '\n')
            ;

        switch (opcion)
        {
        case 1:
            crear_receta_interactivo();
            break;
        case 2:
            operacion_segura("listar_recetas", "recetas");
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

        if (opcion != 0)
        {
            printf("\n⏎ Presione Enter para continuar...");
            getchar();
        }

    } while (opcion != 0);
}