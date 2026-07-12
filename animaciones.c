#include "animaciones.h"
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Estructura interna para los cristales rotos (2D en espacio de pantalla)
typedef struct {
    float x, y;
    float vx, vy;
    float rot;
    float v_rot;
    float tam;
} cristal_t;

struct animacion_cristales {
    cristal_t *particulas;
    size_t cant;
    float tiempo;
};

animacion_cristales_t *animacion_cristales_crear(size_t cant) {
    animacion_cristales_t *a = malloc(sizeof(animacion_cristales_t));
    if (!a) return NULL;
    a->particulas = malloc(sizeof(cristal_t) * cant);
    if (!a->particulas) { free(a); return NULL; }
    a->cant = cant;
    a->tiempo = 0.0f;

    for (size_t i = 0; i < cant; i++) {
        // Nacen en el centro de la pantalla o dispersos por el HUD
        a->particulas[i].x = 0.0f;
        a->particulas[i].y = 0.0f;
        float angulo = ((float)rand() / RAND_MAX) * 2 * M_PI;
        float velocidad = 150.0f + ((float)rand() / RAND_MAX) * 250.0f; // px/seg
        a->particulas[i].vx = velocidad * cosf(angulo);
        a->particulas[i].vy = velocidad * sinf(angulo);
        a->particulas[i].rot = ((float)rand() / RAND_MAX) * 2 * M_PI;
        a->particulas[i].v_rot = 3.0f + ((float)rand() / RAND_MAX) * 8.0f;
        a->particulas[i].tam = 5.0f + ((float)rand() / RAND_MAX) * 12.0f;
    }
    return a;
}

bool animacion_cristales_actualizar(animacion_cristales_t *a, float dt) {
    if (!a) return false;
    a->tiempo += dt;
    if (a->tiempo > 1.2f) return false; // Dura 1.2 segundos

    for (size_t i = 0; i < a->cant; i++) {
        a->particulas[i].x += a->particulas[i].vx * dt;
        a->particulas[i].y += a->particulas[i].vy * dt;
        // Gravedad simulada en la pantalla hacia abajo
        a->particulas[i].vy -= 300.0f * dt;
        a->particulas[i].rot += a->particulas[i].v_rot * dt;
    }
    return true;
}

void animacion_cristales_dibujar(const animacion_cristales_t *a, SDL_Renderer *renderer, int centro_x, int centro_y) {
    if (!a) return;
    SDL_SetRenderDrawColor(renderer, 200, 240, 255, 255); // Color cian/cristal brillante
    for (size_t i = 0; i < a->cant; i++) {
        float cx = centro_x + a->particulas[i].x;
        float cy = centro_y - a->particulas[i].y; // Invertir Y para coordenadas SDL
        float r = a->particulas[i].tam;
        float rot = a->particulas[i].rot;

        // Dibujar un pequeño triángulo o rombo por partícula girando
        int x1 = cx + r * cosf(rot);
        int y1 = cy + r * sinf(rot);
        int x2 = cx + r * cosf(rot + 2.0f * M_PI / 3.0f);
        int y2 = cy + r * sinf(rot + 2.0f * M_PI / 3.0f);
        int x3 = cx + r * cosf(rot + 4.0f * M_PI / 3.0f);
        int y3 = cy + r * sinf(rot + 4.0f * M_PI / 3.0f);

        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        SDL_RenderDrawLine(renderer, x2, y2, x3, y3);
        SDL_RenderDrawLine(renderer, x3, y3, x1, y1);
    }
}

void animacion_cristales_destruir(animacion_cristales_t *a) {
    if (!a) return;
    free(a->particulas);
    free(a);
}

// Estructura interna para fragmentos rotatorios 3D del enemigo destrozado
typedef struct {
    float x1, y1, z1;
    float x2, y2, z2;
    float bx, by, bz;     // Baricentro original de la arista
    float vx, vy, vz;     // Velocidad de expansión
    float rot_intrinseca; // Ángulo de rotación sobre su propio centro
    float vel_rot;        // Velocidad angular
} fragmento_t;

struct animacion_enemigo {
    fragmento_t *fragmentos;
    size_t cant;
    float tiempo;
    float orig_x, orig_y; // Posición de la explosión en el mundo
};

