#ifndef MEDICAMENTO_H
#define MEDICAMENTO_H

#include <libpq-fe.h>

void crear_medicamento();
void listar_medicamentos();
void buscar_medicamento();
void actualizar_medicamento();
void eliminar_medicamento();

// Nuevas funciones para uso por FIFO
char* crear_medicamento_fifo(const char* datos);
char* listar_medicamentos_fifo();
char* buscar_medicamento_fifo(int num_med);
char* actualizar_medicamento_fifo(int num_med, int campo, const char* nuevo_valor);
char* eliminar_medicamento_fifo(int num_med);

// Funciones auxiliares
char* medicamento_to_string(PGresult *res, int row);

#endif