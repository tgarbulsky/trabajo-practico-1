#ifndef MODELO_H
#define MODELO_H

#include <stddef.h>
#include <stdbool.h>
#include "lista.h"

typedef struct modelo modelo_t;

modelo_t *modelo_crear(const char *nombre, const float *coordenadas, size_t ncoordenadas,
    const size_t *lineas, size_t nlineas);

void modelo_destruir(modelo_t *m);

const char *modelo_nombre(const modelo_t *m);

const float *modelo_coordenadas(const modelo_t *m);

size_t modelo_ncoordenadas(const modelo_t *m);

const size_t *modelo_lineas(const modelo_t *m);

size_t modelo_nlineas(const modelo_t *m);

// Búsqueda sobre la lista de modelos: devuelve el modelo cuyo nombre
// coincide con 'nombre', o NULL si no está.
modelo_t *modelo_buscar(lista_t *lista, const char *nombre);

#endif
