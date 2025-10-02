#ifndef RECETA_H
#define RECETA_H

#include <libpq-fe.h>

void crear_receta();
void listar_recetas();
void buscar_receta();
void agregar_medicamento_receta();
void listar_medicamentos_receta();
void eliminar_receta();

// Nuevas funciones para uso por FIFO
char* crear_receta_fifo(const char* datos);
char* listar_recetas_fifo();
char* buscar_receta_fifo(int folio);
char* agregar_medicamento_receta_fifo(int folio, int num_med, int cantidad);
char* listar_medicamentos_receta_fifo(int folio);
char* eliminar_receta_fifo(int folio);

// Funciones auxiliares
char* receta_to_string(PGresult *res, int row);

#endif