#ifndef MUNDO_H
#define MUNDO_H

#include <stdbool.h>
#include <stddef.h>
#include "lista.h"
#include "tanque.h"
#include "obstaculo.h"

// TDA Mundo: concentra toda la lógica del juego (tanques, obstáculos,
// misiles, colisiones, puntaje y condición de fin) separada del renderizado.
typedef struct mundo mundo_t;

// Crea el mundo: jugador en el origen con 4 vidas, 50 obstáculos al azar
// en [-150, 150] y el primer enemigo a 50 metros del jugador.
// Recibe la lista de modelos cargada para asignar formas a los obstáculos.
// Devuelve NULL si falla la memoria.
mundo_t *mundo_crear(lista_t *lista_modelos);

// Libera toda la memoria del mundo (tanques, misiles y obstáculos).
void mundo_destruir(mundo_t *m);

// Avanza la simulación un paso de dt segundos: movimiento de tanques y
// misiles, IA y puntería del enemigo, colisiones y puntaje.
void mundo_actualizar(mundo_t *m, float dt);

// Getters para que el módulo de visualización dibuje las entidades.
tanque_t *mundo_jugador(const mundo_t *m);
tanque_t *mundo_enemigo(const mundo_t *m);
size_t mundo_num_obstaculos(const mundo_t *m);
obstaculo_t *mundo_obstaculo(const mundo_t *m, size_t i);

// Estado para el HUD.
int mundo_puntaje(const mundo_t *m);
int mundo_vidas(const mundo_t *m);
bool mundo_terminado(const mundo_t *m);

// Eventos de un solo uso (se limpian al consultarlos): permiten al
// llamador disparar las animaciones sin acoplar el mundo al renderizado.
bool mundo_evento_jugador_impactado(mundo_t *m);
bool mundo_evento_enemigo_destruido(mundo_t *m, float *x, float *y);

#endif // MUNDO_H
