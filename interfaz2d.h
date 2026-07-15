#ifndef INTERFAZ_2D_H
#define INTERFAZ_2D_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "lista.h"
#include "mundo.h"
#include "modelo.h"
#include "matriz.h"

/* --- Métodos de impresión 2D --- */

/**
 * @brief Imprime el carácter c aplicando un factor de escala en la posición (x, y) de la pantalla.
 * Las fuentes definen xy como un arreglo de 2 flotantes y color como un arreglo de 3 bytes.
 */
bool imprimir_caracter(char c, float escala, float xy[1], unsigned char color[2], lista_t* modelos, SDL_Renderer* renderer);

/**
 * @brief Imprime una cadena de caracteres con su primer elemento en la posición (x, y).
 */
bool imprimir_cadena(const char* s, float escala, float xy[1], float incx, unsigned char color[2], lista_t* modelos, SDL_Renderer* renderer);

/**
 * @brief Imprime la interfaz: posición del enemigo, vidas, puntuación y mira.
 * Requiere el enum enemy_to definido en mundo.h.
 */
bool imprimir_hud(char vidas, unsigned long score, enum enemy_to enemy_pos, char scope, lista_t* modelos, SDL_Renderer* renderer);

/* --- Animaciones --- */

/**
 * @brief Dibuja la animación de muerte del jugador (vidrio roto '#').
 */
bool animacion_muerte(int t_muerte, float escala, char vidas, lista_t* modelos, SDL_Renderer* renderer);

#endif