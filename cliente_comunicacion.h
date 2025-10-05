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

// Variables globales de comunicación
extern int cliente_fifo_respuesta;
extern char fifo_respuesta[50];
extern pid_t mi_pid;

// Funciones de comunicación
void manejar_senal(int sig);
void enviar_comando(const char* comando);
int esperar_respuesta_con_timeout(int timeout_sec);
int esperar_respuesta_formateada(int timeout_sec, const char* tipo_formato);
void inicializar_comunicacion();
void limpiar_comunicacion();

#endif