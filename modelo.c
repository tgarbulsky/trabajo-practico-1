#include "modelo.h"
#include <stdlib.h>
#include <string.h>

struct modelo {
    char *nombre;
    float *coordenadas;
    size_t ncoordenadas;
    size_t *lineas;
    size_t nlineas;
};

modelo_t *modelo_crear(const char *nombre, const float *coordenadas, size_t ncoordenadas,
                        const size_t *lineas, size_t nlineas) {
    modelo_t *m = malloc(sizeof(modelo_t));
    if (m == NULL) return NULL;

    m->nombre = malloc(strlen(nombre) + 1);
    if (m->nombre == NULL) { free(m); return NULL; }
    strcpy(m->nombre, nombre);

    m->coordenadas = malloc(sizeof(float) * 3 * ncoordenadas);
    if (m->coordenadas == NULL) { free(m->nombre); free(m); return NULL; }
    memcpy(m->coordenadas, coordenadas, sizeof(float) * 3 * ncoordenadas);

    m->lineas = malloc(sizeof(size_t) * 2 * nlineas);
    if (m->lineas == NULL) { free(m->coordenadas); free(m->nombre); free(m); return NULL; }
    memcpy(m->lineas, lineas, sizeof(size_t) * 2 * nlineas);

    m->ncoordenadas = ncoordenadas;
    m->nlineas = nlineas;
    return m;
}

void modelo_destruir(modelo_t *m) {
    free(m->nombre);
    free(m->coordenadas);
    free(m->lineas);
    free(m);
}

const char *modelo_nombre(const modelo_t *m) { 
    return m->nombre; 
}

const float *modelo_coordenadas(const modelo_t *m) { 
    return m->coordenadas; 
}

size_t modelo_ncoordenadas(const modelo_t *m) { 
    return m->ncoordenadas; 
}

const size_t *modelo_lineas(const modelo_t *m) { 
    return m->lineas; 
}

size_t modelo_nlineas(const modelo_t *m) {
    return m->nlineas;
}

modelo_t *modelo_buscar(lista_t *lista, const char *nombre) {
    lista_iter_t *iter = lista_iter_crear(lista);
    if (iter == NULL) return NULL;

    modelo_t *encontrado = NULL;
    while (!lista_iter_al_final(iter)) {
        modelo_t *m = lista_iter_ver_actual(iter);
        if (strcmp(m->nombre, nombre) == 0) {
            encontrado = m;
            break;
        }
        lista_iter_avanzar(iter);
    }
    lista_iter_destruir(iter);
    return encontrado;
}
