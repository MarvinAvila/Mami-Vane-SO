#ifndef CLIENTE_COMUNICACION_H
#define CLIENTE_COMUNICACION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/select.h>

#define TAM_BUFFER 2048
#define FIFO_SERVIDOR "/tmp/hospital_servidor"

// Variable global
extern pid_t mi_pid;

// Funciones de comunicación
void manejar_senal(int sig);
void enviar_comando(const char* comando);
int operacion_segura(const char* comando, const char* tipo_formato);
int operacion_simple(const char* comando);
void inicializar_comunicacion();
void limpiar_comunicacion();

#endif