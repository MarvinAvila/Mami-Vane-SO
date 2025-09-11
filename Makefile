# Compilador
CC = gcc

# Flags de compilación
CFLAGS = -Wall -I/usr/include/postgresql

# Flags de linkeo
LDFLAGS = -lpq

# Archivos fuente (añade aquí todos los .c que tengas en tu proyecto)
SRCS = main.c database.c paciente.c medico.c medicamento.c receta.c

# Archivos objeto
OBJS = $(SRCS:.c=.o)

# Nombre del ejecutable
TARGET = hospital_crud

# Regla por defecto
all: $(TARGET)

# Enlazado del ejecutable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compilación de cada archivo .c a .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar la compilación
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
