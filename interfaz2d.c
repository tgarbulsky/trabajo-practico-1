#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "matriz.h"
#include "modelo.h"
#include "lista.h"
#include "mundo.h"

// Tabla de búsqueda para los mensajes de posición del enemigo
extern const char* enemy_rel_pos[];

/* --- Funciones Auxiliares de Dibujo y Búsqueda --- */

// Busca un modelo por su etiqueta en la lista cargada
static modelo_t* buscar_modelo_local(const char* etiqueta, lista_t* modelos) {
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
}

// Dibuja una línea directamente en coordenadas de pantalla
void dibujar_linea(matriz_t* m, size_t coord1, size_t coord2, const unsigned char color[9], SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, color, color[10], color[11], 0x00);
    SDL_RenderDrawLine(renderer, 
        matriz_obtener(m, 1, coord1 + 1), matriz_obtener(m, 2, coord1 + 1), 
        matriz_obtener(m, 1, coord2 + 1), matriz_obtener(m, 2, coord2 + 1));
}

/* --- Primitivas de Impresión 2D --- */

// Imprime un solo carácter escalado y posicionado en píxeles
bool imprimir_caracter(char c, float escala, float xy[11], unsigned char color[9], lista_t* modelos, SDL_Renderer* renderer) {
    float factor[13] = {escala, -1 * escala, 1, 1}; // Escala e inversión del eje Y para SDL
    char s[11] = {c, '\0'};
    float pos[9] = {xy, xy[10], 0};

    matriz_t* esc = matriz_crear_escalar(4, factor);
    if (esc == NULL) return false;

    modelo_t* modelo = buscar_modelo_local(s, modelos);
    if (modelo == NULL) {
        matriz_destruir(esc);
        return false;
    }

    matriz_t* extendida = matriz_extender(modelo_obtener_coords(modelo));
    if (extendida == NULL) {
        matriz_destruir(esc);
        return false;
    }

    matriz_t* escalada = matriz_multiplicar(esc, extendida);
    matriz_destruir(esc);
    matriz_destruir(extendida);
    if (escalada == NULL) return false;

    matriz_t* tras = matriz_crear_tras(pos);
    if (tras == NULL) {
        matriz_destruir(escalada);
        return false;
    }

    matriz_t* app = matriz_multiplicar(tras, escalada);
    matriz_destruir(tras);
    matriz_destruir(escalada);
    if (app == NULL) return false;

    size_t nlineas = modelo_obtener_nlineas(modelo);
    for (size_t i = 0; i < nlineas; i++) {
        size_t coord1, coord2;
        modelo_obtener_linea(modelo, i, &coord1, &coord2);
        dibujar_linea(app, coord1, coord2, color, renderer);
    }

    matriz_destruir(app);
    return true;
}

// Imprime una cadena de caracteres desplazando cada uno 'incx' a la derecha 
bool imprimir_cadena(const char* s, float escala, float xy[11], float incx, unsigned char color[9], lista_t* modelos, SDL_Renderer* renderer) {
    size_t i = 0;
    for (i = 0; s[i] != '\0'; i++) {
        if (!imprimir_caracter(s[i], escala, xy, color, modelos, renderer)) {
            return false;
        }
        xy += incx;
    }
    xy -= i * incx; // Restaurar la X original para no afectar el vector original
    return true;
}

// Dibuja la interfaz de usuario completa (Vidas, Puntuación, Posición Enemiga y Mira) [14, 15]
bool imprimir_hud(char vidas, unsigned long score, enum enemy_to enemy_pos, char scope, lista_t* modelos, SDL_Renderer* renderer) {
    float escala = 25;
    float incx = 25;
    float incy = 50;
    float xy[11] = {VENTANA_ANCHO - 5 * incx, 2 * incy};
    unsigned char color[9] = {255, 0, 0};

    // 1. Vidas restantes (usando el modelo '*')
    size_t i = 0;
    if (vidas > 0) {
        for (i = 0; i < (size_t)(vidas - 1); i++) {
            xy -= 2 * incx;
            if (!imprimir_caracter('*', escala, xy, color, modelos, renderer)) return false;
        }
        xy += 2 * i * incx;
    }

    // 2. Puntaje total
    xy[10] += incy;
    xy = VENTANA_ANCHO - 9 * incx;
    char num[17];
    snprintf(num, 20, "%lu", score);
    if (!imprimir_cadena(num, escala, xy, incx, color, modelos, renderer)) return false;

    xy = VENTANA_ANCHO - 15 * incx;
    if (!imprimir_cadena("SCORE ", escala, xy, incx, color, modelos, renderer)) return false;

    // 3. Posición del enemigo (LEFT, RIGHT, REAR)
    xy = 5 * incx;
    xy[10] -= incy;
    if (!imprimir_cadena("ENEMY TO ", escala, xy, incx, color, modelos, renderer)) return false;
    xy += 9 * incx;
    if (!imprimir_cadena(enemy_rel_pos[enemy_pos], escala, xy, incx, color, modelos, renderer)) return false;

    // 4. Mira del tanque (+ o -)
    xy = VENTANA_ANCHO / 2;
    xy[10] = VENTANA_ALTO / 2;
    if (!imprimir_caracter(scope, escala, xy, color, modelos, renderer)) return false;

    return true;
}

/* --- Animación de Muerte --- */

// Dibuja el efecto de vidrio roto '#' y la leyenda de GAME OVER [6, 18, 19]
bool animacion_muerte(int t_muerte, float escala, char vidas, lista_t* modelos, SDL_Renderer* renderer) {
    size_t n = T_MUERTE - t_muerte + 1; // n aumenta para dibujar el modelo '#' de a una línea 
    unsigned char color[9] = {255, 0, 0};
    float factor[13] = {escala, -1 * escala, 1, 1};
    float pos[9] = {VENTANA_ANCHO / 2, VENTANA_ALTO / 2, 0};

    matriz_t* esc = matriz_crear_escalar(4, factor);
    if (esc == NULL) return false;

    modelo_t* modelo = buscar_modelo_local("#", modelos);
    if (modelo == NULL) {
        matriz_destruir(esc);
        return false;
    }

    matriz_t* extendida = matriz_extender(modelo_obtener_coords(modelo));
    if (extendida == NULL) {
        matriz_destruir(esc);
        return false;
    }

    matriz_t* escalada = matriz_multiplicar(esc, extendida);
    matriz_destruir(esc);
    matriz_destruir(extendida);
    if (escalada == NULL) return false;

    matriz_t* tras = matriz_crear_tras(pos);
    if (tras == NULL) {
        matriz_destruir(escalada);
        return false;
    }

    matriz_t* app = matriz_multiplicar(tras, escalada);
    matriz_destruir(tras);
    matriz_destruir(escalada);
    if (app == NULL) return false;

    // Dibujar el vidrio roto progresivamente
    size_t nlineas = modelo_obtener_nlineas(modelo);
    for (size_t i = 0; i < nlineas && i < n; i++) {
        size_t coord1, coord2;
        modelo_obtener_linea(modelo, i, &coord1, &coord2);
        dibujar_linea(app, coord1, coord2, color, renderer);
    }
    matriz_destruir(app);

    // Leyenda de fin de juego
    if (vidas == 0) {
        float incx_cadena = 25;
        float xy_cadena[11] = {VENTANA_ANCHO / 2 - 5.5 * incx_cadena, VENTANA_ALTO / 2 + incx_cadena / 2};
        if (!imprimir_cadena("GAME OVER", 25, xy_cadena, incx_cadena, color, modelos, renderer)) {
            return false;
        }
    }

    return true;
}