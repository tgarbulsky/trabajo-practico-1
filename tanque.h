#ifndef TANQUE_H
#define TANQUE_H

#include <stdbool.h>
#include <stddef.h>
#include "obstaculo.h"

typedef struct tanque tanque_t;

typedef enum {
    MOV_NINGUNO,
    MOV_ADELANTE,
    MOV_ATRAS,
    MOV_GIRAR_IZQ,
    MOV_GIRAR_DER
} movimiento_e;

// Constructor y Destructor
tanque_t *tanque_crear(float x, float y, float phi, int vidas);
void      tanque_destruir(tanque_t *t);

// Crea un tanque enemigo (con IA de movimiento aleatorio) en (x, y).
// Devuelve NULL si la posición queda a menos de 5 metros de algún
// obstáculo (o si falla la memoria); el llamador debe reintentar con
// otra posición. Guarda la referencia a los obstáculos para evitar
// atravesarlos al moverse.
tanque_t *crear_tanque_enemigo(float x, float y, float phi, int vidas,
                               obstaculo_t *obstaculos[], size_t num_obstaculos);

// Asigna los obstáculos que el tanque no puede atravesar al moverse
// (permite dárselos al jugador, cuyo constructor no los recibe).
void tanque_asignar_obstaculos(tanque_t *t, obstaculo_t *obstaculos[],
                               size_t num_obstaculos);

// Getters públicos (Encapsulamiento para el Juego y el Render)
float tanque_x(const tanque_t *t);
float tanque_y(const tanque_t *t);
float tanque_phi(const tanque_t *t);
int   tanque_vidas(const tanque_t *t);
float tanque_torreta(const tanque_t *t);
bool  tanque_puede_disparar(const tanque_t *t);

// Acciones y Modificadores de Estado
void tanque_girar(tanque_t *t, float delta_phi);
void tanque_mover(tanque_t *t, float delta);
void tanque_girar_torreta(tanque_t *t, float delta);
void tanque_recibir_impacto(tanque_t *t);

// Lógica de disparo: Intenta disparar. Si puede, crea el misil,
// activa el cooldown y devuelve true.
bool tanque_disparar(tanque_t *t);

// Misil del tanque (cada tanque tiene a lo sumo un misil en vuelo).
// Pre de los getters de posición: el misil está activo.
bool  tanque_misil_activo(const tanque_t *t);
float tanque_misil_x(const tanque_t *t);
float tanque_misil_y(const tanque_t *t);
float tanque_misil_phi(const tanque_t *t);

// Destruye el misil en vuelo (por ejemplo, tras impactar). Acepta que no haya misil.
void tanque_misil_terminar(tanque_t *t);

// Lógica de Movimientos Sostenidos y Ciclo de Vida (Actualizador)
void         tanque_iniciar_movimiento(tanque_t *t, movimiento_e mov);
movimiento_e tanque_movimiento(const tanque_t *t);
void         tanque_actualizar(tanque_t *t, float dt);

#endif