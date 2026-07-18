# Nombre del ejecutable final
TARGET = battlezone

# compilador y flags
CC = gcc
CFLAGS = -Wall -std=c11 -pedantic -g
LDFLAGS = -lSDL2 -lm

#lista de archivos objeto
OBJS = main.o \
       matriz.o \
       pila.o \
       lista.o \
       modelo.o \
       mundo.o \
       tanque.o \
       misil.o \
       obstaculo.o \
       interfaz2d.o \
       animaciones.o

# Regla principal
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Reglas para cada archivo objeto

main.o: main.c mundo.h animaciones.h matriz.h modelo.h lista.h pila.h
matriz.o: matriz.c matriz.h
pila.o: pila.c pila.h
lista.o: lista.c lista.h
modelo.o: modelo.c modelo.h matriz.h
mundo.o: mundo.c mundo.h matriz.h modelo.h pila.h lista.h
tanque.o: tanque.c tanque.h mundo.h
misil.o: misil.c misil.h mundo.h
obstaculo.o: obstaculo.c obstaculo.h mundo.h
interfaz2d.o: interfaz2d.c interfaz2d.h animaciones.h
animaciones.o: animaciones.c animaciones.h mundo.h matriz.h pila.h

clean:
	rm -f $(OBJS) $(TARGET)

clean_objs:
	rm -f $(OBJS)

#Valgrind y las supresiones
valgrind: $(TARGET)
	valgrind --leak-check=full --suppressions=suppressions_20261_tp1.supp ./$(TARGET)
