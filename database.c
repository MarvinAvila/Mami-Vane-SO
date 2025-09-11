#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"

PGconn *conn = NULL;

PGconn* conectar_base_datos() {
    if (conn != NULL) {
        return conn;
    }
    

const char *conninfo = "dbname=hospital user=postgres password=2504 host=localhost port=5432";
    
    conn = PQconnectdb(conninfo);
    
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Error de conexión: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return NULL;
    }
    
    return conn;
}

void desconectar_base_datos() {
    if (conn != NULL) {
        PQfinish(conn);
        conn = NULL;
    }
}

int ejecutar_consulta(const char *consulta) {
    if (conn == NULL) {
        fprintf(stderr, "No hay conexión a la base de datos\n");
        return 1 ;
    }
    
    PGresult *res = PQexec(conn, consulta);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Error en la consulta: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return 1; // Error
    }

    PQclear(res);
    return 0; // Éxito
}

PGresult* ejecutar_consulta_con_resultado(const char *consulta) {
    if (conn == NULL) {
        fprintf(stderr, "No hay conexión a la base de datos\n");
        return NULL;
    }
    
    PGresult *res = PQexec(conn, consulta);
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error en consulta: %s", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }
    
    return res;
}