
#include "misil.h"
#include <stdlib.h>
#include <math.h>

struct misil {
    float x;
    float y;
    float angulo;
    float velocidad;
    float tiempo_vida;
    int danio;
};

misil_t *misil_crear(const misil_config_t *config) {
    if (config == NULL) return NULL;

    misil_t *m = malloc(sizeof(misil_t));
    if (m == NULL) return NULL;

    m->x = config->x;
    m->y = config->y;
    m->angulo = config->angulo;
    m->velocidad = config->velocidad;
    m->tiempo_vida = config->tiempo_vida;
    m->danio = config->danio;

    return m;
}

void misil_destruir(misil_t *misil) {
    if (misil == NULL) return;
    free(misil);
}

bool misil_actualizar(misil_t *misil, float dt) {
    if (misil == NULL) return true;

    // Descontamos el tiempo de vida
    misil->tiempo_vida -= dt;
    if (misil->tiempo_vida <= 0.0f) {
        return true; // El misil expiró y debe ser removido
    }

    // Movimiento rectilíneo uniforme basado en el ángulo de disparo
    misil->x += misil->velocidad * cosf(misil->angulo) * dt;
    misil->y += misil->velocidad * sinf(misil->angulo) * dt;

    return false; // Sigue activo
}

void misil_obtener_posicion(const misil_t *misil, float *x, float *y) {
    if (misil == NULL) return;
    if (x) *x = misil->x;
    if (y) *y = misil->y;
}

float misil_obtener_angulo(const misil_t *misil) {
    return misil ? misil->angulo : 0.0f;
}

int misil_obtener_danio(const misil_t *misil) {
    return misil ? misil->danio : 0;
}

matriz_t *misil_obtener_matriz_transformacion(const misil_t *misil) {
    if (misil == NULL) return NULL;

    // Creamos las matrices dinámicas usando tu TDA matriz
    matriz_t *m_rot = matriz_crear_rotacion(misil->angulo);
    matriz_t *m_tras = matriz_crear_traslacion(misil->x, misil->y);

    if (!m_rot || !m_tras) {
        matriz_destruir(m_rot);
        matriz_destruir(m_tras);
        return NULL;
    }

    // transformacion = Traslación * Rotación
    matriz_t *m_final = matriz_multiplicar(m_tras, m_rot);

    // Liberamos la memoria de las matrices auxiliares intermedias
    matriz_destruir(m_rot);
    matriz_destruir(m_tras);

    return m_final;
}