#include "modelo.h"
#include <stdlib.h>

// Estructura interna para representar un punto bidimensional plano
typedef struct {
    float x;
    float y;
} punto2d_t;

// Estructura principal del TDA Modelo
struct modelo {
    punto2d_t *puntos;
    size_t cantidad_puntos;
};

modelo_t *modelo_crear(size_t cantidad_puntos) {
    if (cantidad_puntos == 0) return NULL;

    modelo_t *m = malloc(sizeof(modelo_t));
    if (m == NULL) return NULL;

    m->puntos = malloc(cantidad_puntos * sizeof(punto2d_t));
    if (m->puntos == NULL) {
        free(m);
        return NULL;
    }

    m->cantidad_puntos = cantidad_puntos;
    return m;
}

void modelo_destruir(modelo_t *m) {
    if (m == NULL) return;
    free(m->puntos);
    free(m);
}

void modelo_establecer_punto(modelo_t *m, size_t indice, float x, float y) {
    if (m == NULL || indice >= m->cantidad_puntos) return;
    m->puntos[indice].x = x;
    m->puntos[indice].y = y;
}

size_t modelo_obtener_cantidad_puntos(const modelo_t *m) {
    return m ? m->cantidad_puntos : 0;
}

void modelo_dibujar(const modelo_t *m, const matriz_t *transformacion, SDL_Renderer *renderer) {
    if (m == NULL || renderer == NULL || m->cantidad_puntos < 2) return;

    // Iteramos por los puntos del modelo dibujando líneas entre vértices consecutivos
    for (size_t i = 0; i < m->cantidad_puntos - 1; i++) {
        // Puntos originales locales del modelo
        float x1 = m->puntos[i].x;
        float y1 = m->puntos[i].y;
        float x2 = m->puntos[i + 1].x;
        float y2 = m->puntos[i + 1].y;

        // Si hay una matriz de transformación válida (posición de la entidad en el mundo), la aplicamos
        if (transformacion != NULL) {
            matriz_aplicar(transformacion, &x1, &y1);
            matriz_aplicar(transformacion, &x2, &y2);
        }

        // Dibujamos el segmento de recta transformado en la pantalla usando SDL2
        SDL_RenderDrawLine(renderer, (int)x1, (int)y1, (int)x2, (int)y2);
    }
}