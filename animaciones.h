#ifndef ANIMACIONES_H
#define ANIMACIONES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>

#include "matriz.h"
#include "modelo.h"
#include "mundo.h"
#include "pila.h"
#include "lista.h"

/*Declaración de constantes y tablas de búsqueda*/
extern const float g;
extern const float pos_rel_torreta[3];
extern const float pos_rel_radar[3];

extern const char* etiquetas[];
extern const unsigned char colores[][3];
extern const char* enemy_rel_pos[];

/*Métodos de transformación*/
bool apilar_transformacion(pila_t* transformacion, matriz_t* matriz);
void desapilar_transformacion(pila_t* transformacion);
bool apilar_rototraslacion(pila_t* transformacion, const float v[3], const float angz);
void desapilar_rototraslacion(pila_t* transformacion);
bool apilar_cuadro_transformacion(int t_mov, int t_rot, tanque_t* tanque, pila_t* transformacion);
void desapilar_cuadro_transformacion(pila_t* transformacion);

/*Funciones de dibujo 3D*/
void dibujar_linea_3d(matriz_t* m, size_t coord1, size_t coord2, const unsigned char color[3], SDL_Renderer* renderer);

/*Métodos de impresión 3D*/
bool bloque_imprimir(bloque_t bloque, modelo_t* modelo, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer);
bool cuerpo_imprimir(cuerpo_t* cuerpo, lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer);
bool imprimir_obstaculos(lista_t* obstaculos, lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer);
bool tanque_imprimir(tanque_t* tanque, lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer);
bool mundo_imprimir(lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer);

/*Animaciones*/
bool animacion_destruccion(float pos[3], int t_animacion, lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer);

/*Auxiliares*/
matriz_t* matriz_crear_pantalla(unsigned int altura, unsigned int ancho);
modelo_t* buscar_modelo(const char* etiqueta, lista_t* modelos);
modelo_t* buscar_bloque(bloque_t bloque, lista_t* modelos);

#endif
