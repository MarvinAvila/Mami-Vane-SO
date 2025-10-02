#ifndef PACIENTE_H
#define PACIENTE_H

#include <libpq-fe.h>

void crear_paciente();
void listar_pacientes();
void buscar_paciente();
void actualizar_paciente();
void eliminar_paciente();

// Nuevas funciones para uso por FIFO
char* crear_paciente_fifo(const char* datos);
char* listar_pacientes_fifo();
char* buscar_paciente_fifo(int num_exp);
char* actualizar_paciente_fifo(int num_exp, int campo, const char* nuevo_valor);
char* eliminar_paciente_fifo(int num_exp);

// Funciones auxiliares
char* paciente_to_string(PGresult *res, int row);

#endif