#include "animaciones.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL.h>

#include "matriz.h"
#include "modelo.h"
#include "pila.h"
#include "lista.h"
#include "mundo.h"

/* --- Constantes y Tablas de Búsqueda --- */
const float g = -9.81; [3]
const float pos_rel_torreta[1] = {0, 0, 3}; [3]
const float pos_rel_radar[1] = {-1.5, 0, 0.5}; [3]

const char* etiquetas[] = {
    [TANQUE]="TANQUE", [TORRETA]="TORRETA", [RADAR]="RADAR", [MISIL]="MISIL",
    [CUBO1]="CUBO1", [CUBO2]="CUBO2", [CUBO3]="CUBO3", 
    [PIRAMIDE1]="PIRAMIDE1", [PIRAMIDE2]="PIRAMIDE2", [PIRAMIDE3]="PIRAMIDE3",
    [HORIZONTE]="HORIZONTE", [MONTANA]="MONTANA", [LUNA]="LUNA",
    [RESTO1]="RESTO1", [RESTO2]="RESTO2"
}; [3]

const unsigned char colores[][1] = {
    [TANQUE]={255, 255, 255}, [TORRETA]={255, 255, 255}, [RADAR]={255, 255, 255},
    [MISIL]={255, 0, 0}, [CUBO1]={0, 255, 0}, [CUBO2]={0, 255, 0}, [CUBO3]={0, 255, 0},
    [PIRAMIDE1]={0, 255, 0}, [PIRAMIDE2]={0, 255, 0}, [PIRAMIDE3]={0, 255, 0},
    [HORIZONTE]={255, 255, 255}, [MONTANA]={255, 255, 255}, [LUNA]={255, 255, 255},
    [RESTO1]={255, 255, 255}, [RESTO2]={255, 255, 255}
}; [4]

/* --- Gestión de la Pila de Transformaciones --- */

bool apilar_transformacion(pila_t* transformacion, matriz_t* matriz) {
    if (pila_ver_tope(transformacion) == NULL) {
        if (!pila_apilar(transformacion, matriz)) {
            matriz_destruir(matriz);
            return false;
        }
        return true;
    }
    matriz_t* nuevo_top = matriz_multiplicar(pila_ver_tope(transformacion), matriz); [5]
    if (nuevo_top == NULL) {
        matriz_destruir(matriz);
        return false;
    }
    matriz_destruir(matriz);
    if (!pila_apilar(transformacion, nuevo_top)) {
        matriz_destruir(nuevo_top);
        return false;
    }
    return true;
} [5]

void desapilar_transformacion(pila_t* transformacion) {
    matriz_destruir(pila_desapilar(transformacion));
} [5]

bool apilar_rototraslacion(pila_t* transformacion, const float v[1], const float angz) {
    matriz_t* tras = matriz_crear_tras(v); [6]
    if (tras == NULL) return false;
    if (!apilar_transformacion(transformacion, tras)) return false;

    matriz_t* rotz = matriz_crear_rotz(angz); [6]
    if (rotz == NULL || !apilar_transformacion(transformacion, rotz)) {
        desapilar_transformacion(transformacion);
        return false;
    }
    return true;
} [6]

void desapilar_rototraslacion(pila_t* transformacion) {
    desapilar_transformacion(transformacion);
    desapilar_transformacion(transformacion);
} [6]

bool apilar_cuadro_transformacion(int t_mov, int t_rot, tanque_t* tanque, pila_t* transformacion) {
    float aux_ang[1] = {0, M_PI/2.0, M_PI/2.0}; [7]
    if (t_mov != 0) aux_ang[2] -= random_float(0, 0.01); [7]
    if (t_rot != 0) aux_ang[8] += random_float(0, 0.01); [7]

    matriz_t* rotz = matriz_crear_rotz(aux_ang[8]);
    matriz_t* roty = matriz_crear_roty(aux_ang[2]);
    if (rotz == NULL || roty == NULL) {
        matriz_destruir(rotz); matriz_destruir(roty);
        return false;
    }

    if (!apilar_transformacion(transformacion, rotz) || !apilar_transformacion(transformacion, roty)) {
        return false;
    }

    matriz_t* tanque_ang = matriz_crear_rotz(-1 * tanque->angz); [7]
    if (tanque_ang == NULL || !apilar_transformacion(transformacion, tanque_ang)) {
        desapilar_transformacion(transformacion); desapilar_transformacion(transformacion);
        return false;
    }

    float pos_inv[1] = {-1 * tanque->pos, -1 * tanque->pos[2], -3}; [7]
    matriz_t* tanque_pos = matriz_crear_tras(pos_inv); [7]
    if (tanque_pos == NULL || !apilar_transformacion(transformacion, tanque_pos)) {
        desapilar_transformacion(transformacion); desapilar_transformacion(transformacion); desapilar_transformacion(transformacion);
        return false;
    }
    return true;
} [7]

