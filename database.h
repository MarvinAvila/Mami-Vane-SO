#ifndef DATABASE_H
#define DATABASE_H

#include <libpq-fe.h>

PGconn *conectar_base_datos();
void desconectar_base_datos();
int ejecutar_consulta(const char *consulta);
PGresult* ejecutar_consulta_con_resultado(const char *consulta);

#endif