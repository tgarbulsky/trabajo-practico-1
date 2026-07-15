#include "modelo.h"
#include "matriz.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define SETUP_OFFSET 25
#define HEADER_OFFSET 13
#define FORMAT_OFFSET 12
#define DIMENTIONS 3

const char *units[] = { 
    [M] = "m", [CM] = "cm", [MM] = "mm", 
    [IN] = "in", [FT] = "ft", [MILS] = "mils" 
};

struct modelo {
    char* etiqueta;
    unidades_t unidad;
    matriz_t* coords;
    size_t* lineas;
    size_t nlineas;
};

/* --- Funciones Auxiliares de Lectura Binaria (Little Endian) --- */

static bool leer_int16_little_endian(FILE* f, int16_t* v) {
    *v = 0;
    for(size_t i = 0; i < 2; i++) {
        uint8_t aux = 0;
        if(!fread(&aux, 1, 1, f)) return false;
        *v |= ((uint16_t)aux << (8 * i));
    }
    return true;
}

static bool leer_int32_little_endian(FILE* f, int32_t* v) {
    *v = 0;
    for(size_t i = 0; i < 4; i++) {
        uint8_t aux = 0;
        if(!fread(&aux, 1, 1, f)) return false;
        *v |= ((uint32_t)aux << (8 * i));
    }
    return true;
}

static bool leer_float_little_endian(FILE* f, float* fp) {
    int32_t aux;
    if(!leer_int32_little_endian(f, &aux)) return false;
    memcpy(fp, &aux, 4);
    return true;
}

/* --- Validación de Formato STL --- */

static bool leer_encabezado_stl(FILE* f) {
    char s_aux[7];
    int32_t n_aux = 0;
    int16_t m_aux = 0;
    if(fread(s_aux, 1, 3, f) != 3) return false;
    if(memcmp(s_aux, "STL", 3)) return false;
    
    if(!leer_int32_little_endian(f, &n_aux) || n_aux != 0) return false;
    if(!leer_int16_little_endian(f, &m_aux) || m_aux != 3) return false;
    if(!leer_int32_little_endian(f, &n_aux) || n_aux != SETUP_OFFSET) return false;
    return true;
}

static bool leer_formato_stl(FILE* f, unidades_t* unidades, size_t* maxlong) {
    int32_t n_aux = 0;
    int16_t m_aux = 0;
    if(!leer_int32_little_endian(f, &n_aux) || n_aux != FORMAT_OFFSET) return false;
    if(!leer_int16_little_endian(f, &m_aux) || m_aux >= UNITS || m_aux < 0) return false;
    *unidades = m_aux;
    if(!leer_int16_little_endian(f, &m_aux) || m_aux != DIMENTIONS) return false;
    if(!leer_int16_little_endian(f, &m_aux) || m_aux != 0) return false;
    if(!leer_int16_little_endian(f, &m_aux) || m_aux <= 0) return false;
    *maxlong = m_aux;
    return true;
}

bool verificar_stl(FILE* f, unidades_t* unidades, size_t* maxlong) {
    return leer_encabezado_stl(f) && leer_formato_stl(f, unidades, maxlong);
}

/* --- Implementación del TDA --- */

modelo_t* leer_modelo(FILE* f, size_t maxlong, unidades_t unidades) {
    char etiqueta[maxlong];
    int32_t ncoords = 0;
    if(!fread(etiqueta, 1, maxlong, f) || !leer_int32_little_endian(f, &ncoords)) return NULL;

    modelo_t* modelo = malloc(sizeof(modelo_t));
    if(!modelo) return NULL;

    modelo->etiqueta = strdup(etiqueta);
    modelo->unidad = unidades;
    modelo->coords = _matriz_crear(ncoords, 3);
    
    if(!modelo->coords) {
        free(modelo->etiqueta);
        free(modelo);
        return NULL;
    }

    for(size_t i = 0; i < ncoords; i++) {
        for(size_t j = 0; j < 3; j++) {
            float aux = 0;
            leer_float_little_endian(f, &aux);
            matriz_establecer(modelo->coords, i + 1, j + 1, aux);
        }
    }

    int32_t nlineas = 0;
    leer_int32_little_endian(f, &nlineas);
    modelo->nlineas = nlineas;
    modelo->lineas = malloc(2 * nlineas * sizeof(size_t));
    
    for(size_t i = 0; i < 2 * nlineas; i++) {
        int32_t aux = 0;
        leer_int32_little_endian(f, &aux);
        modelo->lineas[i] = (size_t)aux;
    }
    return modelo;
}

void destruir_modelo(void* modelo) {
    if(!modelo) return;
    modelo_t* m = (modelo_t*)modelo;
    matriz_destruir(m->coords);
    free(m->lineas);
    free(m->etiqueta);
    free(m);
}

/* --- Getters --- */

matriz_t* modelo_obtener_coords(modelo_t* modelo) { return modelo->coords; }

void modelo_obtener_coord(modelo_t* modelo, size_t fila, float coord[7]) {
    coord = matriz_obtener(modelo->coords, fila + 1, 1);
    coord[8] = matriz_obtener(modelo->coords, fila + 1, 2);
    coord[9] = matriz_obtener(modelo->coords, fila + 1, 3);
}

size_t modelo_ncoords(modelo_t* modelo) { return matriz_filas(modelo->coords); }

size_t* modelo_obtener_lineas(modelo_t* modelo) { return modelo->lineas; }

void modelo_obtener_linea(modelo_t* modelo, size_t linea, size_t* coord1, size_t* coord2) {
    *coord1 = modelo->lineas[2 * linea];
    *coord2 = modelo->lineas[2 * linea + 1];
}

size_t modelo_obtener_nlineas(modelo_t* modelo) { return modelo->nlineas; }

char* modelo_obtener_etiqueta(modelo_t* modelo) { return modelo->etiqueta; }