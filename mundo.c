#include "mundo.h"
#include "lista.h"
#include "tanque.h"
#include "misil.h"
#include "modelo.h"
#include "stl.h"
#include <stdlib.h>
#include <math.h>

#define MAX_ENEMIGOS 1
#define DISTANCIA_RESPAWN 300.0f
#define VELOCIDAD_TANQUE 120.0f
#define VELOCIDAD_ROTACION 2.5f

struct mundo {
    tanque_t *jugador;
    lista_t *enemigos;
    lista_t *misiles;
    
    modelo_t *modelo_tanque;
    modelo_t *modelo_misil;
};

static bool detectar_colision(float x1, float y1, float r1, float x2, float y2, float r2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float distancia_cuadrado = (dx * dx) + (dy * dy);
    float radio_suma = r1 + r2;
    return distancia_cuadrado <= (radio_suma * radio_suma);
}

static void spawnear_enemigo_aleatorio(mundo_t *m) {
    float px, py;
    tanque_obtener_posicion(m->jugador, &px, &py);

    float angulo_aleatorio = ((float)rand() / (float)RAND_MAX) * 2.0f * M_PI;
    float ex = px + cosf(angulo_aleatorio) * DISTANCIA_RESPAWN;
    float ey = py + sinf(angulo_aleatorio) * DISTANCIA_RESPAWN;

    tanque_config_t config_e = {
        .x = ex,
        .y = ey,
        .angulo = angulo_aleatorio + M_PI,
        .velocidad = 60.0f, // Los enemigos se mueven un poco más lento
        .vida_maxima = 50
    };

    tanque_t *nuevo_enemigo = tanque_crear(&config_e);
    if (nuevo_enemigo) {
        lista_insertar_ultimo(m->enemigos, nuevo_enemigo);
    }
}

mundo_t *mundo_crear(void) {
    mundo_t *m = malloc(sizeof(mundo_t));
    if (m == NULL) return NULL;

    m->modelo_tanque = stl_cargar_modelo("tanque.stl");
    m->modelo_misil = stl_cargar_modelo("misil.stl");

    m->enemigos = lista_crear();
    m->misiles = lista_crear();

    tanque_config_t config_p = { .x = 512.0f, .y = 384.0f, .angulo = 0.0f, .velocidad = 0.0f, .vida_maxima = 100 };
    m->jugador = tanque_crear(&config_p);

    if (m->jugador) {
        spawnear_enemigo_aleatorio(m);
    }

    return m;
}

void mundo_destruir(mundo_t *m) {
    if (m == NULL) return;

    tanque_destruir(m->jugador);
    lista_destruir(m->enemigos, (void (*)(void *))tanque_destruir);
    lista_destruir(m->misiles, (void (*)(void *))misil_destruir);

    modelo_destruir(m->modelo_tanque);
    modelo_destruir(m->modelo_misil);
    
    free(m);
}

// Nueva función requerida para conectar el teclado del main sin variables globales
void mundo_procesar_evento(mundo_t *m, const SDL_Event *event) {
    if (m == NULL || event == NULL) return;

    if (event->type == SDL_KEYDOWN) {
        switch(event->key.keysym.sym) {
            case SDLK_UP:
                tanque_establecer_velocidad(m->jugador, VELOCIDAD_TANQUE);
                break;
            case SDLK_DOWN:
                tanque_establecer_velocidad(m->jugador, -VELOCIDAD_TANQUE);
                break;
            case SDLK_RIGHT:
                tanque_rotar(m->jugador, VELOCIDAD_ROTACION * 0.1f);
                break;
            case SDLK_LEFT:
                tanque_rotar(m->jugador, -VELOCIDAD_ROTACION * 0.1f);
                break;
            case ' ':
                mundo_jugador_disparar(m);
                break;
        }
    } else if (event->type == SDL_KEYUP) {
        // Al soltar las flechas detenemos el movimiento lineal
        if (event->key.keysym.sym == SDLK_UP || event->key.keysym.sym == SDLK_DOWN) {
            tanque_establecer_velocidad(m->jugador, 0.0f);
        }
    }
}

