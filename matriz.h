#ifndef MATRIZ_H
#define MATRIZ_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

// Tipo opaco que representa la estructura de una matriz
typedef struct matriz matriz_t;

// Crea una matriz dinamica de dimensiones 'n' filas por 'm' columnas
matriz_t *_matriz_crear(size_t n, size_t m);

// Libera la memoria dinamica asociada a la matriz
void matriz_destruir(void *matriz);

// Devuelve el numero de filas de la matriz
size_t matriz_filas(const matriz_t *matriz); 

// Devuelve el numero de columnas de la matriz
size_t matriz_columnas(const matriz_t *matriz);

// Obtiene el valor numerico almacenado en una posicion (fila, columna) especifica
float matriz_obtener(const matriz_t *matriz, size_t fila, size_t columna); 

// Asigna un valor flotante en la posicion (fila, columna) de la matriz
void matriz_establecer(matriz_t *matriz, size_t fila, size_t columna, float valor);

// Crea una matriz de proyeccion de tamaño n x n
matriz_t *matriz_crear_proy(size_t n);

// Crea una matriz de rotacion en el eje Z dado un angulo en radianes
matriz_t *matriz_crear_rotz(double angz);

// Crea una matriz de rotacion en el eje Y dado un angulo en radianes
matriz_t *matriz_crear_roty(double angy);

// Crea una matriz de rotacion en el eje X dado un angulo en radianes
matriz_t *matriz_crear_rotx(double angx);

// Crea una matriz de traslacion a partir de un vector tridimensional [x, y, z]
matriz_t *matriz_crear_tras(const float vector[3]);

// Crea una matriz de escalado para transformar el tamaño de un objeto
matriz_t *matriz_crear_escalar(size_t n, float* factor);

// Multiplica dos matrices (A x B) y retorna una nueva matriz resultado
matriz_t *matriz_multiplicar(const matriz_t *a, const matriz_t *b);

// Aplica una matriz de transformacion sobre una matriz de puntos o vertices
matriz_t *matriz_aplicar(const matriz_t *matriz, const matriz_t *ps);

// Retorna una nueva matriz extendida (por ejemplo, para coordenadas homogeneas)
matriz_t *matriz_extender(const matriz_t *matriz);

// Convierte la matriz provista en una matriz identidad (1s en la diagonal principal)
void set_id_mx(matriz_t* mx_s);

// Imprime el contenido de la matriz en la salida estandar para depuracion
void print_mx(matriz_t *mx);

#endif
