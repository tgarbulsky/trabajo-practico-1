#ifndef MUNDO_H
#define MUNDO_H

#include <stdbool.h>
#include <SDL2/SDL.h>

// Estructura opaca que contiene todo el estado del juego
typedef struct mundo mundo_t;

// Crea el mundo e inicializa las listas dinámicas, el tanque principal y carga los modelos STL
mundo_t *mundo_crear(void);

// Libera absolutamente toda la memoria del mundo (listas, tanques, misiles y modelos)
void mundo_destruir(mundo_t *mundo);

// Corre un paso de la simulación física e IA usando el delta de tiempo (dt)
// También procesa las colisiones exactas siguiendo la lógica de Nico
void mundo_actualizar(mundo_t *mundo, float dt);

// Renderiza todas las entidades del juego aplicando sus matrices de transformación
void mundo_dibujar(mundo_t *mundo, SDL_Renderer *renderer);

// Permite disparar un misil desde el tanque del jugador
void mundo_jugador_disparar(mundo_t *mundo);

#endif // MUNDO_H