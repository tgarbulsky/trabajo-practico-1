CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g $(shell sdl2-config --cflags)
LDLIBS = $(shell sdl2-config --libs) -lm

OBJS = main.o matriz.o pila.o lista.o cola.o modelo.o obstaculo.o \
       tanque.o misil.o stl.o mundo.o animaciones.o motor3d.o

all: battlezone

battlezone: $(OBJS)
	$(CC) $(OBJS) -o battlezone $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c modelo.h obstaculo.h tanque.h stl.h matriz.h pila.h lista.h mundo.h animaciones.h
matriz.o: matriz.c matriz.h
pila.o: pila.c pila.h
lista.o: lista.c lista.h
cola.o: cola.c cola.h
modelo.o: modelo.c modelo.h lista.h
obstaculo.o: obstaculo.c obstaculo.h modelo.h
tanque.o: tanque.c tanque.h obstaculo.h modelo.h misil.h
misil.o: misil.c misil.h
stl.o: stl.c stl.h
mundo.o: mundo.c mundo.h lista.h tanque.h obstaculo.h modelo.h
animaciones.o: animaciones.c animaciones.h
motor3d.o: motor3d.c

valgrind: battlezone
	valgrind --leak-check=full --show-leak-kinds=all ./battlezone

clean:
	rm -f *.o battlezone

.PHONY: all clean valgrind
