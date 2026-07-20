#ifndef MODELO_H
#define MODELO_H

#include "matriz.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

//Ennumerativo de unidades
enum unidades {MM, CM, M, IN, FT, MILS, UNITS}; //UNITS sirve de flag a la cantidad de unidades
typedef enum unidades unidades_t;

//Tabla de búsqueda de unidades
extern const char *units[];

typedef struct modelo modelo_t;

// Verifica el encabezado y formato del archivo stl
// Asigna en unidades las unidades de los modelos
// Asigna en maxlong la longitud de las etiquetas de los modelos
// Pre: El archivo fue abierto correctamente en lectura binaria
// Si el archivo es inválido o ocurre una falla, devuelve false
bool verificar_stl(FILE* f, unidades_t* unidades, size_t* maxlong);

// Creador
// Lee del archivo f un modelo
// Pre: El archivo f está abierto en lectura binaria
//      Las etiquetas tienen longitud maxlong
//      Los modelos vienen dados en unidades
// En caso de falla, devuelve NULL
modelo_t* leer_modelo(FILE* f, size_t maxlong, unidades_t unidades); 

// Destructor
// Libera la memoria de modelo
// Pre: el modelo fue creado
void destruir_modelo(void* modelo);

//Getters

// Devuelve un puntero a la matriz de coordenadas del modelo
// Pre: El modelo fue creado
matriz_t* modelo_obtener_coords(modelo_t* modelo);

// Asigna en coord la fila de la matriz de coordenadas del modelo
// Pre: El modelo fue creado
//      El modelo está en 3 dimensiones
void modelo_obtener_coord(modelo_t* modelo, size_t fila, float coord[3]);

// Devuelve la cantidad de coordenadas del modelo
// Pre: El modelo fue creado
size_t modelo_ncoords(modelo_t* modelo);

// Devuelve un puntero al arreglo de tuplas de puntos que definen las líneas del modelo
// Pre: El modelo fue creado 
size_t* modelo_obtener_lineas(modelo_t* modelo);

// Asigna en coord1 y coord2 los ordinales de los puntos de la línea linea del modelo
// Pre: El modelo fue creado y tiene al menos linea lineas 
void modelo_obtener_linea(modelo_t* modelo, size_t linea, size_t* coord1, size_t* coord2);

// Devuelve la cantidad de líneas del modelo
// Pre: El modelo fue creado
size_t modelo_obtener_nlineas(modelo_t* modelo);

// Devuelve un puntero a la etiqueta del modelo
// Pre: el modelo fue creado
char* modelo_obtener_etiqueta(modelo_t* modelo);
 
#endif
