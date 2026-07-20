CC = gcc
CFLAGS = -Wall -Werror -std=c99 -pedantic -g
LDLIBS = -lSDL2 -lm
OBJS = matriz.o modelo.o lista.o pila.o mundo.o animaciones.o interfaz2d.o tanque.o misil.o obstaculo.o main.o
PROGRAM = battlezone

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDLIBS)


matriz.o: matriz.c matriz.h
modelo.o: modelo.c modelo.h matriz.h
lista.o: lista.c lista.h
pila.o: pila.c pila.h
mundo.o: mundo.c mundo.h modelo.h matriz.h lista.h pila.h
animaciones.o: animaciones.c animaciones.h modelo.h matriz.h pila.h
interfaz2d.o: interfaz2d.c interfaz2d.h modelo.h
tanque.o: tanque.c tanque.h modelo.h lista.h
misil.o: misil.c misil.h modelo.h lista.h tanque.h
obstaculo.o: obstaculo.c obstaculo.h modelo.h
main.o: main.c mundo.h modelo.h matriz.h lista.h pila.h animaciones.h interfaz2d.h tanque.h misil.h obstaculo.h

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(PROGRAM)

.PHONY: all clean
