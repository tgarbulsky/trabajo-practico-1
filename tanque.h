#ifndef TANQUE_H
#define TANQUE_H

#include <stdbool.h>
#include "matriz.h"

// Estructura opaca para representar un tanque
typedef struct tanque tanque_t;

// Estructura auxiliar para pasar los parámetros de configuración inicial
typedef struct {
    float x;
    float y;
    float angulo;
    float velocidad;
    int vida_maxima;
    const char *ruta_modelo_stl; // Por si carga la malla desde un archivo STL
} tanque_config_t;

// Crea un tanque dinámicamente con una configuración inicial
tanque_t *tanque_crear(const tanque_config_t *config);

// Destruye el tanque y libera su memoria asociada
void tanque_destruir(tanque_t *tanque);

// Actualiza la posición y estado del tanque en base al tiempo transcurrido (dt)
void tanque_actualizar(tanque_t *tanque, float dt);

// Aplica una rotación al tanque sumando un delta de ángulo (en radianes)
void tanque_rotar(tanque_t *tanque, float delta_angulo);

// Modifica la velocidad del tanque (frenado, reversa, aceleración)
void tanque_establecer_velocidad(tanque_t *tanque, float nueva_velocidad);

// Aplica daño al tanque restándole puntos de vida. Devuelve true si el tanque murió.
bool tanque_recibir_danio(tanque_t *tanque, int danio);

// --- Getters ---
void tanque_obtener_posicion(const tanque_t *tanque, float *x, float *y);
float tanque_obtener_angulo(const tanque_t *tanque);
int tanque_obtener_vida(const tanque_t *tanque);
bool tanque_esta_vivo(const tanque_t *tanque);

// Genera y devuelve la matriz de transformación (3x3) del tanque (Escalado * Rotación * Traslación)
// Útil para pasársela al renderizador al momento de dibujar el modelo.
matriz_t *tanque_obtener_matriz_transformacion(const tanque_t *tanque);

#endif // TANQUE_H