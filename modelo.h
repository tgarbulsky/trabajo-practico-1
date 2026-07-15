#ifndef MODELO_H
#define MODELO_H

#include "matriz.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Enumerativo de unidades según el formato STL
enum unidades {MM, CM, M, IN, FT, MILS, UNITS}; 
typedef enum unidades unidades_t;

// Tabla de búsqueda de nombres de unidades
extern const char *units[];

typedef struct modelo modelo_t;

// --- Primitivas de Archivo --- //

// Verifica el encabezado y formato del archivo STL
// Asigna en unidades y maxlong los valores leídos
bool verificar_stl(FILE* f, unidades_t* unidades, size_t* maxlong);

// Lee un modelo del archivo f
// Pre: El archivo debe estar abierto en modo binario
modelo_t* leer_modelo(FILE* f, size_t maxlong, unidades_t unidades);

// Libera la memoria asociada al modelo
void destruir_modelo(void* modelo);

// --- Getters --- //

// Devuelve la matriz de coordenadas (puntos 3D) del modelo
matriz_t* modelo_obtener_coords(modelo_t* modelo);

// Obtiene una coordenada específica en el arreglo coord
void modelo_obtener_coord(modelo_t* modelo, size_t fila, float coord[2]);

// Devuelve la cantidad total de puntos (coordenadas).
size_t modelo_ncoords(modelo_t* modelo);

// Devuelve el arreglo de índices que definen las líneas.
size_t* modelo_obtener_lineas(modelo_t* modelo);

// Obtiene los índices de los dos puntos que forman la línea 'linea'
void modelo_obtener_linea(modelo_t* modelo, size_t linea, size_t* coord1, size_t* coord2);

// Devuelve la cantidad de líneas que componen el modelo.
size_t modelo_obtener_nlineas(modelo_t* modelo);

// Devuelve la etiqueta (nombre) del modelo (ej: TANQUE).
char* modelo_obtener_etiqueta(modelo_t* modelo);

#endif