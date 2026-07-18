#ifndef MUNDO_H
#define MUNDO_H

#include "matriz.h"
#include "modelo.h"
#include "pila.h"
#include "lista.h"
#include <stdbool.h>
#include <math.h>
#include "tanque.h"

//--- Constantes del Juego --- //
#define VENTANA_ALTO 768
#define VENTANA_ANCHO 1024
#define JUEGO_FPS 24
#define NRO_OBSTACULOS 50

//--- Constantes Físicas --- //
#define V_TANQUE 7          // m/s
#define WZ_TANQUE 0.36      // rad/s
#define V_MISIL 24          // m/s
#define HITBOX_MISIL 3      // Radio de colisión misil
#define HITBOX_COLISION 5   // Radio de colisión tanque
#define VIDAS 4
#define SCORE_INC 1000
#define T_MUERTE 27

// --- Enumerativos --- //
enum bloques { TANQUE, TORRETA, RADAR, MISIL, CUBO1, CUBO2, CUBO3, 
               PIRAMIDE1, PIRAMIDE2, PIRAMIDE3, HORIZONTE, MONTANA, LUNA, RESTO1, RESTO2 };
typedef enum bloques bloque_t;

// --- Estructuras --- //
typedef struct cuerpo {
    bloque_t bloque;
    float pos[3];
    float angz;
} cuerpo_t;

//---randomizadores---//
float random_01();
float random_float(float a, float b);
int random_int(int a, int b);

// --- Primitivas de Física y Colisión --- //
float norma_r3(float v1[3], float v2[3]);
bool colisiones(lista_t* obstaculos, float otros_colisionables[3], float pos[3], float hitbox);

// --- Primitivas de Creación --- //
cuerpo_t* crear_obstaculo();

// --- Lógica y Movimiento --- //
void tanque_mover(tanque_t* tanque, enum tras tras, lista_t* obstaculos, tanque_t* otro_tanque);
bool misil_mover(cuerpo_t* misil, lista_t* obstaculos, tanque_t* otro_tanque, bool* kill);

#endif
