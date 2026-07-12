#ifndef ANIMACIONES_H
#define ANIMACIONES_H

#include <stdbool.h>
#include <stddef.h>
#include <SDL2/SDL.h>
#include "lista.h"
#include "matriz.h" // Inclusión necesaria para compatibilidad de tipos

#define CRISTAL_LINEAS 27
#define EXPLOSION_PIEZAS 6

typedef enum {
    ANIM_NINGUNA,
    ANIM_CRISTAL,   // Vidrio roto del jugador: 27 líneas en secuencia
    ANIM_EXPLOSION  // Tiro oblicuo de las 6 piezas del enemigo destruido
} anim_tipo_t;

// TDA Animación: Maneja la física y los tiempos de los efectos visuales.
typedef struct animacion animacion_t;

animacion_t *animacion_crear(void);
void animacion_destruir(animacion_t *a);

bool animacion_activa(const animacion_t *a);
anim_tipo_t animacion_tipo(const animacion_t *a);

// Arranca la animación de vidrio roto (genera las 27 rajaduras al azar).
void animacion_iniciar_cristal(animacion_t *a);

// Arranca la explosión del enemigo centrada en (x, y) del mundo
void animacion_iniciar_explosion(animacion_t *a, float x, float y);

void animacion_actualizar(animacion_t *a, float dt);

// Cristal: Cantidad de líneas ya visibles y sus extremos en coordenadas normalizadas [-1, 1].
size_t animacion_cristal_visibles(const animacion_t *a);
void animacion_cristal_linea(const animacion_t *a, size_t i,
                             float *x0, float *y0, float *x1, float *y1);

// Explosión: Nombre del modelo de la pieza i y su posición/rotación en el mundo.
const char *animacion_pieza_modelo(const animacion_t *a, size_t i);
bool animacion_pieza_posicion(const animacion_t *a, size_t i,
                              float *x, float *y, float *z, float *rot);

// ============================================================================
// FUNCIONES DE RENDERIZADO GEOMÉTRICO DIRECTO 2D (HUD Y EFECTOS)
// ============================================================================
bool imprimir_caracter_2d(char c, float escala, float xy[2], unsigned char color[3], lista_t* modelos, SDL_Renderer* renderer);
bool imprimir_cadena_2d(const char* s, float escala, float xy[2], float incx, unsigned char color[3], lista_t* modelos, SDL_Renderer* renderer);
bool renderizar_cristal_2d(animacion_t *a, float escala, lista_t *modelos, SDL_Renderer *renderer);

#endif // ANIMACIONES_H