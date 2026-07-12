#ifndef ANIMACIONES_H
#define ANIMACIONES_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stddef.h>
#include "modelo.h"

// --- TDA Animación de Cristales Rotos (Efecto 2D sobre el HUD) ---
typedef struct animacion_cristales animacion_cristales_t;

// Crea la animación de cristales rotos en pantalla con una cantidad dada de fragmentos.
animacion_cristales_t *animacion_cristales_crear(size_t cant);

// Actualiza la posición y rotación de los cristales en base al tiempo delta (dt).
// Devuelve false cuando la animación termina (cumple su tiempo de vida).
bool animacion_cristales_actualizar(animacion_cristales_t *a, float dt);

// Dibuja los cristales en coordenadas de pantalla alrededor de un centro dado (generalmente el centro de la pantalla).
void animacion_cristales_dibujar(const animacion_cristales_t *a, SDL_Renderer *renderer, int centro_x, int centro_y);

// Libera la memoria del TDA de cristales.
void animacion_cristales_destruir(animacion_cristales_t *a);


// --- TDA Animación de Destrucción del Enemigo (Efecto 3D con Rotación Intrínseca) ---
typedef struct animacion_enemigo animacion_enemigo_t;

// Crea la animación de explosión 3D desarmando las aristas del modelo del enemigo en la posición (x, y) del mundo.
animacion_enemigo_t *animacion_enemigo_crear(const modelo_t *mod, float x, float y);

// Actualiza la expansión, gravedad y rotación sobre sí mismas de las piezas del tanque.
// Devuelve false cuando la animación finaliza.
bool animacion_enemigo_actualizar(animacion_enemigo_t *a, float dt);

// Proyecta en perspectiva 3D y dibuja cada fragmento rotando independientemente.
void animacion_enemigo_dibujar(const animacion_enemigo_t *a, SDL_Renderer *renderer, const matriz_t *matriz_vista, int width, int height);

// Libera la memoria del TDA del enemigo destruido.
void animacion_enemigo_destruir(animacion_enemigo_t *a);

#endif // ANIMACIONES_H