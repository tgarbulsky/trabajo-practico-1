#include "animaciones.h"
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define CRISTAL_DURACION_TRAZADO 1.35f 
#define CRISTAL_DURACION_TOTAL 2.5f
#define EXPLOSION_TIMEOUT 5.0f
#define GRAVEDAD 9.81f
#define PIEZA_VEL_HORIZONTAL 5.0f  
#define PIEZA_VEL_VERTICAL 10.0f     
#define PIEZA_ALTURA_INICIAL 3.0f

typedef struct {
    float dist;      // distancia horizontal recorrida desde el centro
    float z, vz;     // altura y velocidad vertical (tiro oblicuo)
    float ang;       // dirección de eyección 
    float rot, vrot; // rotación propia acumulada y su velocidad
    bool en_suelo;
} pieza_t;

typedef struct {
    float x0, y0, x1, y1;
} linea_t;

struct animacion {
    anim_tipo_t tipo;
    bool activa;
    float tiempo;

    // Cristal del jugador
    linea_t lineas[CRISTAL_LINEAS];
    size_t visibles;

    // Explosión del enemigo
    float cx, cy; // centro de la explosión en el mundo
    pieza_t piezas[EXPLOSION_PIEZAS];
};

// Tabla de búsqueda: modelo de cada pieza eyectada
static const char *PIEZA_MODELOS[EXPLOSION_PIEZAS] = {
    "TORRETA", "RADAR", "RESTO1", "RESTO1", "RESTO2", "RESTO2"
};

static float aleatorio(float min, float max) {
    return min + (max - min) * (float)rand() / (float)RAND_MAX;
}

animacion_t *animacion_crear(void) {
    animacion_t *a = calloc(1, sizeof(animacion_t));
    if (a) a->tipo = ANIM_NINGUNA;
    return a;
}

void animacion_destruir(animacion_t *a) {
    free(a);
}

bool animacion_activa(const animacion_t *a) {
    return a->activa;
}

anim_tipo_t animacion_tipo(const animacion_t *a) {
    return a->tipo;
}

void animacion_iniciar_cristal(animacion_t *a) {
    a->tipo = ANIM_CRISTAL;
    a->activa = true;
    a->tiempo = 0;
    a->visibles = 0;

    for (size_t i = 0; i < CRISTAL_LINEAS; i++) {
        float ang = aleatorio(-(float)M_PI, (float)M_PI);
        float r0 = aleatorio(0.0f, 0.4f);
        float r1 = r0 + aleatorio(0.2f, 1.0f);
        a->lineas[i].x0 = r0 * cosf(ang);
        a->lineas[i].y0 = r0 * sinf(ang);
        a->lineas[i].x1 = r1 * cosf(ang + aleatorio(-0.3f, 0.3f));
        a->lineas[i].y1 = r1 * sinf(ang + aleatorio(-0.3f, 0.3f));
    }
}

void animacion_iniciar_explosion(animacion_t *a, float x, float y) {
    a->tipo = ANIM_EXPLOSION;
    a->activa = true;
    a->tiempo = 0;
    a->cx = x;
    a->cy = y;

    for (size_t i = 0; i < EXPLOSION_PIEZAS; i++) {
        pieza_t *p = &a->piezas[i];
        p->dist = 0;
        p->z = PIEZA_ALTURA_INICIAL;
        p->vz = PIEZA_VEL_VERTICAL;
        p->ang = (float)i * 60.0f * (float)M_PI / 180.0f;
        p->rot = 0;
        p->vrot = aleatorio(-3.0f, 3.0f);
        p->en_suelo = false;
    }
}

void animacion_actualizar(animacion_t *a, float dt) {
    if (!a->activa) return;
    a->tiempo += dt;

    if (a->tipo == ANIM_CRISTAL) {
        float progreso = a->tiempo / CRISTAL_DURACION_TRAZADO;
        if (progreso > 1) progreso = 1;
        a->visibles = (size_t)(progreso * CRISTAL_LINEAS);

        if (a->tiempo >= CRISTAL_DURACION_TOTAL) a->activa = false;
    } else if (a->tipo == ANIM_EXPLOSION) {
        bool todas_en_suelo = true;

        for (size_t i = 0; i < EXPLOSION_PIEZAS; i++) {
            pieza_t *p = &a->piezas[i];
            if (p->en_suelo) continue;

            // Tiro oblicuo: avance horizontal constante, caída con g
            p->dist += PIEZA_VEL_HORIZONTAL * dt;
            p->vz -= GRAVEDAD * dt;
            p->z += p->vz * dt;
            p->rot += p->vrot * dt;

            if (p->z <= 0) {
                p->z = 0;
                p->en_suelo = true;
            } else {
                todas_en_suelo = false;
            }
        }

        if (todas_en_suelo || a->tiempo > EXPLOSION_TIMEOUT) a->activa = false;
    }
}

size_t animacion_cristal_visibles(const animacion_t *a) {
    if (!a->activa || a->tipo != ANIM_CRISTAL) return 0;
    return a->visibles;
}

void animacion_cristal_linea(const animacion_t *a, size_t i,
                             float *x0, float *y0, float *x1, float *y1) {
    *x0 = a->lineas[i].x0;
    *y0 = a->lineas[i].y0;
    *x1 = a->lineas[i].x1;
    *y1 = a->lineas[i].y1;
}

const char *animacion_pieza_modelo(const animacion_t *a, size_t i) {
    (void)a;
    return PIEZA_MODELOS[i];
}

bool animacion_pieza_posicion(const animacion_t *a, size_t i,
                              float *x, float *y, float *z, float *rot) {
    if (!a->activa || a->tipo != ANIM_EXPLOSION || i >= EXPLOSION_PIEZAS)
        return false;

    const pieza_t *p = &a->piezas[i];
    *x = a->cx + p->dist * cosf(p->ang);
    *y = a->cy + p->dist * sinf(p->ang);
    *z = p->z;
    *rot = p->rot;
    return true;
}
