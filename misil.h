#ifndef MISIL_H
#define MISIL_H

#include <stdbool.h>
#include "matriz.h"

// Estructura opaca para representar un misil
typedef struct misil misil_t;

// Estructura auxiliar para la creación de un misil
typedef struct {
    float x;
    float y;
    float angulo;       // Dirección hacia donde se dispara (en radianes)
    float velocidad;    // Velocidad constante del misil
    float tiempo_vida;  // Cuánto tiempo (en segundos) dura antes de disiparse
    int danio;          // Cuánta vida le resta a lo que golpee
} misil_config_t;

// Crea un misil dinámicamente con una configuración inicial
misil_t *misil_crear(const misil_config_t *config);

// Destruye el misil y libera su memoria
void misil_destruir(misil_t *misil);

// Actualiza la posición del misil y descuenta su tiempo de vida.
// Devuelve true si el misil "expiró" (se quedó sin tiempo de vida).
bool misil_actualizar(misil_t *misil, float dt);

// --- Getters ---
void misil_obtener_posicion(const misil_t *misil, float *x, float *y);
float misil_obtener_angulo(const misil_t *misil);
int misil_obtener_danio(const misil_t *misil);

// Genera y devuelve la matriz de transformación (3x3) del misil para el renderizado
matriz_t *misil_obtener_matriz_transformacion(const misil_t *misil);

#endif // MISIL_H