#include "obstaculo.h"
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// Funciones internas de aleatoriedad para que este archivo sea 100% independiente
static float rand_float(float min, float max) {
    return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

static int rand_int(int min, int max) {
    return min + rand() % (max - min + 1);
}

obstaculo_t* obstaculo_crear(void) {
    obstaculo_t* obs = malloc(sizeof(obstaculo_t));
    if (obs == NULL) return NULL;

    obs->tipo = (tipo_obstaculo_t)rand_int(CUBO1, PIRAMIDE3);
    obs->pos[0] = rand_float(-150.0f, 150.0f);
    obs->pos[1] = rand_float(-150.0f, 150.0f);
    obs->pos[2] = 0.0f;
    obs->angz = rand_float(-M_PI, M_PI);

    return obs;
}

void obstaculo_destruir(obstaculo_t* obs) {
    free(obs);
}