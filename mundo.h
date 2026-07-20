#ifndef MUNDO_H
#define MUNDO_H

#include "matriz.h"
#include "modelo.h"
#include "pila.h"
#include "lista.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>

// Dimensiones de la ventana y rendimiento
#define VENTANA_ALTO 768
#define VENTANA_ANCHO 1024
#define JUEGO_FPS 24

// Constante matematica Pi si no esta definida por el sistema
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// Parametros fisicos y de velocidad
#define V_TANQUE 7
#define WZ_TANQUE 0.36
#define WZ_TORRETA_ON 0.12
#define WZ_TORRETA_OFF 0.24
#define WZ_RADAR 2*M_PI
#define V_MISIL 24
#define V0X 5
#define V0Z 10

// Temporizadores y tiempo de juego
#define T_ANIM 2.3
#define T_MUERTE 27
#define T_ACCION 0.5
#define COOLDOWN 2

// Hitboxes, estado y configuracion del mapa
#define HITBOX_MISIL 3
#define HITBOX_COLISION 5
#define VIDAS 4
#define SCORE_INC 1000
#define NRO_OBSTACULOS 50

// Enumeracion de los tipos de bloques y modelos disponibles
enum bloques {TANQUE, TORRETA, RADAR, MISIL, CUBO1, CUBO2, CUBO3, PIRAMIDE1, PIRAMIDE2, PIRAMIDE3, HORIZONTE, MONTANA, LUNA, RESTO1, RESTO2};
typedef enum bloques bloque_t;

// Enumeraciones para los estados de movimiento y control
enum tras {TRAS_NONE, TRAS_FWD, TRAS_BWD};
enum rot {ROT_NONE, ROT_CW, ROT_CCW};
enum acciones_ia {IA_NONE, IA_FWD, IA_BWD, IA_CW, IA_CCW};
enum turr {TURR_NONE, TURR_ON_CW, TURR_ON_CCW, TURR_OFF};
enum enemy_to {FRONT, LEFT, RIGHT, BACK};

// Estructura que representa un objeto fisico en el mundo 3D
typedef struct cuerpo{
    bloque_t bloque;
    float posicion[3];
    float angz;
}cuerpo_t;

// Estructura que almacena la posicion y las rotaciones del tanque del jugador
typedef struct tanque{
    float posicion[3];
    float angz;
    float ang_torreta;
    float ang_radar;
}tanque_t;

// Genera un numero flotante aleatorio entre 0.0 y 1.0
float random_entre01();

// Genera un numero flotante aleatorio en el rango especificado [a, b]
float random_float(float a, float b);

// Genera un numero entero aleatorio en el rango especificado [a, b]
int random_int(int a, int b);

// Calcula la distancia euclidiana entre dos puntos en el espacio tridimensional
float norma_r3(float v1[3], float v2[3]);

// Evalua si existe una colision entre un punto dado y los elementos del escenario
bool colisiones(lista_t* obstaculos, float otros_colisionables[3], float posicion[3], float hitbox);

#endif
