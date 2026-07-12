#ifndef ANIMACIONES_H
#define ANIMACIONES_H

#include <stdbool.h>
#include <stddef.h>
#include <SDL2/SDL.h>
#include "lista.h"

#define CRISTAL_LINEAS 27
#define EXPLOSION_PIEZAS 6

typedef enum {
    ANIM_NINGUNA,
    ANIM_CRISTAL,   // vidrio roto del jugador: 27 líneas que aparecen en secuencia
    ANIM_EXPLOSION  // tiro oblicuo de las 6 piezas del enemigo destruido
} anim_tipo_t;

// TDA Animación: sólo lleva la lógica (tiempos, posiciones, física del
// tiro oblicuo); el dibujo queda a cargo del módulo de visualización.
typedef struct animacion animacion_t;

animacion_t *animacion_crear(void);
void animacion_destruir(animacion_t *a);

bool animacion_activa(const animacion_t *a);
anim_tipo_t animacion_tipo(const animacion_t *a);

// Arranca la animación de vidrio roto (genera las 27 rajaduras al azar).
void animacion_iniciar_cristal(animacion_t *a);

// Arranca la explosión del enemigo centrada en (x, y) del mundo:
// 6 piezas eyectadas cada 60°, con Vx = 5 m/s, Vz = 10 m/s y g = 9.81.
void animacion_iniciar_explosion(animacion_t *a, float x, float y);

void animacion_actualizar(animacion_t *a, float dt);

// Cristal: cantidad de líneas ya visibles y sus extremos en coordenadas
// normalizadas de pantalla ([-1, 1] en ambos ejes).
// Pre de animacion_cristal_linea: i < animacion_cristal_visibles(a).
size_t animacion_cristal_visibles(const animacion_t *a);
void animacion_cristal_linea(const animacion_t *a, size_t i,
                             float *x0, float *y0, float *x1, float *y1);

// Explosión: nombre del modelo de la pieza i y su posición/rotación en el
// mundo. animacion_pieza_posicion devuelve false si la animación no está
// activa o el índice es inválido.
// Pre: i < EXPLOSION_PIEZAS.
const char *animacion_pieza_modelo(const animacion_t *a, size_t i);
bool animacion_pieza_posicion(const animacion_t *a, size_t i,
                              float *x, float *y, float *z, float *rot);

// ============================================================================
// NUEVAS FUNCIONES: RENDERIZADO MATRICIAL DIRECTO 2D (HUD / TEXTOS / CRISTAL)
// ============================================================================
void dibujar_linea_2d(matriz_t* m, size_t coord1, size_t coord2, const unsigned char color[3], SDL_Renderer* renderer);
bool imprimir_caracter_2d(char c, float escala, float xy[2], unsigned char color[3], lista_t* modelos, SDL_Renderer* renderer);
bool imprimir_cadena_2d(const char* s, float escala, float xy[2], float incx, unsigned char color[3], lista_t* modelos, SDL_Renderer* renderer);
bool renderizar_cristal_2d(animacion_t *a, float escala, lista_t *modelos, SDL_Renderer *renderer);

#endif // ANIMACIONES_H