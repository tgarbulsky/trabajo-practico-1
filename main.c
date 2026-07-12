#include <SDL2/SDL.h>

#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#define VENTANA_ANCHO 1024
#define VENTANA_ALTO 768

#define JUEGO_FPS 24

// BEGIN código del alumno
#include <stdlib.h>
#include <time.h>
#include "lista.h"
#include "cola.h"
#include "pila.h"
#include "stl.h"
#include "matriz.h"
#include "modelo.h"
#include "mundo.h"
#include "animaciones.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Funciones auxiliares para dibujo 2D/3D acopladas a tus TDA nativos
static void mi_dibujar_modelo_2d(SDL_Renderer *renderer, const modelo_t *mod, float x, float y, float escala) {
    if (!mod) return;
    const float *coords = modelo_coordenadas(mod);
    const size_t *lineas = modelo_lineas(mod);
    size_t nlineas = modelo_nlineas(mod);
    for (size_t i = 0; i < nlineas; i++) {
        int x1 = (int)(x + coords[lineas[i * 2] * 3] * escala);
        int y1 = (int)(y - coords[lineas[i * 2] * 3 + 1] * escala);
        int x2 = (int)(x + coords[lineas[i * 2 + 1] * 3] * escala);
        int y2 = (int)(y - coords[lineas[i * 2 + 1] * 3 + 1] * escala);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
}

static void mi_dibujar_texto_hud(SDL_Renderer *renderer, lista_t *lista, const char *texto, float x, float y, float escala) {
    float offset_x = 0;
    for (size_t i = 0; texto[i] != '\0'; i++) {
        char etiqueta[2] = { texto[i], '\0' };
        const modelo_t *char_mod = modelo_buscar(lista, etiqueta);
        if (char_mod) mi_dibujar_modelo_2d(renderer, char_mod, x + offset_x, y, escala);
        offset_x += 25.0f * escala;
    }
}

static void mi_dibujar_modelo_3d(SDL_Renderer *renderer, const modelo_t *mod, float x, float y, float phi, float rot_add, const matriz_t *matriz_vista) {
    if (!mod || !matriz_vista) return;

    size_t nvertices = modelo_ncoordenadas(mod);
    const float *coords = modelo_coordenadas(mod);
    const size_t *lineas = modelo_lineas(mod);
    size_t nlineas = modelo_nlineas(mod);

    // Creamos la matriz dinámica de puntos locales usando tu constructor
    matriz_t *puntos_locales = _matriz_crear(nvertices, 3);
    if (!puntos_locales) return;

    float c = cosf(phi + rot_add), s = sinf(phi + rot_add);
    for (size_t i = 0; i < nvertices; i++) {
        float wx = coords[i * 3] * c - coords[i * 3 + 1] * s + x;
        float wy = coords[i * 3] * s + coords[i * 3 + 1] * c + y;
        float wz = coords[i * 3 + 2];
        matriz_establecer(puntos_locales, i, 0, wx);
        matriz_establecer(puntos_locales, i, 1, wy);
        matriz_establecer(puntos_locales, i, 2, wz);
    }

    // Aplicamos tu proyección homogénea
    matriz_t *puntos_proyectados = matriz_aplicar(matriz_vista, puntos_locales);
    matriz_destruir(puntos_locales);
    if (!puntos_proyectados) return;

    for (size_t i = 0; i < nlineas; i++) {
        size_t i1 = lineas[i * 2];
        size_t i2 = lineas[i * 2 + 1];

        // Filtro de clipping homogéneo según la columna 2 (w) de tu TDA
        float w1 = matriz_obtener(puntos_proyectados, i1, 2);
        float w2 = matriz_obtener(puntos_proyectados, i2, 2);
        if (w1 < 1.0f || w2 < 1.0f) continue;

        float x1_h = matriz_obtener(puntos_proyectados, i1, 0);
        float y1_h = matriz_obtener(puntos_proyectados, i1, 1);
        float x2_h = matriz_obtener(puntos_proyectados, i2, 0);
        float y2_h = matriz_obtener(puntos_proyectados, i2, 1);

        int px1 = (int)(x1_h * (VENTANA_ANCHO / 2) + (VENTANA_ANCHO / 2));
        int py1 = (int)(VENTANA_ALTO / 2 - y1_h * (VENTANA_ALTO / 2));
        int px2 = (int)(x2_h * (VENTANA_ANCHO / 2) + (VENTANA_ANCHO / 2));
        int py2 = (int)(VENTANA_ALTO / 2 - y2_h * (VENTANA_ALTO / 2));

        SDL_RenderDrawLine(renderer, px1, py1, px2, py2);
    }

    matriz_destruir(puntos_proyectados);
}
// END código del alumno

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;

    SDL_CreateWindowAndRenderer(VENTANA_ANCHO, VENTANA_ALTO, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "Battle Zone");

    int dormir = 0;

    // BEGIN código del alumno
    srand((unsigned int)time(NULL));
    FILE *f_stl = fopen("modelos.stl", "rb");
    if (!f_stl) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    unidades_t un; size_t ml;
    leer_encabezado_stl(f_stl);
    leer_formato_STL(f_stl, &un, &ml);

    lista_t *lista_modelos = lista_crear();
    while (!feof(f_stl)) {
        char et[256]; size_t nc, nl; float *cc = NULL; size_t *ll = NULL;
        if (leer_modelo_3d(f_stl, ml, et, &nc, &cc, &nl, &ll)) {
            modelo_t *m = modelo_crear(et, cc, nc, ll, nl);
            if (m) lista_insertar_ultimo(lista_modelos, m);
            free(cc); free(ll);
        } else break;
    }
    fclose(f_stl);

    mundo_t *mundo = mundo_crear(lista_modelos);
    lista_t *an_enemigos = lista_crear();
    animacion_cristales_t *cristales = NULL;
    float dt = 1.0f / JUEGO_FPS;
    // END código del alumno

    unsigned int ticks = SDL_GetTicks();
    while(1) {
        if(SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                break;

            // BEGIN código del alumno
            if (event.type == SDL_KEYDOWN) {
                tanque_t *jugador = mundo_jugador(mundo);
                switch(event.key.keysym.sym) {
                    case SDLK_UP:    tanque_iniciar_movimiento(jugador, MOV_ADELANTE); break;
                    case SDLK_DOWN:  tanque_iniciar_movimiento(jugador, MOV_ATRAS);    break;
                    case SDLK_LEFT:  tanque_iniciar_movimiento(jugador, MOV_GIRAR_IZQ); break;
                    case SDLK_RIGHT: tanque_iniciar_movimiento(jugador, MOV_GIRAR_DER); break;
                    case SDLK_SPACE: tanque_disparar(jugador);                         break;
                }
            }
            // END código del alumno

            continue;
        }

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0x00);

        // BEGIN código del alumno
        // 1. Lógica interna
        mundo_actualizar(mundo, dt);

        if (mundo_evento_jugador_impactado(mundo)) {
            if (cristales) animacion_cristales_destruir(cristales);
            cristales = animacion_cristales_crear(35);
        }
        float ex, ey;
        if (mundo_evento_enemigo_destruido(mundo, &ex, &ey)) {
            const modelo_t *me = modelo_buscar(lista_modelos, "ENEMIGO");
            if (!me) me = modelo_buscar(lista_modelos, "TANQUE");
            animacion_enemigo_t *ae = animacion_enemigo_crear(me, ex, ey);
            if (ae) lista_insertar_ultimo(an_enemigos, ae);
        }
        if (cristales) {
            if (!animacion_cristales_actualizar(cristales, dt)) {
                animacion_cristales_destruir(cristales); cristales = NULL;
            }
        }
        lista_iter_t *it = lista_iter_crear(an_enemigos);
        while (!lista_iter_al_final(it)) {
            if (!animacion_enemigo_actualizar(lista_iter_ver_actual(it), dt)) {
                animacion_enemigo_destruir(lista_iter_ver_actual(it));
                lista_iter_borrar(it);
            } else lista_iter_avanzar(it);
        }
        lista_iter_destruir(it);

        // 2. Renderizado 3D (Cámara construida con tus funciones de matriz.h)
        tanque_t *jugador = mundo_jugador(mundo);
        float jx = tanque_x(jugador), jy = tanque_y(jugador), jphi = tanque_phi(jugador);

        float v_traslacion[3] = {-jx, -jy, 0.0f};
        matriz_t *m_trans = matriz_crear_mt(v_traslacion);
        matriz_t *m_rot = matriz_crear_mz(-jphi - M_PI / 2.0f);
        matriz_t *m_per = matriz_crear_mper(4);

        matriz_t *m_rot_trans = matriz_multiplicar(m_rot, m_trans);
        matriz_t *matriz_camara_completa = matriz_multiplicar(m_per, m_rot_trans);

        matriz_destruir(m_trans);
        matriz_destruir(m_rot);
        matriz_destruir(m_per);
        matriz_destruir(m_rot_trans);

        SDL_SetRenderDrawColor(renderer, 0, 128, 0, 255);
        const modelo_t *mt = modelo_buscar(lista_modelos, "MONTAÑAS");
        if (!mt) mt = modelo_buscar(lista_modelos, "MONTANIAS");
        if (mt) mi_dibujar_modelo_3d(renderer, mt, jx, jy, 0, 0, matriz_camara_completa);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        size_t nobst = mundo_num_obstaculos(mundo);
        for (size_t i = 0; i < nobst; i++) {
            obstaculo_t *o = mundo_obstaculo(mundo, i);
            mi_dibujar_modelo_3d(renderer, obstaculo_modelo(o), obstaculo_x(o), obstaculo_y(o), obstaculo_phi(o), 0, matriz_camara_completa);
        }
        tanque_t *enemigo = mundo_enemigo(mundo);
        if (enemigo) {
            const modelo_t *me = modelo_buscar(lista_modelos, "ENEMIGO");
            if (!me) me = modelo_buscar(lista_modelos, "TANQUE");
            mi_dibujar_modelo_3d(renderer, me, tanque_x(enemigo), tanque_y(enemigo), tanque_phi(enemigo), tanque_torreta(enemigo), matriz_camara_completa);
            if (tanque_misil_activo(enemigo)) {
                const modelo_t *mm = modelo_buscar(lista_modelos, "MISIL");
                if (mm) mi_dibujar_modelo_3d(renderer, mm, tanque_misil_x(enemigo), tanque_misil_y(enemigo), tanque_misil_phi(enemigo), 0, matriz_camara_completa);
            }
        }
        if (tanque_misil_activo(jugador)) {
            const modelo_t *mm = modelo_buscar(lista_modelos, "MISIL");
            if (mm) mi_dibujar_modelo_3d(renderer, mm, tanque_misil_x(jugador), tanque_misil_y(jugador), tanque_misil_phi(jugador), 0, matriz_camara_completa);
        }
        it = lista_iter_crear(an_enemigos);
        while (!lista_iter_al_final(it)) {
            animacion_enemigo_dibujar(lista_iter_ver_actual(it), renderer, matriz_camara_completa, VENTANA_ANCHO, VENTANA_ALTO);
            lista_iter_avanzar(it);
        }
        lista_iter_destruir(it);

        matriz_destruir(matriz_camara_completa);

        // 3. Renderizado HUD 2D
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        char str_pts[32]; sprintf(str_pts, "%06d", mundo_puntaje(mundo));
        mi_dibujar_texto_hud(renderer, lista_modelos, str_pts, 40.0f, 50.0f, 1.2f);

        int vidas = mundo_vidas(mundo); float px_v = 40.0f;
        for (int v = 0; v < vidas; v++) {
            const modelo_t *vm = modelo_buscar(lista_modelos, "*");
            if (vm) mi_dibujar_modelo_2d(renderer, vm, px_v, (float)(VENTANA_ALTO - 50), 0.4f);
            px_v += 25.0f;
        }
        if (enemigo) {
            float obj = atan2f(tanque_y(enemigo) - jy, tanque_x(enemigo) - jx);
            float err = obj - jphi;
            while (err > M_PI) err -= 2 * M_PI;
            while (err <= -M_PI) err += 2 * M_PI;
            const modelo_t *mm = (fabsf(err) <= 0.15f) ? modelo_buscar(lista_modelos, "+") : modelo_buscar(lista_modelos, "-");
            if (mm) mi_dibujar_modelo_2d(renderer, mm, (float)(VENTANA_ANCHO / 2), (float)(VENTANA_ALTO / 2), 1.0f);
        } else {
            const modelo_t *mm = modelo_buscar(lista_modelos, "-");
            if (mm) mi_dibujar_modelo_2d(renderer, mm, (float)(VENTANA_ANCHO / 2), (float)(VENTANA_ALTO / 2), 1.0f);
        }
        if (cristales) animacion_cristales_dibujar(cristales, renderer, VENTANA_ANCHO / 2, VENTANA_ALTO / 2);
        if (mundo_terminado(mundo)) mi_dibujar_texto_hud(renderer, lista_modelos, "GAME OVER", (float)(VENTANA_ANCHO / 2 - 100), (float)(VENTANA_ALTO / 2 + 50), 1.5f);
        // END código del alumno

        SDL_RenderPresent(renderer);
        ticks = SDL_GetTicks() - ticks;
        if(dormir) {
            SDL_Delay(dormir);
            dormir = 0;
        }
        else if(ticks < 1000 / JUEGO_FPS)
            SDL_Delay(1000 / JUEGO_FPS - ticks);
        else
            printf("Perdiendo cuadros\n");
        ticks = SDL_GetTicks();
    }

    // BEGIN código del alumno
    if (cristales) animacion_cristales_destruir(cristales);
    while (!lista_esta_vacia(an_enemigos)) animacion_enemigo_destruir(lista_borrar_primero(an_enemigos));
    lista_destruir(an_enemigos, NULL);
    while (!lista_esta_vacia(lista_modelos)) modelo_destruir(lista_borrar_primero(lista_modelos));
    lista_destruir(lista_modelos, NULL);
    mundo_destruir(mundo);
    // END código del alumno

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}