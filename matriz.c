#include "matriz.h"
#include <stdlib.h>
#include <math.h>

struct matriz {
    size_t filas;
    size_t columnas;
    float **datos;
};

matriz_t *matriz_crear(size_t filas, size_t columnas) {
    matriz_t *m = malloc(sizeof(matriz_t));
    if (!m) return NULL;

    m->filas = filas;
    m->columnas = columnas;
    
    m->datos = malloc(filas * sizeof(float *));
    if (!m->datos) {
        free(m);
        return NULL;
    }

    for (size_t i = 0; i < filas; i++) {
        m->datos[i] = calloc(columnas, sizeof(float));
        if (!m->datos[i]) {
            for (size_t j = 0; j < i; j++) free(m->datos[j]);
            free(m->datos);
            free(m);
            return NULL;
        }
    }
    return m;
}

matriz_t *matriz_crear_identidad(size_t n) {
    matriz_t *m = matriz_crear(n, n);
    if (!m) return NULL;
    for (size_t i = 0; i < n; i++) {
        m->datos[i][i] = 1.0f;
    }
    return m;
}

matriz_t *matriz_crear_traslacion(float tx, float ty) {
    matriz_t *m = matriz_crear_identidad(3);
    if (!m) return NULL;
    m->datos[0][2] = tx;
    m->datos[1][2] = ty;
    return m;
}

matriz_t *matriz_crear_rotacion(float angulo) {
    matriz_t *m = matriz_crear_identidad(3);
    if (!m) return NULL;
    float c = cosf(angulo);
    float s = sinf(angulo);
    m->datos[0][0] = c;  m->datos[0][1] = -s;
    m->datos[1][0] = s;  m->datos[1][1] = c;
    return m;
}

matriz_t *matriz_crear_escalado(float factor) {
    matriz_t *m = matriz_crear_identidad(3);
    if (!m) return NULL;
    m->datos[0][0] = factor;
    m->datos[1][1] = factor;
    return m;
}

void matriz_destruir(matriz_t *m) {
    if (!m) return;
    for (size_t i = 0; i < m->filas; i++) {
        free(m->datos[i]);
    }
    free(m->datos);
    free(m);
}

size_t matriz_filas(const matriz_t *m) { return m->filas; }
size_t matriz_columnas(const matriz_t *m) { return m->columnas; }

float matriz_obtener(const matriz_t *m, size_t fila, size_t columna) {
    return m->datos[fila][columna];
}

void matriz_establecer(matriz_t *m, size_t fila, size_t columna, float valor) {
    m->datos[fila][columna] = valor;
}

matriz_t *matriz_multiplicar(const matriz_t *a, const matriz_t *b) {
    if (a->columnas != b->filas) return NULL;

    matriz_t *r = matriz_crear(a->filas, b->columnas);
    if (!r) return NULL;

    for (size_t i = 0; i < a->filas; i++) {
        for (size_t j = 0; j < b->columnas; j++) {
            r->datos[i][j] = 0;
            for (size_t k = 0; k < a->columnas; k++) {
                r->datos[i][j] += a->datos[i][k] * b->datos[k][j];
            }
        }
    }
    return r;
}

void matriz_aplicar(const matriz_t *m, float *x, float *y) {
    // Multiplicación homogénea 2D usando tu estructura dinámica de 3x3
    float aux_x = m->datos[0][0] * (*x) + m->datos[0][1] * (*y) + m->datos[0][2];
    float aux_y = m->datos[1][0] * (*x) + m->datos[1][1] * (*y) + m->datos[1][2];
    *x = aux_x;
    *y = aux_y;
}