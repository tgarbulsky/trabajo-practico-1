#ifndef MATRIZ_H
#define MATRIZ_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

typedef struct matriz matriz_t;

// Creadores y Destructor [1-3]
matriz_t *_matriz_crear(size_t n, size_t m);
void matriz_destruir(void *matriz);
matriz_t *matriz_crear_proy(size_t n);
matriz_t *matriz_crear_rotz(double angz);
matriz_t *matriz_crear_roty(double angy);
matriz_t *matriz_crear_rotx(double angx);
matriz_t *matriz_crear_tras(const float vector[4]);
matriz_t *matriz_crear_escalar(size_t n, float* factor);

// Getters y Setters [1, 5, 6]
size_t matriz_filas(const matriz_t *matriz);
size_t matriz_columnas(const matriz_t *matriz);
float matriz_obtener(const matriz_t *matriz, size_t fila, size_t columna);
void matriz_establecer(matriz_t *matriz, size_t fila, size_t columna, float valor);
void set_id_mx(matriz_t* mx_s);

// Operaciones [6, 7]
matriz_t *matriz_multiplicar(const matriz_t *a, const matriz_t *b);
matriz_t *matriz_aplicar(const matriz_t *matriz, const matriz_t *ps);
matriz_t *matriz_extender(const matriz_t *matriz);

#endif