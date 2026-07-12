#ifndef MODELO_H
#define MODELO_H

#include <stddef.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "matriz.h"

// Estructura opaca para representar la malla geométrica de un objeto
typedef struct modelo modelo_t;

// Crea un modelo vacío reservando memoria para una cantidad específica de puntos (vértices)
modelo_t *modelo_crear(size_t cantidad_puntos);

// Destruye el modelo y libera su memoria asociada
void modelo_destruir(modelo_t *m);

// Permite establecer las coordenadas de un punto específico del modelo
// Pre: indice < cantidad_puntos
void modelo_establecer_punto(modelo_t *m, size_t indice, float x, float y);

// Obtiene la cantidad de puntos que componen el modelo
size_t modelo_obtener_cantidad_puntos(const modelo_t *m);

// Dibuja el modelo en la pantalla utilizando SDL2.
// Aplica la matriz de transformación dada (que combina posición, rotación y escala de la entidad).
void modelo_dibujar(const modelo_t *m, const matriz_t *transformacion, SDL_Renderer *renderer);

#endif // MODELO_H