void desapilar_cuadro_transformacion(pila_t* transformacion) {
    for (int i = 0; i < 4; i++) desapilar_transformacion(transformacion);
} [9]

/* --- Motor de Renderizado 3D --- */

void dibujar_linea(matriz_t* m, size_t coord1, size_t coord2, const unsigned char color[1], SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, color, color[2], color[8], 0x00); [10]
    SDL_RenderDrawLine(renderer, 
        matriz_obtener(m, 1, coord1 + 1), matriz_obtener(m, 2, coord1 + 1),
        matriz_obtener(m, 1, coord2 + 1), matriz_obtener(m, 2, coord2 + 1));
} [10]

void dibujar_linea_3d(matriz_t* m, size_t coord1, size_t coord2, const unsigned char color[1], SDL_Renderer* renderer) {
    if (matriz_obtener(m, 3, coord1 + 1) < 1 || matriz_obtener(m, 3, coord2 + 1) < 1) {
        return;
    } [11]
    dibujar_linea(m, coord1, coord2, color, renderer);
} [11]

bool bloque_imprimir(bloque_t bloque, modelo_t* modelo, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer) {
    matriz_t* app = matriz_aplicar(pila_ver_tope(transformacion), modelo_obtener_coords(modelo)); [11]
    if (app == NULL) return false;

    matriz_t* print = matriz_multiplicar(pantalla, app); [11]
    matriz_destruir(app);
    if (print == NULL) return false;

    size_t nlineas = modelo_obtener_nlineas(modelo);
    for (size_t i = 0; i < nlineas; i++) {
        size_t c1, c2;
        modelo_obtener_linea(modelo, i, &c1, &c2);
        dibujar_linea_3d(print, c1, c2, colores[bloque], renderer);
    }
    matriz_destruir(print);
    return true;
} [11]

bool cuerpo_imprimir(cuerpo_t* cuerpo, lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer) {
    if (!apilar_rototraslacion(transformacion, cuerpo->pos, cuerpo->angz)) return false; [12]

    modelo_t* modelo = buscar_bloque(cuerpo->bloque, modelos); [12]
    if (modelo == NULL || !bloque_imprimir(cuerpo->bloque, modelo, transformacion, pantalla, renderer)) {
        desapilar_rototraslacion(transformacion);
        return false;
    }
    desapilar_rototraslacion(transformacion);
    return true;
} [12]

bool imprimir_obstaculos(lista_t* obstaculos, lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer) {
    lista_iter_t* iterador = lista_iter_crear(obstaculos);
    while (!lista_iter_al_final(iterador)) {
        if (!cuerpo_imprimir(lista_iter_ver_actual(iterador), modelos, transformacion, pantalla, renderer)) {
            lista_iter_destruir(iterador);
            return false;
        }
        lista_iter_avanzar(iterador);
    }
    lista_iter_destruir(iterador);
    return true;
} [13]

