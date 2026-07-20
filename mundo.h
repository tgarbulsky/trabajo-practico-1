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

#define VENTANA_ALTO 768
#define VENTANA_ANCHO 1024
#define JUEGO_FPS 24

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#define V_TANQUE 7
#define WZ_TANQUE 0.36
#define WZ_TORRETA_ON 0.12
#define WZ_TORRETA_OFF 0.24
#define WZ_RADAR 2*M_PI
#define V_MISIL 24
#define V0X 5
#define V0Z 10
#define T_ANIM 2.3
#define T_MUERTE 27
#define T_ACCION 0.5
#define COOLDOWN 2
#define HITBOX_MISIL 3
#define HITBOX_COLISION 5
#define VIDAS 4
#define SCORE_INC 1000
#define NRO_OBSTACULOS 50

enum bloques {TANQUE, TORRETA, RADAR, MISIL, CUBO1, CUBO2, CUBO3, PIRAMIDE1, PIRAMIDE2, PIRAMIDE3, HORIZONTE, MONTANA, LUNA, RESTO1, RESTO2};
typedef enum bloques bloque_t;

enum tras {TRAS_NONE, TRAS_FWD, TRAS_BWD};
enum rot {ROT_NONE, ROT_CW, ROT_CCW};
enum acciones_ia {IA_NONE, IA_FWD, IA_BWD, IA_CW, IA_CCW};
enum turr {TURR_NONE, TURR_ON_CW, TURR_ON_CCW, TURR_OFF};
enum enemy_to {FRONT, LEFT, RIGHT, BACK};

typedef struct cuerpo{
    bloque_t bloque;
    float pos[3];
    float angz;
}cuerpo_t;

typedef struct tanque{
    float pos[3];
    float angz;
    float ang_torreta;
    float ang_radar;
}tanque_t;

// primitivas de aleatoriedad
float random_01();
float random_float(float a, float b);
int random_int(int a, int b);

// primitivas Auxiliares y generales de colisiones
float norma_r3(float v1[3], float v2[3]);
bool colisiones(lista_t* obstaculos, float otros_colisionables[3], float pos[3], float hitbox);

#endif
