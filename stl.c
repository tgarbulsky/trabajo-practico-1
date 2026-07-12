##include "stl.h"
#include <stdlib.h>
#include <stdint.h>

// --- FUNCIONES DE LA CÁTEDRA PARA DESEMPAQUETADO DE BYTES ---

static uint16_t leer_uint16_le(const uint8_t *bytes) {
    return (uint16_t)(bytes[0] | (bytes[1] << 8));
}

static uint32_t leer_uint32_le(const uint8_t *bytes) {
    return (uint32_t)(bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24));
}

static float leer_float_le(const uint8_t *bytes) {
    uint32_t bits = leer_uint32_le(bytes);
    return *(float *)&bits;
}

// --- ESTRUCTURA BINARIA DEL ARCHIVO STL ---

typedef struct {
    uint8_t normal[12];   
    uint8_t v1[12];       
    uint8_t v2[12];       
    uint8_t v3[12];       
    uint8_t atributo[2];  
} registro_triangulo_t;

modelo_t *stl_cargar_modelo(const char *ruta_archivo) {
    if (ruta_archivo == NULL) return NULL;

    FILE *f = fopen(ruta_archivo, "rb");
    if (f == NULL) return NULL;

    char cabecera[80];
    if (fread(cabecera, sizeof(char), 80, f) != 80) {
        fclose(f);
        return NULL;
    }

    uint8_t bytes_cantidad[4];
    if (fread(bytes_cantidad, 1, 4, f) != 4) {
        fclose(f);
        return NULL;
    }
    uint32_t cantidad_triangulos = leer_uint32_le(bytes_cantidad);

    if (cantidad_triangulos == 0) {
        fclose(f);
        return NULL;
    }

    size_t puntos_totales = cantidad_triangulos * 4;
    modelo_t *m = modelo_crear(puntos_totales);
    if (m == NULL) {
        fclose(f);
        return NULL;
    }

    registro_triangulo_t reg;
    size_t indice_punto = 0;

    for (uint32_t i = 0; i < cantidad_triangulos; i++) {
        if (fread(&reg, sizeof(registro_triangulo_t), 1, f) != 1) {
            modelo_destruir(m);
            fclose(f);
            return NULL;
        }

        float v1_x = leer_float_le(&reg.v1[0]);
        float v1_y = leer_float_le(&reg.v1[4]);
        float v2_x = leer_float_le(&reg.v2[0]);
        float v2_y = leer_float_le(&reg.v2[4]);
        float v3_x = leer_float_le(&reg.v3[0]);
        float v3_y = leer_float_le(&reg.v3[4]);

        modelo_establecer_punto(m, indice_punto++, v1_x, v1_y);
        modelo_establecer_punto(m, indice_punto++, v2_x, v2_y);
        modelo_establecer_punto(m, indice_punto++, v3_x, v3_y);
        modelo_establecer_punto(m, indice_punto++, v1_x, v1_y);
    }

    fclose(f);
    return m;
}