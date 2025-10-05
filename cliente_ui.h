#ifndef CLIENTE_UI_H
#define CLIENTE_UI_H

// Funciones de menús
void mostrar_menu_principal();
void mostrar_submenu_pacientes();
void mostrar_submenu_medicos();
void mostrar_submenu_medicamentos();
void mostrar_submenu_recetas();

// Funciones interactivas de pacientes
void crear_paciente_interactivo();
void buscar_paciente_interactivo();
void actualizar_paciente_interactivo();
void eliminar_paciente_interactivo();

// Funciones interactivas de médicos
void crear_medico_interactivo();
void buscar_medico_interactivo();
void actualizar_medico_interactivo();
void eliminar_medico_interactivo();

// Funciones interactivas de medicamentos
void crear_medicamento_interactivo();
void buscar_medicamento_interactivo();
void actualizar_medicamento_interactivo();
void eliminar_medicamento_interactivo();

// Funciones interactivas de recetas
void crear_receta_interactivo();
void buscar_receta_interactivo();
void agregar_medicamento_receta_interactivo();
void listar_medicamentos_receta_interactivo();
void eliminar_receta_interactivo();

// Procesadores de submenús
void procesar_submenu_pacientes();
void procesar_submenu_medicos();
void procesar_submenu_medicamentos();
void procesar_submenu_recetas();

#endif