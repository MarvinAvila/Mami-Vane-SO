#ifndef MEDICO_H
#define MEDICO_H

#include <libpq-fe.h>

void crear_medico();
void listar_medicos();
void buscar_medico();
void actualizar_medico();
void eliminar_medico();

// Nuevas funciones para uso por FIFO
char* crear_medico_fifo(const char* datos);
char* listar_medicos_fifo();
char* buscar_medico_fifo(int num_ced);
char* actualizar_medico_fifo(int num_ced, int campo, const char* nuevo_valor);
char* eliminar_medico_fifo(int num_ced);

// Funciones auxiliares
char* medico_to_string(PGresult *res, int row);


#endif