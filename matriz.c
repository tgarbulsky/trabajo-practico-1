#include "matriz.h"
#include <stdlib.h>
#include <math.h>

struct matriz {
    float *datos;           // filas x columnas, orden por filas
    size_t filas, columnas;
};

matriz_t *_matriz_crear(size_t filas, size_t columnas) {
    matriz_t *m = malloc(sizeof(matriz_t));
    if (m == NULL) return NULL;

    m->datos = calloc(filas * columnas, sizeof(float));
    if (m->datos == NULL && filas * columnas > 0) {
        free(m);
        return NULL;
    }

    m->filas = filas;
    m->columnas = columnas;
    return m;
}

void matriz_destruir(matriz_t *m) {
    if (m == NULL) return;
    free(m->datos);
    free(m);
}

size_t matriz_filas(const matriz_t *m) {
    return m->filas;
}

size_t matriz_columnas(const matriz_t *m) {
    return m->columnas;
}

float matriz_obtener(const matriz_t *m, size_t fila, size_t columna) {
    return m->datos[fila * m->columnas + columna];
}

void matriz_establecer(matriz_t *m, size_t fila, size_t columna, float valor) {
    m->datos[fila * m->columnas + columna] = valor;
}

matriz_t *matriz_crear_identidad(size_t n) {
    matriz_t *m = _matriz_crear(n, n);
    if (m == NULL) return NULL;

    for (size_t i = 0; i < n; i++)
        matriz_establecer(m, i, i, 1.0f);

    return m;
}

matriz_t *matriz_crear_mper(size_t n) {
    matriz_t *m = matriz_crear_identidad(n);
    if (m == NULL) return NULL;

    // w = -z: la cámara mira hacia -z, los puntos visibles quedan con w > 0
    matriz_establecer(m, n - 1, n - 1, 0.0f);
    matriz_establecer(m, n - 1, n - 2, -1.0f);
    return m;
}

matriz_t *matriz_crear_mz(float angulo) {
    matriz_t *m = matriz_crear_identidad(4);
    if (m == NULL) return NULL;

    float c = cosf(angulo);
    float s = sinf(angulo);
    matriz_establecer(m, 0, 0, c);
    matriz_establecer(m, 0, 1, -s);
    matriz_establecer(m, 1, 0, s);
    matriz_establecer(m, 1, 1, c);
    return m;
}

matriz_t *matriz_crear_my(float angulo) {
    matriz_t *m = matriz_crear_identidad(4);
    if (m == NULL) return NULL;

    float c = cosf(angulo);
    float s = sinf(angulo);
    matriz_establecer(m, 0, 0, c);
    matriz_establecer(m, 0, 2, s);
    matriz_establecer(m, 2, 0, -s);
    matriz_establecer(m, 2, 2, c);
    return m;
}

matriz_t *matriz_crear_mt(const float v[3]) {
    matriz_t *m = matriz_crear_identidad(4);
    if (m == NULL) return NULL;

    matriz_establecer(m, 0, 3, v[0]);
    matriz_establecer(m, 1, 3, v[1]);
    matriz_establecer(m, 2, 3, v[2]);
    return m;
}

matriz_t *matriz_multiplicar(const matriz_t *a, const matriz_t *b) {
    if (a->columnas != b->filas) return NULL;

    matriz_t *r = _matriz_crear(a->filas, b->columnas);
    if (r == NULL) return NULL;

    for (size_t i = 0; i < a->filas; i++) {
        for (size_t j = 0; j < b->columnas; j++) {
            float suma = 0.0f;
            for (size_t k = 0; k < a->columnas; k++)
                suma += matriz_obtener(a, i, k) * matriz_obtener(b, k, j);
            matriz_establecer(r, i, j, suma);
        }
    }
    return r;
}

matriz_t *matriz_aplicar(const matriz_t *t, const matriz_t *puntos) {
    size_t n = puntos->filas;

    matriz_t *r = _matriz_crear(n, 3);
    if (r == NULL) return NULL;

    for (size_t i = 0; i < n; i++) {
        float x = matriz_obtener(puntos, i, 0);
        float y = matriz_obtener(puntos, i, 1);
        float z = matriz_obtener(puntos, i, 2);

        // Punto en coordenadas homogéneas: [x y z 1]
        float xh = matriz_obtener(t, 0, 0) * x + matriz_obtener(t, 0, 1) * y +
                   matriz_obtener(t, 0, 2) * z + matriz_obtener(t, 0, 3);
        float yh = matriz_obtener(t, 1, 0) * x + matriz_obtener(t, 1, 1) * y +
                   matriz_obtener(t, 1, 2) * z + matriz_obtener(t, 1, 3);
        float w  = matriz_obtener(t, 3, 0) * x + matriz_obtener(t, 3, 1) * y +
                   matriz_obtener(t, 3, 2) * z + matriz_obtener(t, 3, 3);

        if (fabsf(w) < 1e-6f) {
            // No se puede dividir: se marca con profundidad 0 y el
            // clipping (w >= 1) lo descarta.
            matriz_establecer(r, i, 0, 0.0f);
            matriz_establecer(r, i, 1, 0.0f);
            matriz_establecer(r, i, 2, 0.0f);
        } else {
            matriz_establecer(r, i, 0, xh / w);
            matriz_establecer(r, i, 1, yh / w);
            matriz_establecer(r, i, 2, w);
        }
    }
    return r;
}