void mundo_actualizar(mundo_t *m, float dt) {
    if (m == NULL) return;

    tanque_actualizar(m->jugador, dt);

    if (lista_largo(m->enemigos) < MAX_ENEMIGOS) {
        spawnear_enemigo_aleatorio(m);
    }

    // Lógica interna de IA usando la interfaz pública de tanque
    float px, py;
    tanque_obtener_posicion(m->jugador, &px, &py);

    lista_iter_t *iter_e = lista_iter_crear(m->enemigos);
    while (!lista_iter_al_final(iter_e)) {
        tanque_t *enemigo = lista_iter_ver_actual(iter_e);
        
        float ex, ey;
        tanque_obtener_posicion(enemigo, &ex, &ey);
        
        // El enemigo rota apuntando lentamente en dirección al jugador
        float angulo_hacia_jugador = atan2f(py - ey, px - ex);
        float angulo_enemigo = tanque_obtener_angulo(enemigo);
        float diff_angulo = angulo_hacia_jugador - angulo_enemigo;
        
        tanque_rotar(enemigo, diff_angulo * dt);
        tanque_actualizar(enemigo, dt);
        
        // Disparos de la IA basados en una pequeña probabilidad aleatoria por cuadro
        if ((rand() % 50) == 0) {
            misil_config_t conf = {
                .x = ex + cosf(angulo_enemigo) * 20.0f,
                .y = ey + sinf(angulo_enemigo) * 20.0f,
                .angulo = angulo_enemigo,
                .velocidad = 200.0f,
                .tiempo_vida = 3.0f,
                .danio = 25
            };
            misil_t *nuevo = misil_crear(&conf);
            if (nuevo) lista_insertar_ultimo(m->misiles, nuevo);
        }
        
        lista_iter_avanzar(iter_e);
    }
    lista_iter_destruir(iter_e);

    // Actualizar misiles
    lista_iter_t *iter_misil = lista_iter_crear(m->misiles);
    while (!lista_iter_al_final(iter_misil)) {
        misil_t *misil = lista_iter_ver_actual(iter_misil);
        if (misil_actualizar(misil, dt)) {
            misil_destruir(lista_iter_borrar(iter_misil));
        } else {
            lista_iter_avanzar(iter_misil);
        }
    }
    lista_iter_destruir(iter_misil);

    // Detección de colisiones corregida sin avances dobles
    iter_misil = lista_iter_crear(m->misiles);
    while (!lista_iter_al_final(iter_misil)) {
        misil_t *misil = lista_iter_ver_actual(iter_misil);
        float mx, my;
        misil_obtener_posicion(misil, &mx, &my);
        bool misil_borrado = false;

        // Caso A: Misil impacta en un enemigo
        lista_iter_t *iter_enemigo = lista_iter_crear(m->enemigos);
        while (!lista_iter_al_final(iter_enemigo)) {
            tanque_t *enemigo = lista_iter_ver_actual(iter_enemigo);
            float ex, ey;
            tanque_obtener_posicion(enemigo, &ex, &ey);

            if (detectar_colision(mx, my, 4.0f, ex, ey, 15.0f)) {
                if (tanque_recibir_danio(enemigo, misil_obtener_danio(misil))) {
                    tanque_destruir(lista_iter_borrar(iter_enemigo));
                } else {
                    lista_iter_avanzar(iter_enemigo);
                }
                misil_borrado = true;
                break; 
            } else {
                lista_iter_avanzar(iter_enemigo);
            }
        }
        lista_iter_destruir(iter_enemigo);

        // Caso B: Misil impacta en el propio jugador
        if (!misil_borrado && detectar_colision(mx, my, 4.0f, px, py, 15.0f)) {
            tanque_recibir_danio(m->jugador, misil_obtener_danio(misil));
            misil_borrado = true;
        }

        if (misil_borrado) {
            misil_destruir(lista_iter_borrar(iter_misil));
        } else {
            lista_iter_avanzar(iter_misil);
        }
    }
    lista_iter_destruir(iter_misil);
}

void  mundo_dibujar(mundo_t *m, SDL_Renderer *renderer) {
    if (m == NULL || renderer == NULL) return;

    if (tanque_esta_vivo(m->jugador)) {
        matriz_t *t_jugador = tanque_obtener_matriz_transformacion(m->jugador);
        modelo_dibujar(m->modelo_tanque, t_jugador, renderer);
        matriz_destruir(t_jugador);
    }

    lista_iter_t *iter_e = lista_iter_crear(m->enemigos);
    while (!lista_iter_al_final(iter_e)) {
        tanque_t *enemigo = lista_iter_ver_actual(iter_e);
        matriz_t *t_enemigo = tanque_obtener_matriz_transformacion(enemigo);
        modelo_dibujar(m->modelo_tanque, t_enemigo, renderer);
        matriz_destruir(t_enemigo);
        lista_iter_avanzar(iter_e);
    }
    lista_iter_destruir(iter_e);

    lista_iter_t *iter_m = lista_iter_crear(m->misiles);
    while (!lista_iter_al_final(iter_m)) {
        misil_t *misil = lista_iter_ver_actual(iter_m);
        matriz_t *t_misil = misil_obtener_matriz_transformacion(misil);
        modelo_dibujar(m->modelo_misil, t_misil, renderer);
        matriz_destruir(t_misil);
        lista_iter_avanzar(iter_m);
    }
    lista_iter_destruir(iter_m);
}

void mundo_jugador_disparar(mundo_t *m) {
    if (m == NULL || !tanque_esta_vivo(m->jugador)) return;

    float px, py;
    tanque_obtener_posicion(m->jugador, &px, &py);
    float angulo = tanque_obtener_angulo(m->jugador);

    misil_config_t conf = {
        .x = px + cosf(angulo) * 20.0f,
        .y = py + sinf(angulo) * 20.0f,
        .angulo = angulo,
        .velocidad = 250.0f,
        .tiempo_vida = 3.0f,
        .danio = 50
    };

    misil_t *nuevo = misil_crear(&conf);
    if (nuevo) {
        lista_insertar_ultimo(m->misiles, nuevo);
    }
}