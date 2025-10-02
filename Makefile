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

CC = gcc
CFLAGS = -Wall -g -I/usr/include/postgresql
LIBS = -lpq

# Objetos del CRUD
CRUD_OBJS = database.o paciente.o medico.o medicamento.o receta.o

# Servidor del Hospital
servidor_hospital: servidor_hospital.o $(CRUD_OBJS)
	$(CC) $(CFLAGS) -o servidor_hospital servidor_hospital.o $(CRUD_OBJS) $(LIBS)

# Cliente del Hospital
cliente_hospital: cliente_hospital.o
	$(CC) $(CFLAGS) -o cliente_hospital cliente_hospital.o

# Objetos
servidor_hospital.o: servidor_hospital.c database.h paciente.h medico.h medicamento.h receta.h
	$(CC) $(CFLAGS) -c servidor_hospital.c

cliente_hospital.o: cliente_hospital.c
	$(CC) $(CFLAGS) -c cliente_hospital.c

# Objetos del CRUD
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

# Limpieza
clean:
	rm -f *.o servidor_hospital cliente_hospital
	rm -f /tmp/hospital_servidor /tmp/hospital_cliente_*_respuesta

# Instalar FIFOs
fifos:
	mkfifo /tmp/hospital_servidor 0666

.PHONY: clean fifos