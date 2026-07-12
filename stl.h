#ifndef STL_H
#define STL_H

#include "modelo.h"

// Lee un archivo binario STL interpretando manualmente el formato Little Endian.
// Genera un TDA modelo_t dinámico proyectando las facetas en el plano 2D.
//
// Pre: ruta_archivo es una cadena válida con la ubicación del archivo .stl.
// Post: devuelve un puntero a modelo_t con los vértices cargados listos para
//       dibujar, o NULL si ocurrió un error de lectura o memoria.
modelo_t *stl_cargar_modelo(const char *ruta_archivo);

#endif // STL_H