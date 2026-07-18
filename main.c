#include <SDL2/SDL.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

//begin codigo alumno
#include "mundo.h"
#include "animaciones.h"
#include "matriz.h"
#include "modelo.h"
#include "lista.h"
#include "pila.h"
#include "tanque.h"
#include "misil.h"
#include "interfaz2d.h"
#include "obstaculo.h"

// Macro para la ruta del archivo de modelos
#define RUTA_STL "modelos.stl"
//end codigo alumno

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(VENTANA_ANCHO, VENTANA_ALTO, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "Battlezone 3D - Algoritmos y Programación");

    //begin codigo alumno
    srand(time(NULL));

    // CARGA DE MODELOS DEL STL 
    FILE* f = fopen(RUTA_STL, "rb");
    if (f == NULL) return 1;

    unidades_t unidades;
    size_t maxlong;
    if (!verificar_stl(f, &unidades, &maxlong)) {
        fclose(f);
        return 1;
    }

    lista_t* modelos = lista_crear();
    while (!feof(f)) {
        modelo_t* m = leer_modelo(f, maxlong, unidades);
        if (m != NULL) {
            lista_insertar_ultimo(modelos, m);
        }
    }
    fclose(f);

    // INICIALIZACION DEL MUNDO
    tanque_t* jugador = crear_tanque(0, 0, 0);
    lista_t* obstaculos = lista_crear();
    for (int i = 0; i < NRO_OBSTACULOS; i++) {
        lista_insertar_ultimo(obstaculos, crear_obstaculo());
    }
    tanque_t* enemigo = crear_tanque_enemigo(jugador->pos[0], jugador->pos[1], obstaculos);

    matriz_t* pantalla = matriz_crear_pantalla(VENTANA_ALTO, VENTANA_ANCHO);

    // Estado del juego
    unsigned long score = 0;
    char vidas = VIDAS;
    bool running = true;
    
    // Tiempos y cooldowns
    int t_cooldown_jugador = 0;
    int t_muerte_anim = 0;
    int t_destruccion_enemigo = 0;
    float pos_destruccion[6];

    cuerpo_t* misil_jugador = NULL;
    //cuerpo_t* misil_enemigo = NULL;
    
    enum acciones_ia ia_accion = IA_NONE;
    int t_accion_ia = 0;
    //end codigo alumno

    unsigned int frame_ticks = 1000 / JUEGO_FPS;
    while (running) {
        unsigned int start_ticks = SDL_GetTicks();
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }

        //begin codigo alumno
        // --- ENTRADA DE USUARIO ---
        const Uint8* state = SDL_GetKeyboardState(NULL);
        enum tras mov_jug = TRAS_NONE;
        enum rot rot_jug = ROT_NONE;

        if (state[SDL_SCANCODE_W]) mov_jug = TRAS_FWD;
        if (state[SDL_SCANCODE_S]) mov_jug = TRAS_BWD;
        if (state[SDL_SCANCODE_A]) rot_jug = ROT_CCW;
        if (state[SDL_SCANCODE_D]) rot_jug = ROT_CW;

        // --- LOGICA DEL JUEGO ---
        if (t_muerte_anim == 0) {
            tanque_mover(jugador, mov_jug, obstaculos, enemigo);
            tanque_rotar(jugador, rot_jug);
            tanque_radar(jugador);

            if (state[SDL_SCANCODE_SPACE] && t_cooldown_jugador == 0 && misil_jugador == NULL) {
                misil_jugador = malloc(sizeof(cuerpo_t));
                misil_jugador->bloque = MISIL;
                misil_jugador->pos[0] = jugador->pos[0];
                misil_jugador->pos[1] = jugador->pos[1];
                misil_jugador->pos[2] = 3;
                misil_jugador->angz = jugador->angz;
                t_cooldown_jugador = COOLDOWN * JUEGO_FPS;
            }

            // IA Enemiga
            decidir_accion_ia(&ia_accion, &t_accion_ia, enemigo, jugador);
            ia_acciones(enemigo, ia_accion, obstaculos, jugador);
            if (t_accion_ia > 0) t_accion_ia--; else ia_accion = IA_NONE;

            // Torreta enemiga y disparo IA 
            enum turr turr_ia;
            if (en_rango_vision_ia(enemigo, jugador, 1.0, &turr_ia)) {
                tanque_rotar_torreta(enemigo, turr_ia);
            } else {
                tanque_rotar_torreta(enemigo, TURR_OFF);
            }

            // Evolución de misiles 
            if (misil_jugador != NULL) {
                bool kill = false;
                if (!misil_mover(misil_jugador, obstaculos, enemigo, &kill)) {
                    if (kill) {
                        score += SCORE_INC;
                        t_destruccion_enemigo = T_ANIM * JUEGO_FPS;
                        pos_destruccion[0] = enemigo->pos[0];
                        pos_destruccion[1] = enemigo->pos[1];
                        pos_destruccion[2] = enemigo->pos[2];
                        free(enemigo);
                        enemigo = crear_tanque_enemigo(jugador->pos[0], jugador->pos[1], obstaculos);
                    }
                    free(misil_jugador);
                    misil_jugador = NULL;
                }
            }
        } else {
            t_muerte_anim--;
            if (t_muerte_anim == 0 && vidas == 0) running = false;
        }

        if (t_cooldown_jugador > 0) t_cooldown_jugador--;
        if (t_destruccion_enemigo > 0) t_destruccion_enemigo--;

        // --- RENDERIZADO ---
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        pila_t* transformaciones = pila_crear();
        
        //Motores 3D: Camara y Mundo
        if (apilar_cuadro_transformacion((mov_jug != TRAS_NONE), (rot_jug != ROT_NONE), jugador, transformaciones)) {
            mundo_imprimir(modelos, transformaciones, pantalla, renderer);
            imprimir_obstaculos(obstaculos, modelos, transformaciones, pantalla, renderer);
            
            if (t_destruccion_enemigo == 0) {
                tanque_imprimir(enemigo, modelos, transformaciones, pantalla, renderer);
            } else {
                animacion_destruccion(pos_destruccion, t_destruccion_enemigo, modelos, transformaciones, pantalla, renderer);
            }

            if (misil_jugador != NULL) {
                cuerpo_imprimir(misil_jugador, modelos, transformaciones, pantalla, renderer);
            }
            
            desapilar_cuadro_transformacion(transformaciones);
        }

        //Interfaz 2D
        char mira_actual = en_rango_vision_sim(jugador, enemigo, 0.15) ? '+' : '-';
        
        // Calcular posición relativa del enemigo
        enum enemy_to e_pos = FRONT;
        float dy = enemigo->pos[1] - jugador->pos[1];
        float dx = enemigo->pos[0] - jugador->pos[0];
        float ang_rel = atan2(dy, dx) - jugador->angz;
        while (ang_rel < -M_PI) ang_rel += 2 * M_PI;
        while (ang_rel > M_PI) ang_rel -= 2 * M_PI;

        if (fabs(ang_rel) > 2.44) e_pos = BACK;
        else if (ang_rel > 1.0) e_pos = LEFT;
        else if (ang_rel < -1.0) e_pos = RIGHT;

        imprimir_hud(vidas, score, e_pos, mira_actual, modelos, renderer);

        // Animacion de muerte si corresponde
        if (t_muerte_anim > 0) {
            animacion_muerte(t_muerte_anim, 25, vidas, modelos, renderer);
        }

        pila_destruir(transformaciones, matriz_destruir);
        //end codigo alumno

        SDL_RenderPresent(renderer);

        unsigned int elapsed_ticks = SDL_GetTicks() - start_ticks;
        if (elapsed_ticks < frame_ticks) SDL_Delay(frame_ticks - elapsed_ticks);
    }

    //begin codigo alumno
    // --- LIMPIEZA FINAL ---
    lista_destruir(modelos, destruir_modelo);
    lista_destruir(obstaculos, free);
    free(jugador);
    free(enemigo);
    matriz_destruir(pantalla);
    if (misil_jugador) free(misil_jugador);
    //end codigo alumno

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
