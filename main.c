#include <SDL2/SDL.h>

#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h> 
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "mundo.h"
#include "matriz.h"
#include "modelo.h"
#include "lista.h"
#include "pila.h"
#include "animaciones.h"
#include "interfaz2d.h"
#include "tanque.h"
#include "misil.h"
#include "obstaculo.h"

#define VENTANA_ANCHO 1024
#define VENTANA_ALTO 768

#define JUEGO_FPS 24

#define RUTA_STL "modelos.stl"

int main(int argc, char *argv[]) {
    
    // BEGIN código del alumno
    /*Inicialización*/
    
    //Inicialización de variables
    char vidas=VIDAS; //Vidas
    unsigned long score=0; //Puntuación
    char t_mov=0; //Tiempo de movimiento (traslación) en cuadros
    enum tras mov=TRAS_NONE; //Enumerativo de movimiento (traslación)
    char t_rot=0; //Tiempo de movimiento (rotación) en cuadros
    enum rot rot=ROT_NONE; //Enumerativo de movimiento (rotación)
    bool misil_activo=false;
    int t_misil=0; //Tiempo del misil en cuadros
    int ia_t_accion=0; //Tiempo de acción del enemigo en cuadros
    enum acciones_ia ia_accion=IA_NONE; //Enumerativo de acciones del tanque enemigo
    enum turr turr=TURR_NONE; //Enumerativo de estado de la torreta del tanque enemigo
    bool ia_misil_activo=false;
    int ia_t_misil=0; //Tiempo del misil enemigo en cuadros
    bool enemigo_vivo=false; //Estado del enemigo
    int t_animacion=0; //Tiempo de animacion de destruccion enemiga en cuadros
    int t_muerte=0; //Tiempo de animación de muerte del jugador
    enum enemy_to enemy_pos=FRONT; //Enumerativo de las posiciones relativas del tanque enemigo
    char scope='-'; //Caracter de modelo de mira
    //

    //Inicialización de cuerpos
    //Alcance mayor al ciclo para asegurar un puntero válido para free
    tanque_t* fp=NULL; //Jugador (fp: first person)
    tanque_t* ia=NULL; //Enemigo
    cuerpo_t* misil=NULL;
    cuerpo_t* misil_ia=NULL;

    //Inicializar tanque del jugador en el origen
    fp=crear_tanque(0, 0, 0); 
    if(fp==NULL){
        return 1;
    }
    //

    //Inicialización de pseudoaleatorios
    srand(time(NULL));
    //

    //Lectura de modelos
    FILE* stl=fopen(RUTA_STL, "rb");
    if(stl==NULL){
        free(fp);
        return 1;
    }
    unidades_t unidades;
    size_t maxlong;
    if(!verificar_stl(stl, &unidades, &maxlong)){
        free(fp);
        fclose(stl);
        return 1;
    }
    lista_t* modelos=lista_crear();
    if(modelos==NULL){
        free(fp);
        fclose(stl);
        return 1;
    }
    modelo_t* aux;
    while((aux=leer_modelo(stl, maxlong, unidades))!=NULL){
        if(!lista_insertar_primero(modelos, aux)){
            free(fp);
            lista_destruir(modelos, destruir_modelo);
            fclose(stl);
            return 1;
        }
    }
    fclose(stl);
    //
    
    //Inicialización de la transformación
    pila_t* transformacion=pila_crear();
    if(transformacion==NULL){
        free(fp);
        lista_destruir(modelos, destruir_modelo);
        return 1;
    }
    matriz_t* proyeccion=matriz_crear_proy(4);
    if(proyeccion==NULL){
        free(fp);
        lista_destruir(modelos, destruir_modelo);
        pila_destruir(transformacion, matriz_destruir);
        return 1;
    }
    if(!apilar_transformacion(transformacion, proyeccion)){
        free(fp);
        lista_destruir(modelos, destruir_modelo);
        pila_destruir(transformacion, matriz_destruir);
        return 1;
    }
    //
    
    //Crear transformación Mundo-Pantalla para coordenadas ya transformadas
    matriz_t* pantalla=matriz_crear_pantalla(VENTANA_ALTO, VENTANA_ANCHO);
    if(pantalla==NULL){
        free(fp);
        lista_destruir(modelos, destruir_modelo);
        pila_destruir(transformacion, matriz_destruir);
        return 1;
    }
    //

    //Generar obstáculos aleatorios
    lista_t* obstaculos=lista_crear();
    if(obstaculos==NULL){
        free(fp);
        lista_destruir(modelos, destruir_modelo);
        pila_destruir(transformacion, matriz_destruir);
        return 1;
    }
    for(size_t i=0; i<NRO_OBSTACULOS; i++){
        cuerpo_t* aux=crear_obstaculo();
        if(aux==NULL || !lista_insertar_primero(obstaculos, aux)){
            free(fp);
            lista_destruir(obstaculos, free);
            lista_destruir(modelos, destruir_modelo);
            pila_destruir(transformacion, matriz_destruir);
            return 1;
        }
    }
    //

    /*Fin de inicialización*/
    // END código del alumno
    
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;

    SDL_CreateWindowAndRenderer(VENTANA_ANCHO, VENTANA_ALTO, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "Battle Zone");

    int dormir = 0;

    unsigned int ticks = SDL_GetTicks();
    while(1) {
        if(SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                break;

            // BEGIN código del alumno
            if (event.type == SDL_KEYDOWN) {
                // Se apretó una tecla
                switch(event.key.keysym.sym) {
                    case SDLK_UP:
                        t_mov=T_ACCION*JUEGO_FPS;
                        mov=TRAS_FWD;
                        break;
                    case SDLK_DOWN:
                        t_mov=T_ACCION*JUEGO_FPS;
                        mov=TRAS_BWD;
                        break;
                    case SDLK_RIGHT:
                        t_rot=T_ACCION*JUEGO_FPS;
                        rot=ROT_CW;
                        break;
                    case SDLK_LEFT:
                        t_rot=T_ACCION*JUEGO_FPS;
                        rot=ROT_CCW;
                        break;
                    case ' ':
                        if(t_misil==0 && !misil_activo){
                            t_misil=COOLDOWN*JUEGO_FPS;
                            misil_activo=true;
                        }
                        break;
                }
            }
            // END código del alumno

            continue;
        }

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0x00);

        // BEGIN código del alumno
        /*Ciclo de cuadro*/

        if(vidas==0 && t_muerte==0){
            break;
        }

        //Si el jugador ya está muerto, pero sigue la animación, se anulan las acciones
        if(vidas==0){
            t_mov=0;
            t_rot=0;
            t_misil=0;
            misil_activo=false;
            mov=TRAS_NONE;
            rot=ROT_NONE;
        }

        //Si el misil del jugador no está creado pero se ha disparado, lo crea
        if(misil_activo && misil==NULL){
            misil=malloc(sizeof(cuerpo_t));
            if(misil==NULL){
                break;
            }
            for(size_t i=0; i<2; i++){
                misil->pos[i]=fp->pos[i];
            }
            misil->pos[2]=3;
            misil->angz=fp->angz;
            misil->bloque=MISIL;
        }

        //Si no hay tanque enemigo y la animación de destrucción ya terminó, crea un tanque enemigo nuevo
        if(!enemigo_vivo && t_animacion==0){
            ia=crear_tanque_enemigo(fp->pos[0], fp->pos[1], obstaculos);
            if(ia==NULL){
                break;
            }
            enemigo_vivo=true;
        }

        //Si el tanque enemigo está vivo y puede disparar, crea el misil enemigo
        if(enemigo_vivo && ia_t_misil==0 && en_rango_vision_turr(ia, fp, -0.1, 0.1) && !ia_misil_activo){
            ia_misil_activo=true;
            ia_t_misil=COOLDOWN*JUEGO_FPS;
            misil_ia=malloc(sizeof(cuerpo_t));
            if(misil_ia==NULL){
                break;
            }
            for(size_t i=0; i<2; i++){
                misil_ia->pos[i]=ia->pos[i];
            }
            misil_ia->pos[2]=3;
            misil_ia->angz=ia->angz+ia->ang_torreta;
            misil_ia->bloque=MISIL; 
        }

        /*Movimientos*/

        //Moviemientos de los misiles (si los hay)
        //Misil del jugador
        if(misil_activo){
            bool kill=false;
            if(!misil_mover(misil, obstaculos, ia, &kill)){
                misil_activo=false;
                free(misil);
                misil=NULL;
            }
            if(kill){
                if(enemigo_vivo){
                    t_animacion=T_ANIM*JUEGO_FPS;
                    enemigo_vivo=false;
                    ia_accion=IA_NONE;
                    ia_t_accion=0;
                    score+=SCORE_INC;
                }
            }
        }
        if(t_misil!=0){
            t_misil--;
            if(t_misil==0){
                free(misil);
                misil=NULL;
                misil_activo=false;
            }
        }

        //Misil enemigo
        if(ia_misil_activo){
            bool kill=false;
            if(!misil_mover(misil_ia, obstaculos, fp, &kill)){
                ia_misil_activo=false;
                free(misil_ia);
                misil_ia=NULL;
            }
            if(kill){
                vidas--;
                t_muerte=T_MUERTE;
            }
        }
        if(ia_t_misil!=0){
            ia_t_misil--;
            if(ia_t_misil==0){
                free(misil_ia);
                misil_ia=NULL;
                ia_misil_activo=false;
            }
        }

        //Movimientos del jugador
        if(t_mov!=0){
            tanque_mover(fp, mov, obstaculos, ia);
            t_mov--;
            if(t_mov==0){
                mov=TRAS_NONE;
            }
        }
        if(t_rot!=0){
            tanque_rotar(fp, rot);
            t_rot--;
            if(t_rot==0){
                rot=ROT_NONE;
            }
        }    

        //Movimientos del enemigo
        if(enemigo_vivo){
            tanque_radar(ia);
            decidir_accion_ia(&ia_accion, &ia_t_accion, ia, fp); //Decide la acción
            ia_acciones(ia, ia_accion, obstaculos, fp); //Ejecuta la acción
            if(ia_t_accion!=0){
                ia_t_accion--;
                if(ia_t_accion==0){
                    ia_accion=IA_NONE;
                }
            }
            en_rango_vision_ia(ia, fp, 1, &turr); //Determina el estado de la torreta
            tanque_rotar_torreta(ia, turr); //Rota la torreta acorde al estado determinado
        }

        /*Impresión 3D*/

        //Apila la transformación de cuadro
        if(!apilar_cuadro_transformacion(t_mov, t_rot, fp, transformacion)){
            break;
        }

        //Imprimir elementos del mundo: Horizonte, montañas y luna
        if(!mundo_imprimir(modelos, transformacion, pantalla, renderer)){
            break;
        }

        if(t_animacion!=0){ //Si se destruyó el enemigo calcula y dibuja la aninación de destrucción
            if(!animacion_destruccion(ia->pos, t_animacion, modelos, transformacion, pantalla, renderer)){
                break;    
            }
            t_animacion--;
            if(t_animacion==0){
                free(ia);
                ia=NULL;
            }
        }else{ //Si el enemigo está vivo lo dibuja
            if(!tanque_imprimir(ia, modelos, transformacion, pantalla, renderer)){
                break;
            }
        }

        //Si los misiles están activos, los dibuja        
        if(misil_activo){
            if(t_misil==0){
                misil_activo=false;
            }        
            if(!cuerpo_imprimir(misil, modelos, transformacion, pantalla, renderer)){
                break;
            }
        }

        if(ia_misil_activo){
            if(ia_t_misil==0){
                ia_misil_activo=false;
            }
            if(!cuerpo_imprimir(misil_ia, modelos, transformacion, pantalla, renderer)){
                break;
            }
        }

        //Imprime obstáculos
        if(!imprimir_obstaculos(obstaculos, modelos, transformacion, pantalla, renderer)){
            break;
        }

        //Desapila la transformación de cuadro
        desapilar_cuadro_transformacion(transformacion);

        /*Impresión 2D*/

        scope='-';

        //Determina la posición relativa del enemigo
        if(en_rango_vision_sim(fp, ia, 1)){
            enemy_pos=FRONT;
            if(en_rango_vision_sim(fp, ia, 0.15)){
                scope='+';
            }
        }else if(_en_rango_vision(fp, ia, 1, 2.44)){
            enemy_pos=LEFT;
        }else if(_en_rango_vision(fp, ia, -2.44, -1)){
            enemy_pos=RIGHT;
        }else{
            enemy_pos=BACK;
        }

        //Impresión de HUD
        if(!imprimir_hud(vidas, score, enemy_pos, scope, modelos, renderer)){
            break;
        }

        //Animación de muerte del jugador
        if(t_muerte!=0){
            float escala=50; //Tune
            if(!animacion_muerte(t_muerte, escala, vidas, modelos, renderer)){
                break;
            }
            t_muerte--;
        }

        /*Fin de ciclo de cuadro*/
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
    /*Fin de ejecución*/

    //Borrar cuerpos
    free(fp);
    free(ia);
    free(misil);
    free(misil_ia);

    //Destruir lista de modelos, lista de obstáculos y pila de transformaciones
    lista_destruir(modelos, destruir_modelo);
    lista_destruir(obstaculos, free);
    pila_destruir(transformacion, matriz_destruir);

    //Borrar matriz de pantalla
    matriz_destruir(pantalla);

    /*Fin de programa*/
    // END código del alumno

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}
