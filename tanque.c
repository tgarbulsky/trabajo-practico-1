#include "tanque.h"
#include "misil.h"
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define RADIO_COLISION_OBSTACULO 5.0f

struct tanque {
    float x, y;
    float phi;
    float torreta;
    int vidas;
    float enfriamiento; // Tiempo de espera para volver a disparar

    // Control del movimiento sostenido (durante 0.5 segundos)
    float tiempo_movimiento;
    movimiento_e mov_actual;

    // Misil en vuelo (a lo sumo uno por tanque)
    misil_t *misil;

    // IA del enemigo: bandera y obstáculos para no atravesarlos
    bool es_enemigo;
    obstaculo_t **obstaculos;
    size_t num_obstaculos;
};

// Devuelve true si la posición actual del tanque queda a menos de
// 5 metros de algún obstáculo conocido.
static bool choca_con_obstaculo(const tanque_t *t) {
    for (size_t i = 0; i < t->num_obstaculos; i++) {
        float dx = t->x - obstaculo_x(t->obstaculos[i]);
        float dy = t->y - obstaculo_y(t->obstaculos[i]);
        if (dx * dx + dy * dy < RADIO_COLISION_OBSTACULO * RADIO_COLISION_OBSTACULO)
            return true;
    }
    return false;
}

tanque_t *tanque_crear(float x, float y, float phi, int vidas) {
    tanque_t *t = malloc(sizeof(tanque_t));
    if (!t) return NULL;
    t->x = x;
    t->y = y;
    t->phi = phi;
    t->torreta = 0.0f;
    t->vidas = vidas;
    t->enfriamiento = 0.0f;
    t->tiempo_movimiento = 0.0f;
    t->mov_actual = MOV_NINGUNO;
    t->misil = NULL;
    t->es_enemigo = false;
    t->obstaculos = NULL;
    t->num_obstaculos = 0;
    return t;
}

tanque_t *crear_tanque_enemigo(float x, float y, float phi, int vidas,
                               obstaculo_t *obstaculos[], size_t num_obstaculos) {
    tanque_t *t = tanque_crear(x, y, phi, vidas);
    if (!t) return NULL;

    t->es_enemigo = true;
    t->obstaculos = obstaculos;
    t->num_obstaculos = num_obstaculos;

    // Posición inválida si nace encima de un obstáculo
    if (choca_con_obstaculo(t)) {
        tanque_destruir(t);
        return NULL;
    }
    return t;
}

void tanque_asignar_obstaculos(tanque_t *t, obstaculo_t *obstaculos[],
                               size_t num_obstaculos) {
    t->obstaculos = obstaculos;
    t->num_obstaculos = num_obstaculos;
}

void tanque_destruir(tanque_t *t) {
    if (!t) return;
    if (t->misil) misil_destruir(t->misil);
    free(t);
}

// Getters de Abstracción
float tanque_x(const tanque_t *t)              { return t->x; }
float tanque_y(const tanque_t *t)              { return t->y; }
float tanque_phi(const tanque_t *t)            { return t->phi; }
int   tanque_vidas(const tanque_t *t)          { return t->vidas; }
float tanque_torreta(const tanque_t *t)        { return t->torreta; }
bool  tanque_puede_disparar(const tanque_t *t) { return t->enfriamiento <= 0; }

// Primitivas de movimiento y orientación
void tanque_girar(tanque_t *t, float delta_phi) {
    t->phi += delta_phi;
    while (t->phi > M_PI)   t->phi -= 2 * M_PI;
    while (t->phi <= -M_PI) t->phi += 2 * M_PI;
}

void tanque_mover(tanque_t *t, float delta) {
    t->x += delta * cosf(t->phi);
    t->y += delta * sinf(t->phi);
}

