#include "cliente_comunicacion.h"
#include "cliente_ui.h"
#include <stdio.h>

int main()
{
    // Inicializar comunicación (FIFOs, señales, etc.)
    inicializar_comunicacion();

    int opcion_principal;
    do
    {
        mostrar_menu_principal();

        if (scanf("%d", &opcion_principal) != 1)
        {
            while (getchar() != '\n')
                ;
            printf("❌ Opción inválida\n");
            continue;
        }
        while (getchar() != '\n')
            ;

        switch (opcion_principal)
        {
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
        case 5: // ✅ NUEVO - AYUDA
            operacion_segura("ayuda", "simple");
            break;
        case 0: // ✅ MODIFICADO - ahora envía "salir" al servidor
            operacion_segura("salir", "simple");
            limpiar_comunicacion();
            break;
        default:
            printf("❌ Opción no válida\n");
        }

    } while (opcion_principal != 0);
    // Limpieza final
    limpiar_comunicacion();

    return 0;
}