animacion_enemigo_t *animacion_enemigo_crear(const modelo_t *mod, float x, float y) {
    if (!mod) return NULL;
    animacion_enemigo_t *a = malloc(sizeof(animacion_enemigo_t));
    if (!a) return NULL;

    size_t nlineas = modelo_nlineas(mod);
    a->fragmentos = malloc(sizeof(fragmento_t) * nlineas);
    if (!a->fragmentos) { free(a); return NULL; }
    a->cant = nlineas;
    a->tiempo = 0.0f;
    a->orig_x = x;
    a->orig_y = y;

    const float *coords = modelo_coordenadas(mod);
    const size_t *lineas = modelo_lineas(mod);

    for (size_t i = 0; i < nlineas; i++) {
        size_t idx1 = lineas[i * 2];
        size_t idx2 = lineas[i * 2 + 1];

        // Guardamos los vértices locales relativos al centro del tanque
        a->fragmentos[i].x1 = coords[idx1 * 3];
        a->fragmentos[i].y1 = coords[idx1 * 3 + 1];
        a->fragmentos[i].z1 = coords[idx1 * 3 + 2];

        a->fragmentos[i].x2 = coords[idx2 * 3];
        a->fragmentos[i].y2 = coords[idx2 * 3 + 1];
        a->fragmentos[i].z2 = coords[idx2 * 3 + 2];

        // Calculamos el baricentro de la arista para saber la dirección de explosión
        a->fragmentos[i].bx = (a->fragmentos[i].x1 + a->fragmentos[i].x2) / 2.0f;
        a->fragmentos[i].by = (a->fragmentos[i].y1 + a->fragmentos[i].y2) / 2.0f;
        a->fragmentos[i].bz = (a->fragmentos[i].z1 + a->fragmentos[i].z2) / 2.0f;

        // Velocidad de expansión proporcional al baricentro + factor aleatorio
        float factor = 5.0f + ((float)rand() / RAND_MAX) * 10.0f;
        a->fragmentos[i].vx = a->fragmentos[i].bx * factor + (((float)rand() / RAND_MAX) - 0.5f) * 4.0f;
        a->fragmentos[i].vy = a->fragmentos[i].by * factor + (((float)rand() / RAND_MAX) - 0.5f) * 4.0f;
        a->fragmentos[i].vz = (a->fragmentos[i].bz + 1.0f) * factor;

        a->fragmentos[i].rot_intrinseca = 0.0f;
        a->fragmentos[i].vel_rot = 5.0f + ((float)rand() / RAND_MAX) * 15.0f; // rads/seg rápidos
    }
    return a;
}

bool animacion_enemigo_actualizar(animacion_enemigo_t *a, float dt) {
    if (!a) return false;
    a->tiempo += dt;
    if (a->tiempo > 2.0f) return false; // Dura 2 segundos estallando

    for (size_t i = 0; i < a->cant; i++) {
        // Desplazar el baricentro por la velocidad de expansión
        a->fragmentos[i].bx += a->fragmentos[i].vx * dt;
        a->fragmentos[i].by += a->fragmentos[i].vy * dt;
        a->fragmentos[i].bz += a->fragmentos[i].vz * dt;
        // Gravedad sobre los fragmentos en el espacio 3D
        a->fragmentos[i].vz -= 9.8f * dt;

        // Rotación intrínseca sobre sí misma
        a->fragmentos[i].rot_intrinseca += a->fragmentos[i].vel_rot * dt;
    }
    return true;
}

void animacion_enemigo_dibujar(const animacion_enemigo_t *a, SDL_Renderer *renderer, const matriz_t *matriz_vista, int width, int height) {
    if (!a || !matriz_vista) return;

    // Iteramos por cada una de las piezas en las que se desarmó el tanque enemigo
    for (size_t i = 0; i < a->cant_piezas; i++) {
        const pieza_t *p = &a->piezas[i];
        size_t nvertices = p->nvertices;

        // 1. Creamos la matriz dinámica de puntos en espacio local/mundo usando tu constructor
        matriz_t *puntos_locales = _matriz_crear(nvertices, 3);
        if (!puntos_locales) continue;

        // Calculamos rotación intrínseca de la pieza rota
        float c = cosf(p->angulo_rot), s = sinf(p->angulo_rot);

        for (size_t v = 0; v < nvertices; v++) {
            // Rotación local del fragmento sobre su propio centro + traslación por su física de expansión
            float rx = p->vertices_originales[v * 3] * c - p->vertices_originales[v * 3 + 1] * s;
            float ry = p->vertices_originales[v * 3] * s + p->vertices_originales[v * 3 + 1] * c;

            float wx = rx + p->x;
            float wy = ry + p->y;
            float wz = p->vertices_originales[v * 3 + 2] + p->z; // Incorporamos gravedad acumulada en Z

            matriz_establecer(puntos_locales, v, 0, wx);
            matriz_establecer(puntos_locales, v, 1, wy);
            matriz_establecer(puntos_locales, v, 2, wz);
        }

        // 2. Proyectamos usando el pipeline homogéneo de tu TDA Matriz
        matriz_t *puntos_proyectados = matriz_aplicar(matriz_vista, puntos_locales);
        matriz_destruir(puntos_locales);
        if (!puntos_proyectados) continue;

        // 3. Dibujamos las aristas que componen esta pieza
        for (size_t j = 0; j < p->nlineas; j++) {
            size_t i1 = p->lineas[j * 2];
            size_t i2 = p->lineas[j * 2 + 1];

            // Filtro de clipping homogéneo según la columna de profundidad (w) de tu TDA
            float w1 = matriz_obtener(puntos_proyectados, i1, 2);
            float w2 = matriz_obtener(puntos_proyectados, i2, 2);
            if (w1 < 1.0f || w2 < 1.0f) continue;

            float x1_h = matriz_obtener(puntos_proyectados, i1, 0);
            float y1_h = matriz_obtener(puntos_proyectados, i1, 1);
            float x2_h = matriz_obtener(puntos_proyectados, i2, 0);
            float y2_h = matriz_obtener(puntos_proyectados, i2, 1);

            int px1 = (int)(x1_h * (width / 2) + (width / 2));
            int py1 = (int)(height / 2 - y1_h * (height / 2));
            int px2 = (int)(x2_h * (width / 2) + (width / 2));
            int py2 = (int)(height / 2 - y2_h * (height / 2));

            SDL_RenderDrawLine(renderer, px1, py1, px2, py2);
        }

        matriz_destruir(puntos_proyectados);
    }
}

void animacion_enemigo_destruir(animacion_enemigo_t *a) {
    if (!a) return;
    free(a->fragmentos);
    free(a);
}