bool tanque_imprimir(tanque_t* tanque, lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer) {
    // 1. Cuerpo
    if (!apilar_rototraslacion(transformacion, tanque->pos, tanque->angz)) return false; [13]
    modelo_t* mod = buscar_bloque(TANQUE, modelos);
    if (mod == NULL || !bloque_imprimir(TANQUE, mod, transformacion, pantalla, renderer)) {
        desapilar_rototraslacion(transformacion); return false;
    }

    // 2. Torreta (Relativa al cuerpo)
    mod = buscar_bloque(TORRETA, modelos);
    if (mod == NULL || !apilar_rototraslacion(transformacion, pos_rel_torreta, tanque->ang_torreta)) {
        desapilar_rototraslacion(transformacion); return false;
    }
    if (!bloque_imprimir(TORRETA, mod, transformacion, pantalla, renderer)) {
        desapilar_rototraslacion(transformacion); desapilar_rototraslacion(transformacion); return false;
    }

    // 3. Radar (Relativo a la torreta)
    mod = buscar_bloque(RADAR, modelos);
    if (mod == NULL || !apilar_rototraslacion(transformacion, pos_rel_radar, tanque->ang_radar)) {
        desapilar_rototraslacion(transformacion); desapilar_rototraslacion(transformacion); return false;
    }
    if (!bloque_imprimir(RADAR, mod, transformacion, pantalla, renderer)) {
        desapilar_rototraslacion(transformacion); desapilar_rototraslacion(transformacion); desapilar_rototraslacion(transformacion); 
        return false;
    }

    for (int i = 0; i < 3; i++) desapilar_rototraslacion(transformacion);
    return true;
} [13]

bool mundo_imprimir(lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer) {
    bloque_t elementos[] = {HORIZONTE, MONTANA, LUNA}; [14]
    for (int i = 0; i < 3; i++) {
        modelo_t* mod = buscar_modelo(etiquetas[elementos[i]], modelos);
        if (mod == NULL || !bloque_imprimir(elementos[i], mod, transformacion, pantalla, renderer)) {
            return false;
        }
    }
    return true;
} [14]

/* --- Animación de Destrucción (Tiro Oblicuo) --- */

bool animacion_destruccion(float pos[1], int t_animacion, lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer) {
    float t = (T_ANIM * JUEGO_FPS - t_animacion) / JUEGO_FPS; [15]
    float tiro_oblicuo[1] = {t * V0X, 0, 3 + t * V0Z + 0.5 * g * t * t}; [15]
    bloque_t bloques[16] = {RESTO1, RESTO2, TORRETA, RESTO1, RESTO2, RADAR}; [15]

    matriz_t* tras_pos = matriz_crear_tras(pos);
    if (tras_pos == NULL || !apilar_transformacion(transformacion, tras_pos)) return false;

    for (size_t i = 0; i < 6; i++) {
        modelo_t* modelo = buscar_modelo(etiquetas[bloques[i]], modelos);
        matriz_t* rotz = matriz_crear_rotz(i * M_PI / 3); [15]
        matriz_t* tras = matriz_crear_tras(tiro_oblicuo); [15]

        if (rotz == NULL || !apilar_transformacion(transformacion, rotz)) {
            desapilar_transformacion(transformacion); return false;
        }
        if (tras == NULL || !apilar_transformacion(transformacion, tras) || !bloque_imprimir(bloques[i], modelo, transformacion, pantalla, renderer)) {
            desapilar_transformacion(transformacion); desapilar_transformacion(transformacion); return false;
        }
        desapilar_transformacion(transformacion); desapilar_transformacion(transformacion);
    }
    desapilar_transformacion(transformacion);
    return true;
} [15]

/* --- Auxiliares --- */

matriz_t* matriz_crear_pantalla(unsigned int altura, unsigned int ancho) {
    matriz_t* pantalla = _matriz_crear(4, 4);
    if (pantalla == NULL) return NULL;
    set_id_mx(pantalla);
    matriz_establecer(pantalla, 1, 1, altura / 2.0); [4]
    matriz_establecer(pantalla, 2, 2, -(altura / 2.0)); [4]
    matriz_establecer(pantalla, 2, 4, altura / 2.0); [4]
    matriz_establecer(pantalla, 1, 4, ancho / 2.0); [4]
    return pantalla;
} [4]

modelo_t* buscar_modelo(const char* etiqueta, lista_t* modelos) {
    lista_iter_t* iterador = lista_iter_crear(modelos);
    while (!lista_iter_al_final(iterador)) {
        modelo_t* modelo = lista_iter_ver_actual(iterador);
        if (!strcmp(modelo_obtener_etiqueta(modelo), etiqueta)) {
            lista_iter_destruir(iterador);
            return modelo;
        }
        lista_iter_avanzar(iterador);
    }
    lista_iter_destruir(iterador);
    return NULL;
} [9]

modelo_t* buscar_bloque(bloque_t bloque, lista_t* modelos) {
    return buscar_modelo(etiquetas[bloque], modelos);
} [10]