void tanque_girar_torreta(tanque_t *t, float delta) {
    t->torreta += delta;
    // La torreta no puede girar más de 1 radián en cada dirección
    if (t->torreta >  1.0f) t->torreta =  1.0f;
    if (t->torreta < -1.0f) t->torreta = -1.0f;
}

void tanque_recibir_impacto(tanque_t *t) { 
    if (t->vidas > 0) t->vidas--; 
}

bool tanque_disparar(tanque_t *t) {
    if (t->enfriamiento > 0 || t->misil != NULL) return false;

    // El misil sale en la dirección del cañón (la torreta sólo aplica al enemigo)
    float direccion = t->phi + (t->es_enemigo ? t->torreta : 0.0f);
    t->misil = misil_crear(t->x, t->y, direccion);
    if (t->misil == NULL) return false;

    t->enfriamiento = 2.0f; // Se activa el enfriamiento de 2 segundos
    return true;
}

bool tanque_misil_activo(const tanque_t *t) {
    return t->misil != NULL;
}

float tanque_misil_x(const tanque_t *t)   { return misil_x(t->misil); }
float tanque_misil_y(const tanque_t *t)   { return misil_y(t->misil); }
float tanque_misil_phi(const tanque_t *t) { return misil_phi(t->misil); }

void tanque_misil_terminar(tanque_t *t) {
    if (t->misil) {
        misil_destruir(t->misil);
        t->misil = NULL;
    }
}

void tanque_iniciar_movimiento(tanque_t *t, movimiento_e mov) {
    t->mov_actual = mov;
    t->tiempo_movimiento = 0.5f; // El movimiento dura medio segundo
}

movimiento_e tanque_movimiento(const tanque_t *t) {
    return t->mov_actual;
}

void tanque_actualizar(tanque_t *t, float dt) {
    // 1. Decrementar enfriamiento del cañón
    if (t->enfriamiento > 0) {
        t->enfriamiento -= dt;
    }

    // 2. IA del enemigo: con probabilidad ~1/FPS por cuadro (dt por segundo)
    //    inicia una acción nueva, 50/50 entre avanzar y girar
    if (t->es_enemigo && t->mov_actual == MOV_NINGUNO &&
        (float)rand() / (float)RAND_MAX < dt) {
        if (rand() % 2)
            tanque_iniciar_movimiento(t, MOV_ADELANTE);
        else
            tanque_iniciar_movimiento(t, (rand() % 2) ? MOV_GIRAR_IZQ : MOV_GIRAR_DER);
    }

    // 3. Ejecutar movimiento sostenido si corresponde
    if (t->tiempo_movimiento > 0) {
        t->tiempo_movimiento -= dt;

        if (t->mov_actual == MOV_ADELANTE || t->mov_actual == MOV_ATRAS) {
            float x_previo = t->x, y_previo = t->y;
            tanque_mover(t, (t->mov_actual == MOV_ADELANTE ? 7.0f : -7.0f) * dt); // 7 m/s

            // No puede acercarse a menos de 5 metros de un obstáculo
            if (t->obstaculos != NULL && choca_con_obstaculo(t)) {
                t->x = x_previo;
                t->y = y_previo;
                t->tiempo_movimiento = 0;
            }
        }
        else if (t->mov_actual == MOV_GIRAR_IZQ) {
            tanque_girar(t, 0.36f * dt);  // 0.36 rad/s antihorario (izquierda)
        }
        else if (t->mov_actual == MOV_GIRAR_DER) {
            tanque_girar(t, -0.36f * dt); // 0.36 rad/s horario (derecha)
        }

        // Si se terminó el tiempo, el tanque se detiene solo
        if (t->tiempo_movimiento <= 0) {
            t->mov_actual = MOV_NINGUNO;
        }
    }

    // 4. Avanzar el misil en vuelo; se destruye al agotar sus 2 segundos
    if (t->misil != NULL && !misil_actualizar(t->misil, dt)) {
        misil_destruir(t->misil);
        t->misil = NULL;
    }
}