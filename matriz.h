#ifndef MATRIZ_H
#define MATRIZ_H

#include <stddef.h>

// TDA Matriz dinámico.
typedef struct matriz matriz_t;

// Crea una matriz de filas x columnas inicializada en cero.
matriz_t *matriz_crear(size_t filas, size_t columnas);

// Crea la matriz identidad de n x n.
matriz_t *matriz_crear_identidad(size_t n);

// Crea la matriz de traslación de 3x3 para coordenadas homogéneas 2D.
matriz_t *matriz_crear_traslacion(float tx, float ty);

// Crea la matriz de rotación de 3x3 en el eje Z (para el plano 2D del juego).
matriz_t *matriz_crear_rotacion(float angulo);

// Crea la matriz de escalado de 3x3.
matriz_t *matriz_crear_escalado(float factor);

// Destruye la matriz. Acepta NULL.
void matriz_destruir(matriz_t *m);

// Getters de dimensiones.
size_t matriz_filas(const matriz_t *m);
size_t matriz_columnas(const matriz_t *m);

// Getters y Setters
float matriz_obtener(const matriz_t *m, size_t fila, size_t columna);
void matriz_establecer(matriz_t *m, size_t fila, size_t columna, float valor);

// Multiplica a x b y devuelve una matriz nueva.
matriz_t *matriz_multiplicar(const matriz_t *a, const matriz_t *b);

// Aplica la transformación m (3x3) a un punto de dos coordenadas (x, y).
// Modifica los valores apuntados por x e y directamente (estilo Nico).
void matriz_aplicar(const matriz_t *m, float *x, float *y);

#endif // MATRIZ_H