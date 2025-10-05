#Hospital Crud

# # Compilador
# CC = gcc

# # Flags de compilación
# CFLAGS = -Wall -I/usr/include/postgresql

# # Flags de linkeo
# LDFLAGS = -lpq

# # Archivos fuente (añade aquí todos los .c que tengas en tu proyecto)
# SRCS = main.c database.c paciente.c medico.c medicamento.c receta.c

# # Archivos objeto
# OBJS = $(SRCS:.c=.o)

# # Nombre del ejecutable
# TARGET = hospital_crud

# # Regla por defecto
# all: $(TARGET)

# # Enlazado del ejecutable
# $(TARGET): $(OBJS)
# 	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# # Compilación de cada archivo .c a .o
# %.o: %.c
# 	$(CC) $(CFLAGS) -c $< -o $@

# # Limpiar la compilación
# clean:
# 	rm -f $(OBJS) $(TARGET)

# .PHONY: all clean


#Hospital Crud Con FIFOS

# Hospital Crud

# Compilador
CC = gcc

# Flags de compilación
CFLAGS = -Wall -g -I/usr/include/postgresql

# Flags de linkeo  
LDFLAGS = -lpq

# Archivos fuente del CRUD (manteniendo tu estructura original)
CRUD_SRCS = database.c paciente.c medico.c medicamento.c receta.c
CRUD_OBJS = $(CRUD_SRCS:.c=.o)

# Archivos del cliente modularizado
CLIENT_SRCS = cliente_hospital.c cliente_comunicacion.c cliente_ui.c cliente_formatos.c
CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)

# Nombre de los ejecutables
SERV_TARGET = servidor_hospital
CLIENT_TARGET = cliente_hospital

# Regla por defecto - compila ambos
all: $(SERV_TARGET) $(CLIENT_TARGET)

# Servidor del Hospital
$(SERV_TARGET): servidor_hospital.o $(CRUD_OBJS)
	$(CC) $(CFLAGS) -o $(SERV_TARGET) servidor_hospital.o $(CRUD_OBJS) $(LDFLAGS)

# Cliente del Hospital (modularizado)
$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $(CLIENT_TARGET) $(CLIENT_OBJS)

# Objeto del servidor
servidor_hospital.o: servidor_hospital.c database.h paciente.h medico.h medicamento.h receta.h
	$(CC) $(CFLAGS) -c servidor_hospital.c

# Objetos del cliente modularizado
cliente_hospital.o: cliente_hospital.c cliente_comunicacion.h cliente_ui.h cliente_formatos.h
	$(CC) $(CFLAGS) -c cliente_hospital.c

cliente_comunicacion.o: cliente_comunicacion.c cliente_comunicacion.h cliente_formatos.h
	$(CC) $(CFLAGS) -c cliente_comunicacion.c

cliente_ui.o: cliente_ui.c cliente_ui.h cliente_comunicacion.h cliente_formatos.h
	$(CC) $(CFLAGS) -c cliente_ui.c

cliente_formatos.o: cliente_formatos.c cliente_formatos.h
	$(CC) $(CFLAGS) -c cliente_formatos.c

# Objetos del CRUD (manteniendo tu estructura original)
database.o: database.c database.h
	$(CC) $(CFLAGS) -c database.c

paciente.o: paciente.c paciente.h database.h
	$(CC) $(CFLAGS) -c paciente.c

medico.o: medico.c medico.h database.h
	$(CC) $(CFLAGS) -c medico.c

medicamento.o: medicamento.c medicamento.h database.h
	$(CC) $(CFLAGS) -c medicamento.c

receta.o: receta.c receta.h database.h
	$(CC) $(CFLAGS) -c receta.c

# Reglas para compilar solo servidor o solo cliente
servidor: $(SERV_TARGET)

cliente: $(CLIENT_TARGET)

# Limpieza
clean:
	rm -f *.o $(SERV_TARGET) $(CLIENT_TARGET)
	rm -f /tmp/hospital_servidor /tmp/hospital_cliente_*_respuesta

# Instalar FIFOs
fifos:
	mkfifo /tmp/hospital_servidor 0666

.PHONY: all servidor cliente clean fifos