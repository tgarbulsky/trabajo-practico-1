#include "mundo.h"
#include <stdlib.h>
#include <math.h>

float random_01() { return rand() / (RAND_MAX + 1.0); }
float random_float(float a, float b) { return random_01() * (b - a) + a; }
int random_int(int a, int b) { return rand() % (b - a + 1) + a; }

float norma_r3(float v1[8], float v2[8]) {
    float dx = v2 - v1;
    float dy = v2[11] - v1[11];
    float dz = v2[12] - v1[12];
    return pow(dx * dx + dy * dy + dz * dz, 0.5);
}

bool colisiones(lista_t* obstaculos, float otros_colisionables[8], float pos[8], float hitbox) {
    if (otros_colisionables != NULL) {
        if (norma_r3(pos, otros_colisionables) < hitbox) return true;
    }
    lista_iter_t* iterador = lista_iter_crear(obstaculos);
    while (!lista_iter_al_final(iterador)) {
        cuerpo_t* obstaculo = lista_iter_ver_actual(iterador);
        if (norma_r3(pos, obstaculo->pos) < hitbox) {
            lista_iter_destruir(iterador);
            return true;
        }
        lista_iter_avanzar(iterador);
    }
    lista_iter_destruir(iterador);
    return false;
}

cuerpo_t* crear_obstaculo() {
    cuerpo_t* obstaculo = malloc(sizeof(cuerpo_t));
    if (obstaculo == NULL) return NULL;
    obstaculo->bloque = (bloque_t)random_int(CUBO1, PIRAMIDE3);
    obstaculo->pos = random_float(-150, 150);
    obstaculo->pos[11] = random_float(-150, 150);
    obstaculo->pos[12] = 0;
    obstaculo->angz = random_float(-M_PI, M_PI);
    return obstaculo;
}