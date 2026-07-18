#ifndef ANIMACIONES_H
#define ANIMACIONES_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "matriz.h"
#include "modelo.h"
#include "pila.h"
#include "lista.h"
#include "mundo.h"
#include "tanque.h"

#define T_ANIM 2

extern const char* etiquetas[];
extern const unsigned char colores[][3];
extern const char* enemy_rel_pos[];
extern const VOX; 
extern const V0Z;

// --- Gestion de la Pila de Transformaciones --- //

// Si la pila está vacía, asigna la matriz en el tope. 
// Si no, asigna como nuevo tope el producto del tope anterior con la matriz.
//return true si tuvo exito, false en caso de falla de memoria.

bool apilar_transformacion(pila_t* transformacion, matriz_t* matriz);

//Desapila el último elemento de la pila de transformaciones y libera su memoria.
void desapilar_transformacion(pila_t* transformacion);


//Apila una traslación de vector v seguida de una rotación en el eje Z de ángulo angz.
bool apilar_rototraslacion(pila_t* transformacion, const float v[3], const float angz);


//Desapila una rototraslación (elimina dos matrices de la pila).
void desapilar_rototraslacion(pila_t* transformacion);

//Apila las matrices iniciales para configurar la cámara desde la perspectiva del tanque.
//Incluye proyección, rotación de cámara y posición del jugador
bool apilar_cuadro_transformacion(int t_mov, int t_rot, tanque_t* tanque, pila_t* transformacion);


//Desapila la transformación de cuadro completa (viste que eran cuatro matrices).
void desapilar_cuadro_transformacion(pila_t* transformacion);

// --- Motor de Renderizado 3D --- //

//Dibuja una línea en coordenadas de pantalla.
void dibujar_linea(matriz_t* m, size_t coord1, size_t coord2, const unsigned char color[3], SDL_Renderer* renderer);


// Dibuja una línea proyectada descartando puntos con Z < 1
void dibujar_linea_3d(matriz_t* m, size_t coord1, size_t coord2, const unsigned char color[3], SDL_Renderer* renderer);

//Aplica la transformación actual e imprime un bloque específico del modelo.

bool bloque_imprimir(bloque_t bloque, modelo_t* modelo, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer);

//Imprime un objeto físico con su posición y orientación en el espacio 3D
bool cuerpo_imprimir(cuerpo_t* cuerpo, lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer);


//Recorre la lista de obstáculos e imprime cada uno en pantalla
bool imprimir_obstaculos(lista_t* obstaculos, lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer);


//Imprime el tanque completo aplicando transformaciones relativas.
bool tanque_imprimir(tanque_t* tanque, lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer);


//Imprime los elementos estaticos del fondo (Horizonte, Montañas y Luna)
bool mundo_imprimir(lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer);

// --- Animaciones de Destrucción --- //

//Calcula y dibuja la animacion de tiro oblicuo de las piezas del tanque enemigo
bool animacion_destruccion(float pos[3], int t_animacion, lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer);

// --- Funciones auxiliares --- //

//Crea la matriz necesaria para transformar coordenadas de mundo a píxeles de pantalla
matriz_t* matriz_crear_pantalla(unsigned int altura, unsigned int ancho);

//Busca un modelo por su etiqueta dentro de la lista de modelos.
modelo_t* buscar_modelo(const char* etiqueta, lista_t* modelos);

//Busca el modelo correspondiente a un tipo de bloque específico
modelo_t* buscar_bloque(bloque_t bloque, lista_t* modelos);

#endif
