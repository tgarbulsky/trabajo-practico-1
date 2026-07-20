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

// Constantes globales de fisica, posiciones relativas y tablas de datos
extern const float g;
extern const float posicion_relativa_torreta[3];
extern const float posicion_relativa_radar[3];

extern const char* etiquetas[];
extern const unsigned char colores[][3];
extern const char* enemigo_relativa_posicion[];

// Apila y multiplica una matriz de transformacion en la pila
bool apilar_transformacion(pila_t* transformacion, matriz_t* matriz);

// Elimina y destruye la matriz superior de la pila de transformaciones
void desapilar_transformacion(pila_t* transformacion);

// Aplica una traslacion y una rotacion en Z juntas sobre la pila
bool apilar_rototraslacion(pila_t* transformacion, const float v[3], const float angz);

// Revierte una operacion de rototraslacion desapilando dos matrices
void desapilar_rototraslacion(pila_t* transformacion);

// Aplica la transformacion completa de camara y movimiento para un fotograma
bool apilar_cuadro_transformacion(int t_mov, int t_rot, tanque_t* tanque, pila_t* transformacion);

// Revierte las transformaciones aplicadas para el cuadro actual
void desapilar_cuadro_transformacion(pila_t* transformacion);

// Dibuja una linea proyectada en 3D siempre que esté delante de la camara
void dibujar_linea_3d(matriz_t* m, size_t coord1, size_t coord2, const unsigned char color[3], SDL_Renderer* renderer);

// Aplica las transformaciones a un bloque geometrico y lo proyecta en pantalla
bool bloque_imprimir(bloque_t bloque, modelo_t* modelo, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer);

// Dibuja un cuerpo individual del juego usando su posicion y rotacion
bool cuerpo_imprimir(cuerpo_t* cuerpo, lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer);

// Recorre e imprime la lista completa de obstaculos en el escenario
bool imprimir_obstaculos(lista_t* obstaculos, lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer);

// Dibuja el tanque junto con sus componentes articulados (torreta y radar)
bool tanque_imprimir(tanque_t* tanque, lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer);

// Dibuja los elementos del entorno de fondo como el horizonte, montañas y la luna
bool mundo_imprimir(lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer);

// Reproduce el efecto de destruccion dividiendo la entidad en partes que vuelan
bool animacion_destruccion(float posicion[3], int t_animacion, lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer);

// Crea la matriz de proyeccion para escalar y centrar la escena a la pantalla
matriz_t* matriz_crear_pantalla(unsigned int altura, unsigned int ancho);

// Busca un modelo 3D en la lista basandosi en su nombre de etiqueta
modelo_t* buscar_modelo(const char* etiqueta, lista_t* modelos);

// Busca el modelo 3D correspondiente a un tipo de bloque especifico
modelo_t* buscar_bloque(bloque_t bloque, lista_t* modelos);

#endif
