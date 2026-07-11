#ifndef MATRIZ_H
#define MATRIZ_H

#include <stddef.h>

// TDA Matriz de floats de dimensiones arbitrarias (filas x columnas).
// Se usa tanto para las transformaciones 4x4 del motor 3D como para
// almacenar los puntos de los modelos (n x 3).
typedef struct matriz matriz_t;

// Crea una matriz de filas x columnas inicializada en cero.
// Devuelve NULL si falla la reserva de memoria.
matriz_t *_matriz_crear(size_t filas, size_t columnas);

// Crea la matriz identidad de n x n.
matriz_t *matriz_crear_identidad(size_t n);

// Crea la matriz de perspectiva (Mper) de n x n.
// Deja w = -z al aplicarla, de modo que los puntos delante de la
// cámara quedan con profundidad positiva.
matriz_t *matriz_crear_mper(size_t n);

// Crea la matriz 4x4 de rotación alrededor del eje Z.
matriz_t *matriz_crear_mz(float angulo);

// Crea la matriz 4x4 de rotación alrededor del eje Y.
matriz_t *matriz_crear_my(float angulo);

// Crea la matriz 4x4 de traslación por el vector v = {tx, ty, tz}.
matriz_t *matriz_crear_mt(const float v[3]);

// Destruye la matriz. Acepta NULL.
void matriz_destruir(matriz_t *m);

// Getters de dimensiones.
size_t matriz_filas(const matriz_t *m);
size_t matriz_columnas(const matriz_t *m);

// Lee el valor en (fila, columna).
// Pre: fila < filas, columna < columnas.
float matriz_obtener(const matriz_t *m, size_t fila, size_t columna);

// Escribe el valor en (fila, columna).
// Pre: fila < filas, columna < columnas.
void matriz_establecer(matriz_t *m, size_t fila, size_t columna, float valor);

// Multiplica a x b y devuelve una matriz nueva.
// Pre: columnas de a == filas de b.
// Devuelve NULL si las dimensiones no son compatibles o falla la memoria.
matriz_t *matriz_multiplicar(const matriz_t *a, const matriz_t *b);

// Aplica la transformación t (4x4) a una matriz de puntos (n x 3).
// Devuelve una matriz nueva de n x 3 donde cada fila es:
//   columna 0: x proyectado (x/w)
//   columna 1: y proyectado (y/w)
//   columna 2: w (profundidad, para descartar puntos con w < 1)
// Si w es aproximadamente cero para un punto, sus coordenadas quedan en 0
// y su profundidad en 0 (el punto queda descartado por el clipping).
// Devuelve NULL si falla la memoria.
matriz_t *matriz_aplicar(const matriz_t *t, const matriz_t *puntos);

#endif
