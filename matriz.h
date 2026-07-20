#ifndef MATRIZ_H
#define MATRIZ_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

typedef struct matriz matriz_t;

// Creador
// Crea una matriz de n x m
// En caso de falla, devuelve NULL 
matriz_t *_matriz_crear(size_t n, size_t m);

// Destructor
// Libera la memoria asociada a la matriz
void matriz_destruir(void *matriz);

// Getters

// Devuelve la cantidad de filas de la matriz
// Pre: La matriz debe estar creada 
size_t matriz_filas(const matriz_t *matriz); //Cantidad de filas de matriz

// Devuelve la cantidad de columnas de la matriz
// Pre: La matriz debe estar creada
size_t matriz_columnas(const matriz_t *matriz); //Cantidad de columnas de matriz

// Devuelve el elemento (fila, columna) de la matriz
// Pre: La matriz debe estar creada y tener al menos fila filas y columna columnas
float matriz_obtener(const matriz_t *matriz, size_t fila, size_t columna); //Devuelve matriz[fila][columna] (contando de 1)

// Setter
// Asigna en (fila, columna) de la matriz valor
// Pre: La matriz debe estar creada y tener al menos fila filas y columna columnas
void matriz_establecer(matriz_t *matriz, size_t fila, size_t columna, float valor); //Asigna valor en matriz[fila][columna] (contando de 1)

// Matrices

// Crea una matriz cuadrada de dimensión n de proyección sobre x(n-1)=-1 en coordenadas homogéneas
// En caso de falla, devuelve NULL
matriz_t *matriz_crear_proy(size_t n); //Proyección (sobre z=-1)

// Crea una matriz de rotación de eje z y ángulo angz en coordenadas homogéneas
// En caso de falla, devuelve NULL
matriz_t *matriz_crear_rotz(double angz);

// Crea una matriz de rotación de eje y y ángulo angy en coordenadas homogéneas
// En caso de falla, devuelve NULL
matriz_t *matriz_crear_roty(double angy);

// Crea una matriz de rotación de eje x y ángulo angx en coordenadas homogéneas
// En caso de falla, devuelve NULL
matriz_t *matriz_crear_rotx(double angx);

// Crea una matriz de traslación con vector incremental vector[3]={dx, dy, dz}
// En caso de falla, devuelve NULL
matriz_t *matriz_crear_tras(const float vector[3]);

// Crea una matriz de homotecia de dimensión n
// La escala de la n-ésima coordenada será el n-ésimo elementos del arreglo factor
// Pre: El arreglo factor tiene al menos n elementos
// En caso de falla, devuelve NULL
matriz_t *matriz_crear_escalar(size_t n, float* factor); //Matriz de escala factor

// Operaciones de matrices

// Producto de matrices
// Crea una nueva matriz que contiene el producto a * b
// Pre: Las matrices a, b están creadas
// En caso de falla o si las dimensiones no permiten el producto, devuelve NULL
matriz_t *matriz_multiplicar(const matriz_t *a, const matriz_t *b);

// -Transpone y extiende cada elemento de ps con 1: (x, y, z, 1)T
// -Realiza el producto de esta matriz con matriz: (X, Y, Z, W)T
// -Devuelve la matriz con las coordenadas normalizadas y W extendidas y traspuestas: (X/W, Y/W, W, 1)T
// Pre: Ambas matrices estás creadas
//      matriz tiene una fila más que ps columnas
// En caso de falla, devuelve NULL
matriz_t *matriz_aplicar(const matriz_t *matriz, const matriz_t *ps); //Aplica la transformación "matriz" a los vectores de "ps"

// Crea la matriz traspuesta y extendida con 1 a coordenadas homogéneas de matriz
// Pre: matriz está creada
// En caso de falla, devuelve NULL
matriz_t *matriz_extender(const matriz_t *matriz);

// Asigna en mx_s la matriz identidad
// Pre: mx_s está creada y es cuadrada
void set_id_mx(matriz_t* mx_s);

//Debug
// Imprime la matriz mx por stdout
// Pre: la matriz está creada
void print_mx(matriz_t *mx);
//

#endif
