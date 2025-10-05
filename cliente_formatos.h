#ifndef CLIENTE_FORMATOS_H
#define CLIENTE_FORMATOS_H

void mostrar_lista_compacta_pacientes(const char* respuesta_servidor);
void mostrar_lista_compacta_medicos(const char* respuesta_servidor);
void mostrar_lista_compacta_medicamentos(const char* respuesta_servidor);
void mostrar_lista_compacta_recetas(const char* respuesta_servidor);
void mostrar_detalle_compacto(const char* respuesta_servidor, const char* titulo);

#endif