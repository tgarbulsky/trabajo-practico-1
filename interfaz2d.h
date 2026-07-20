#ifndef INTERFAZ2D_H
#define INTERFAZ2D_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <SDL2/SDL.h>

#include "matriz.h"
#include "modelo.h"
#include "mundo.h"
#include "lista.h"

// Arreglo externo con los nombres o etiquetas de posicion relativa de los enemigos
extern const char* enemy_rel_pos[];

// Dibuja una linea 2D en pantalla conectando dos coordenadas de la matriz usando SDL
void dibujar_linea(matriz_t* m, size_t coord1, size_t coord2, const unsigned char color[3], SDL_Renderer* renderer);

// Dibuja un solo caracter en pantalla aplicando escala, posicion xy y color especificados
bool imprimir_caracter(char c, float escala, float xy[2], unsigned char color[3], lista_t* modelos, SDL_Renderer* renderer);

// Dibuja una cadena de texto en pantalla avanzando horizontalmente segun el incremento incx
bool imprimir_cadena(const char* s, float escala, float xy[2], float incx, unsigned char color[3], lista_t* modelos, SDL_Renderer* renderer);

// Renderiza la interfaz de usuario completa (vidas, puntaje, indicador de enemigo y mira)
bool imprimir_hud(char vidas, unsigned long score, enum enemy_to enemy_pos, char scope, lista_t* modelos, SDL_Renderer* renderer);

// Reproduce la animacion 2D en pantalla cuando el jugador pierde una vida
bool animacion_muerte(int t_muerte, float escala, char vidas, lista_t* modelos, SDL_Renderer* renderer);

#endif
