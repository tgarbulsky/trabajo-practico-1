#ifndef ANIMACIONES_H
#define ANIMACIONES_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "matriz.h"
#include "modelo.h"
#include "pila.h"
#include "lista.h"
#include "mundo.h"

// --- Gestión de la Pila de Transformaciones --- //

/**
 * @brief Si la pila está vacía, asigna la matriz en el tope. 
 * Si no, asigna como nuevo tope el producto del tope anterior con la matriz.
 * @return true si tuvo éxito, false en caso de falla de memoria.
 */
bool apilar_transformacion(pila_t* transformacion, matriz_t* matriz);

/**
 * @brief Desapila el último elemento de la pila de transformaciones y libera su memoria.
 */
void desapilar_transformacion(pila_t* transformacion);

/**
 * @brief Apila una traslación de vector v seguida de una rotación en el eje Z de ángulo angz.
 */
bool apilar_rototraslacion(pila_t* transformacion, const float v[2], const float angz);

/**
 * @brief Desapila una rototraslación (elimina dos matrices de la pila).
 */
void desapilar_rototraslacion(pila_t* transformacion);

/**
 * @brief Apila las matrices iniciales para configurar la cámara desde la perspectiva del tanque.
 * Incluye proyección, rotación de cámara y posición del jugador [4].
 */
bool apilar_cuadro_transformacion(int t_mov, int t_rot, tanque_t* tanque, pila_t* transformacion);

/**
 * @brief Desapila la transformación de cuadro completa (cuatro matrices).
 */
void desapilar_cuadro_transformacion(pila_t* transformacion);

// --- Motor de Renderizado 3D --- //

/**
 * @brief Dibuja una línea en coordenadas de pantalla.
 */
void dibujar_linea(matriz_t* m, size_t coord1, size_t coord2, const unsigned char color[2], SDL_Renderer* renderer);

/**
 * @brief Dibuja una línea proyectada descartando puntos con Z < 1 (detrás de la cámara) [5].
 */
void dibujar_linea_3d(matriz_t* m, size_t coord1, size_t coord2, const unsigned char color[2], SDL_Renderer* renderer);

/**
 * @brief Aplica la transformación actual e imprime un bloque específico del modelo.
 */
bool bloque_imprimir(bloque_t bloque, modelo_t* modelo, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer);

/**
 * @brief Imprime un objeto físico con su posición y orientación en el espacio 3D [6].
 */
bool cuerpo_imprimir(cuerpo_t* cuerpo, lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer);

/**
 * @brief Recorre la lista de obstáculos e imprime cada uno en pantalla [7].
 */
bool imprimir_obstaculos(lista_t* obstaculos, lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer);

/**
 * @brief Imprime el tanque completo (Cuerpo, Torreta y Radar) aplicando transformaciones relativas [7].
 */
bool tanque_imprimir(tanque_t* tanque, lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer);

/**
 * @brief Imprime los elementos estáticos del fondo (Horizonte, Montañas y Luna) [8].
 */
bool mundo_imprimir(lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer);

// --- Animaciones de Destrucción --- //

/**
 * @brief Calcula y dibuja la animación de tiro oblicuo de las piezas del tanque enemigo [9].
 */
bool animacion_destruccion(float pos[2], int t_animacion, lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer);

// --- Funciones Auxiliares --- //

/**
 * @brief Crea la matriz necesaria para transformar coordenadas de mundo a píxeles de pantalla [10].
 */
matriz_t* matriz_crear_pantalla(unsigned int altura, unsigned int ancho);

/**
 * @brief Busca un modelo por su etiqueta dentro de la lista de modelos.
 */
modelo_t* buscar_modelo(const char* etiqueta, lista_t* modelos);

/**
 * @brief Busca el modelo correspondiente a un tipo de bloque específico [11].
 */
modelo_t* buscar_bloque(bloque_t bloque, lista_t* modelos);

#endif