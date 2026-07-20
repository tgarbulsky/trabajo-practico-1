#include "interfaz2d.h"
#include "animaciones.h"

bool imprimir_caracter(char c, float escala, float xy[2], unsigned char color[3], lista_t* modelos, SDL_Renderer* renderer){
    float factor[4]={escala, -1*escala, 1, 1};
    char s[2]={c, '\0'};
    float pos[3]={xy[0], xy[1], 0};
    matriz_t* esc=matriz_crear_escalar(4, factor);
    if(esc==NULL){
        return false;
    }
    modelo_t* modelo=buscar_modelo(s, modelos);
    if(modelo==NULL){
        matriz_destruir(esc);
        return false;
    }
    matriz_t* extendida=matriz_extender(modelo_obtener_coords(modelo));
    if(extendida==NULL){
        matriz_destruir(esc);
        return false;
    }
    matriz_t* escalada=matriz_multiplicar(esc, extendida);
    matriz_destruir(esc);
    matriz_destruir(extendida);
    if(escalada==NULL){
        return false;
    }
    matriz_t* tras=matriz_crear_tras(pos);
    if(tras==NULL){
        matriz_destruir(escalada);
        return false;
    }
    matriz_t* app=matriz_multiplicar(tras, escalada);
    matriz_destruir(tras);
    matriz_destruir(escalada);
    if(app==NULL){
        return false;
    }
    size_t nlineas=modelo_obtener_nlineas(modelo);
    for(size_t i=0; i<nlineas; i++){
        size_t coord1, coord2;
        modelo_obtener_linea(modelo, i, &coord1, &coord2);
        dibujar_linea(app, coord1, coord2, color, renderer);
    }
    matriz_destruir(app);
    return true;
}

bool imprimir_cadena(const char* s, float escala, float xy[2], float incx, unsigned char color[3], lista_t* modelos, SDL_Renderer* renderer){
    size_t i=0;
    for(i=0; s[i]!='\0'; i++){
        xy[0]+=incx;
        if(!imprimir_caracter(s[i], escala, xy, color, modelos, renderer)){
            return false;
        }
    }
    xy[0]-=i*incx;
    return true;
}

bool imprimir_hud(char vidas, unsigned long score, enum enemy_to enemy_pos, char scope, lista_t* modelos, SDL_Renderer* renderer){
    float escala=25;
    float incx=25;
    float incy=50;
    float xy[2]={VENTANA_ANCHO-5*incx, 2*incy};
    unsigned char color[3]={255, 0, 0};
    size_t i=0;
    if(vidas>0){
        for(i=0; i<vidas-1; i++){
            xy[0]-=2*incx;
            if(!imprimir_caracter('*', escala, xy, color, modelos, renderer)){
                return false;
            }
        }
        xy[0]+=2*i*incx;
    }
    xy[1]+=incy;
    xy[0]=VENTANA_ANCHO-9*incx;
    char num[20];
    snprintf(num, 20, "%lu", score);
    if(!imprimir_cadena(num, escala, xy, incx, color, modelos, renderer)){
        return false;
    }
    xy[0]=VENTANA_ANCHO-15*incx;
    if(!imprimir_cadena("SCORE ", escala, xy, incx, color, modelos, renderer)){
        return false;
    }
    xy[0]=5*incx;
    xy[1]-=incy;
    if(!imprimir_cadena("ENEMY TO ", escala, xy, incx, color, modelos, renderer)){
        return false;
    }
    xy[0]+=9*incx;
    if(!imprimir_cadena(enemy_rel_pos[enemy_pos], escala, xy, incx, color, modelos, renderer)){
        return false;
    }
    xy[0]=VENTANA_ANCHO/2;
    xy[1]=VENTANA_ALTO/2;
    escala=25;
    if(!imprimir_caracter(scope, escala, xy, color, modelos, renderer)){
        return false;
    }
    return true;
}

bool animacion_muerte(int t_muerte, float escala, char vidas, lista_t* modelos, SDL_Renderer* renderer){
    size_t n=T_MUERTE-t_muerte+1;
    unsigned char color[3]={255, 0, 0};
    float factor[4]={escala, -1*escala, 1, 1};
    float pos[3]={VENTANA_ANCHO/2, VENTANA_ALTO/2, 0};
    matriz_t* esc=matriz_crear_escalar(4, factor);
    if(esc==NULL){
        return false;
    }
    modelo_t* modelo=buscar_modelo("#", modelos);
    if(modelo==NULL){
        matriz_destruir(esc);
        return false;
    }
    matriz_t* extendida=matriz_extender(modelo_obtener_coords(modelo));
    if(extendida==NULL){
        matriz_destruir(esc);
        return false;
    }
    matriz_t* escalada=matriz_multiplicar(esc, extendida);
    matriz_destruir(esc);
    matriz_destruir(extendida);
    if(escalada==NULL){
        return false;
    }
    matriz_t* tras=matriz_crear_tras(pos);
    if(tras==NULL){
        matriz_destruir(escalada);
        return false;
    }
    matriz_t* app=matriz_multiplicar(tras, escalada);
    matriz_destruir(tras);
    matriz_destruir(escalada);
    if(app==NULL){
        return false;
    }
    size_t nlineas=modelo_obtener_nlineas(modelo);
    for(size_t i=0; i<nlineas && i<n; i++){
        size_t coord1, coord2;
        modelo_obtener_linea(modelo, i, &coord1, &coord2);
        dibujar_linea(app, coord1, coord2, color, renderer);
    }
    matriz_destruir(app);
    if(vidas==0){
        float escala_cadena = 25;
        float incx_cadena = 25;
        float xy_cadena[2]={VENTANA_ANCHO/2 - 5.5*incx_cadena, VENTANA_ALTO/2 + incx_cadena/2};
        if(!imprimir_cadena("GAME OVER", escala_cadena, xy_cadena, incx_cadena, color, modelos, renderer)){
            return false;
        }
    }
    return true;
}
