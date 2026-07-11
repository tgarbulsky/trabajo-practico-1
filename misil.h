#ifndef MISIL_H
#define MISIL_H

#include <stdbool.h>

typedef struct misil misil_t;

// Crea un misil en una posición inicial (x, y) y con un ángulo de disparo (phi)
misil_t *misil_crear(float x, float y, float phi);

// Libera la memoria utilizada por el misil
void misil_destruir(misil_t *m);

// Getters públicos para que 'juego.c' calcule colisiones y el motor gráfico lo dibuje
float misil_x(const misil_t *m);
float misil_y(const misil_t *m);
float misil_phi(const misil_t *m);

// Actualiza la posición del misil según el tiempo transcurrido (dt).
// Retorna 'false' si se le terminaron los 2 segundos de vida en el aire.
bool misil_actualizar(misil_t *m, float dt);

#endif