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

/*Declaración de constantes y tablas externas necesarias*/
extern const char* enemy_rel_pos[];

/*Funciones internas de dibujo plano*/
void dibujar_linea(matriz_t* m, size_t coord1, size_t coord2, const unsigned char color[3], SDL_Renderer* renderer);

/*Métodos de impresión 2D*/
bool imprimir_caracter(char c, float escala, float xy[2], unsigned char color[3], lista_t* modelos, SDL_Renderer* renderer);
bool imprimir_cadena(const char* s, float escala, float xy[2], float incx, unsigned char color[3], lista_t* modelos, SDL_Renderer* renderer);
bool imprimir_hud(char vidas, unsigned long score, enum enemy_to enemy_pos, char scope, lista_t* modelos, SDL_Renderer* renderer);

/*Animaciones 2D planos*/
bool animacion_muerte(int t_muerte, float escala, char vidas, lista_t* modelos, SDL_Renderer* renderer);

#endif
