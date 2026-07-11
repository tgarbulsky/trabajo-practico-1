
#include "misil.h"
#include <stdlib.h>
#include <math.h>

struct misil {
    float x;
    float y;
    float phi;
    float tiempo_vida; // Arranca en 2.0 segundos y va bajando
};

misil_t *misil_crear(float x, float y, float phi) {
    misil_t *m = malloc(sizeof(misil_t));
    if (m == NULL) {
        return NULL;
    }
    m->x = x;
    m->y = y;
    m->phi = phi;
    m->tiempo_vida = 2.0f; // El enunciado dice que dura un máximo de 2 segundos
    return m;
}

void misil_destruir(misil_t *m) {
    free(m);
}

// Getters para mantener el struct opaco
float misil_x(const misil_t *m)   { return m->x; }
float misil_y(const misil_t *m)   { return m->y; }
float misil_phi(const misil_t *m) { return m->phi; }

bool misil_actualizar(misil_t *m, float dt) {
    // El enunciado dice: avanza a una velocidad de 24 m/s
    m->x += 24.0f * cosf(m->phi) * dt;
    m->y += 24.0f * sinf(m->phi) * dt;
    
    // Le restamos el tiempo que pasó en este cuadro
    m->tiempo_vida -= dt;
    
    // Si se quedó sin tiempo, devolvemos false para avisar que debe ser destruido
    if (m->tiempo_vida <= 0) {
        return false;
    }
    
    return true; // Sigue vivo
}
