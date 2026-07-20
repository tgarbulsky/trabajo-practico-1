#ifndef MODELO_H
#define MODELO_H

#include "matriz.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Enumeración de las unidades de medida soportadas por los modelos
// UNITS actua como contador total del numero de unidades disponibles
enum unidades {MM, CM, M, IN, FT, MILS, UNITS};
typedef enum unidades unidades_t;

// Arreglo de cadenas de texto con los nombres de cada unidad
extern const char *units[];

// Tipo opaco para el modelo tridimensional
typedef struct modelo modelo_t;

// Lee el encabezado de un archivo STL para verificar su validez
// Guarda las unidades de medida encontradas y la longitud maxima de cadenas
bool verificar_stl(FILE* f, unidades_t* unidades, size_t* maxlong);

// Lee la geometria del modelo desde el archivo y crea la estructura modelo_t
modelo_t* leer_modelo(FILE* f, size_t maxlong, unidades_t unidades); 

// Libera toda la memoria dinamica asociada a una instancia de modelo_t
void destruir_modelo(void* modelo);

// Retorna la matriz interna que contiene todas las coordenadas de los vertices
matriz_t* modelo_obtener_coords(modelo_t* modelo);

// Copia las coordenadas [x, y, z] de una fila especifica en el arreglo destino
void modelo_obtener_coord(modelo_t* modelo, size_t fila, float coord[3]);

// Retorna la cantidad total de vertices (coordenadas) que tiene el modelo
size_t modelo_ncoords(modelo_t* modelo);

// Retorna el arreglo completo de conexiones de lineas entre vertices
size_t* modelo_obtener_lineas(modelo_t* modelo);

// Obtiene los indices de los dos vertices (coord1 y coord2) que forman una linea especifica
void modelo_obtener_linea(modelo_t* modelo, size_t linea, size_t* coord1, size_t* coord2);

// Retorna la cantidad total de lineas que componen la malla del modelo
size_t modelo_obtener_nlineas(modelo_t* modelo);

// Retorna el nombre o etiqueta identificadora asignada al modelo
char* modelo_obtener_etiqueta(modelo_t* modelo);

#endif
