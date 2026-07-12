#include "tanque.h"
#include <stdlib.h>
#include <math.h>

struct tanque {
    float x;
    float y;
    float angulo;     // En radianes
    float velocidad;  // Magnitud de la velocidad en la dirección del ángulo
    int vida;
    int vida_max;
    // Aquí podrías agregar un puntero a modelo_t si implementás las mallas por separado:
    // modelo_t *modelo;
};

tanque_t *tanque_crear(const tanque_config_t *config) {
    if (config == NULL) return NULL;

    tanque_t *t = malloc(sizeof(tanque_t));
    if (t == NULL) return NULL;

    t->x = config->x;
    t->y = config->y;
    t->angulo = config->angulo;
    t->velocidad = config->velocidad;
    t->vida_max = config->vida_maxima;
    t->vida = config->vida_maxima;

    return t;
}

void tanque_destruir(tanque_t *tanque) {
    if (tanque == NULL) return;
    // Si el tanque guardara un modelo dinámico, se destruiría acá:
    // modelo_destruir(tanque->modelo);
    free(tanque);
}

void tanque_actualizar(tanque_t *tanque, float dt) {
    if (tanque == NULL || tanque->vida <= 0) return;

    // Descomponemos la velocidad en los ejes X e Y usando el ángulo de orientación
    tanque->x += tanque->velocidad * cosf(tanque->angulo) * dt;
    tanque->y += tanque->velocidad * sinf(tanque->angulo) * dt;
}

void tanque_rotar(tanque_t *tanque, float delta_angulo) {
    if (tanque == NULL) return;
    tanque->angulo += delta_angulo;

    // Normalizamos el ángulo entre 0 y 2*PI para evitar desbordes numéricos a largo plazo
    tanque->angulo = fmodf(tanque->angulo, 2.0f * M_PI);
    if (tanque->angulo < 0) {
        tanque->angulo += 2.0f * M_PI;
    }
}

void tanque_establecer_velocidad(tanque_t *tanque, float nueva_velocidad) {
    if (tanque == NULL) return;
    tanque->velocidad = nueva_velocidad;
}

bool tanque_recibir_danio(tanque_t *tanque, int danio) {
    if (tanque == NULL) return false;

    tanque->vida -= danio;
    if (tanque->vida <= 0) {
        tanque->vida = 0;
        return true; // Murió
    }
    return false; // Sigue vivo
}

void tanque_obtener_posicion(const tanque_t *tanque, float *x, float *y) {
    if (tanque == NULL) return;
    if (x) *x = tanque->x;
    if (y) *y = tanque->y;
}

float tanque_obtener_angulo(const tanque_t *tanque) {
    return tanque ? tanque->angulo : 0.0f;
}

int tanque_obtener_vida(const tanque_t *tanque) {
    return tanque ? tanque->vida : 0;
}

bool tanque_esta_vivo(const tanque_t *tanque) {
    return tanque != NULL && tanque->vida > 0;
}

matriz_t *tanque_obtener_matriz_transformacion(const tanque_t *tanque) {
    if (tanque == NULL) return NULL;

    // Generamos las matrices individuales usando el TDA matriz dinámico que armamos
    matriz_t *m_rot = matriz_crear_rotacion(tanque->angulo);
    matriz_t *m_tras = matriz_crear_traslacion(tanque->x, tanque->y);

    if (!m_rot || !m_tras) {
        matriz_destruir(m_rot);
        matriz_destruir(m_tras);
        return NULL;
    }

    // Multiplicamos para obtener la transformación final que se le aplicará al modelo
    // Recordar: transformacion = Traslación * Rotación
    matriz_t *m_final = matriz_multiplicar(m_tras, m_rot);

    // Liberamos las matrices auxiliares intermedias para no perder memoria
    matriz_destruir(m_rot);
    matriz_destruir(m_tras);

    return m_final; 
}