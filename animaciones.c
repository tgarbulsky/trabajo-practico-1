#include "animaciones.h"

const float g = -9.81;
const float pos_rel_torreta[3]={0, 0, 3};
const float pos_rel_radar[3]={-1.5, 0, 0.5};

const char* etiquetas[] = { 
	[TANQUE]="TANQUE",
	[TORRETA]="TORRETA",
	[RADAR]="RADAR",
    [MISIL]="MISIL",
	[CUBO1]="CUBO1",
	[CUBO2]="CUBO2",
	[CUBO3]="CUBO3",
	[PIRAMIDE1]="PIRAMIDE1",
	[PIRAMIDE2]="PIRAMIDE2",
    [PIRAMIDE3]="PIRAMIDE3",
	[HORIZONTE]="HORIZONTE",
	[MONTANA]="MONTANA",
	[LUNA]="LUNA",
	[RESTO1]="RESTO1",
	[RESTO2]="RESTO2",
};

const unsigned char colores[][3] = {
    [TANQUE]={255, 255, 255},
	[TORRETA]={255, 255, 255},
	[RADAR]={255, 255, 255},
    [MISIL]={255, 0, 0},
	[CUBO1]={0, 255, 0},
	[CUBO2]={0, 255, 0},
	[CUBO3]={0, 255, 0},
	[PIRAMIDE1]={0, 255, 0},
	[PIRAMIDE2]={0, 255, 0},
    [PIRAMIDE3]={0, 255, 0},
	[HORIZONTE]={255, 255, 255},
	[MONTANA]={255, 255, 255},
	[LUNA]={255, 255, 255},
	[RESTO1]={255, 255, 255},
	[RESTO2]={255, 255, 255},
};

const char* enemy_rel_pos[]={
    [FRONT]="FRONT",
    [BACK]="REAR",
    [LEFT]="LEFT",
    [RIGHT]="RIGHT",
};

matriz_t* matriz_crear_pantalla(unsigned int altura, unsigned int ancho){
    matriz_t* pantalla=_matriz_crear(4, 4);
    if(pantalla==NULL){
        return NULL;
    }
    set_id_mx(pantalla);
    matriz_establecer(pantalla, 1, 1, altura/2.0); //escala
    matriz_establecer(pantalla, 2, 2, -(altura/2.0)); //escala y reflexion
    matriz_establecer(pantalla, 2, 4, altura/2.0); //offset
    matriz_establecer(pantalla, 1, 4, ancho/2.0); //offset
    return pantalla;
}

bool apilar_transformacion(pila_t* transformacion, matriz_t* matriz){
    if(pila_ver_tope(transformacion)==NULL){
        if(!pila_apilar(transformacion, matriz)){
            matriz_destruir(matriz);
            return false;
        }
        return true;
    }
    matriz_t* nuevo_top=matriz_multiplicar(pila_ver_tope(transformacion), matriz);
    if(nuevo_top==NULL){
        matriz_destruir(matriz);
        return false;
    }
    matriz_destruir(matriz);
    if(!pila_apilar(transformacion, nuevo_top)){
        matriz_destruir(nuevo_top);
        return false;
    }
    return true;
}

void desapilar_transformacion(pila_t* transformacion){
    matriz_destruir(pila_desapilar(transformacion));
}

bool apilar_rototraslacion(pila_t* transformacion, const float v[3], const float angz){
    matriz_t* tras=matriz_crear_tras(v);
    if(tras==NULL){
        return false;
    }
    if(!apilar_transformacion(transformacion, tras)){
        return false;
    }
    matriz_t* rotz=matriz_crear_rotz(angz);
    if(rotz==NULL || !apilar_transformacion(transformacion, rotz)){
        desapilar_transformacion(transformacion);
        return false;
    }
    return true;
}

void desapilar_rototraslacion(pila_t* transformacion){
    for(size_t i=0; i<2; i++){
        desapilar_transformacion(transformacion);
    }
}

bool apilar_cuadro_transformacion(int t_mov, int t_rot, tanque_t* tanque, pila_t* transformacion){
    float aux_ang[3]={
        0,
        M_PI/2.0,
        M_PI/2.0,
    };
    if(t_mov!=0){
        aux_ang[1]-=random_float(0, 0.01);
    }
    if(t_rot!=0){
        aux_ang[2]+=random_float(0, 0.01);
    }
    matriz_t* rotz=matriz_crear_rotz(aux_ang[2]);
    if(rotz==NULL){
        return false;
    }
    matriz_t* roty=matriz_crear_roty(aux_ang[1]);
    if(roty==NULL){
        matriz_destruir(rotz);
        return false;
    }
    if(!apilar_transformacion(transformacion, rotz)){
        matriz_destruir(roty);
        return false;
    }
    if(!apilar_transformacion(transformacion, roty)){
        desapilar_transformacion(transformacion);
        return false;
    }
    matriz_t* tanque_ang=matriz_crear_rotz(-1*tanque->angz);
    if(tanque_ang==NULL || !apilar_transformacion(transformacion, tanque_ang)){
        desapilar_transformacion(transformacion);
        desapilar_transformacion(transformacion);
        return false;
    }
    float pos[3]={
        -1*tanque->pos[0],
        -1*tanque->pos[1],
        -3,
    };
    matriz_t* tanque_pos=matriz_crear_tras(pos);
    if(tanque_pos==NULL || !apilar_transformacion(transformacion, tanque_pos)){
        desapilar_transformacion(transformacion);
        desapilar_transformacion(transformacion);
        desapilar_transformacion(transformacion);
        return false;
    }
    return true;
}

void desapilar_cuadro_transformacion(pila_t* transformacion){
    desapilar_transformacion(transformacion);
    desapilar_transformacion(transformacion);
    desapilar_transformacion(transformacion);
    desapilar_transformacion(transformacion);
}

modelo_t* buscar_modelo(const char* etiqueta, lista_t* modelos){
    lista_iter_t* iterador=lista_iter_crear(modelos);
    while(!lista_iter_al_final(iterador)){
        modelo_t* modelo=lista_iter_ver_actual(iterador);
        if(!strcmp(modelo_obtener_etiqueta(modelo), etiqueta)){
            lista_iter_destruir(iterador);
            return modelo;
        }
        lista_iter_avanzar(iterador);
    }
    lista_iter_destruir(iterador);
    return NULL;
}

modelo_t* buscar_bloque(bloque_t bloque, lista_t* modelos){
    return buscar_modelo(etiquetas[bloque], modelos);
}

void dibujar_linea(matriz_t* m, size_t coord1, size_t coord2, const unsigned char color[3], SDL_Renderer* renderer){
    SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], 0x00);
    SDL_RenderDrawLine(renderer, matriz_obtener(m, 1, coord1+1), matriz_obtener(m, 2, coord1+1), matriz_obtener(m, 1, coord2+1), matriz_obtener(m, 2, coord2+1));
}

void dibujar_linea_3d(matriz_t* m, size_t coord1, size_t coord2, const unsigned char color[3], SDL_Renderer* renderer){
    if(matriz_obtener(m, 3, coord1+1)<1 || matriz_obtener(m, 3, coord2+1)<1){
        return;
    }
    dibujar_linea(m, coord1, coord2, color, renderer);
}

bool bloque_imprimir(bloque_t bloque, modelo_t* modelo, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer){
    matriz_t* app=matriz_aplicar(pila_ver_tope(transformacion), modelo_obtener_coords(modelo));
    if(app==NULL){
        return false;
    }
    matriz_t* print=matriz_multiplicar(pantalla, app);
    matriz_destruir(app);
    if(print==NULL){
        return false;
    }
    size_t nlineas=modelo_obtener_nlineas(modelo);
    for(size_t i=0; i<nlineas; i++){
        size_t coord1, coord2;
        modelo_obtener_linea(modelo, i, &coord1, &coord2);
        dibujar_linea_3d(print, coord1, coord2, colores[bloque], renderer);
    }
    matriz_destruir(print);
    return true;
}

bool cuerpo_imprimir(cuerpo_t* cuerpo, lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer){
    if(!apilar_rototraslacion(transformacion, cuerpo->pos, cuerpo->angz)){
        return false;
    }
    modelo_t* modelo=buscar_bloque(cuerpo->bloque, modelos);
    if(modelo==NULL){
        desapilar_rototraslacion(transformacion);
        return false;
    }
    if(!bloque_imprimir(cuerpo->bloque, modelo, transformacion, pantalla, renderer)){
        desapilar_rototraslacion(transformacion);
        return false;
    }
    desapilar_rototraslacion(transformacion);
    return true;
}

bool imprimir_obstaculos(lista_t* obstaculos, lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer){
    lista_iter_t* iterador=lista_iter_crear(obstaculos);
    while(!lista_iter_al_final(iterador)){
        if(!cuerpo_imprimir(lista_iter_ver_actual(iterador), modelos, transformacion, pantalla, renderer)){
            lista_iter_destruir(iterador);
            return false;
        }
        lista_iter_avanzar(iterador);
    }
    lista_iter_destruir(iterador);
    return true;
}

bool tanque_imprimir(tanque_t* tanque, lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer){
    if(!apilar_rototraslacion(transformacion, tanque->pos, tanque->angz)){
        return false;
    }
    modelo_t* modelo=buscar_bloque(TANQUE, modelos);
    if(modelo==NULL){
        desapilar_rototraslacion(transformacion);
        return false;
    }
    if(!bloque_imprimir(TANQUE, modelo, transformacion, pantalla, renderer)){
        desapilar_rototraslacion(transformacion);
        return false;
    }
    modelo=buscar_bloque(TORRETA, modelos);
    if(modelo==NULL){
        desapilar_rototraslacion(transformacion);
        return false;
    }
    if(!apilar_rototraslacion(transformacion, pos_rel_torreta, tanque->ang_torreta)){
        desapilar_rototraslacion(transformacion);
        return false;
    }
    if(!bloque_imprimir(TORRETA, modelo, transformacion, pantalla, renderer)){
        desapilar_rototraslacion(transformacion);
        desapilar_rototraslacion(transformacion);
        return false;
    }
    modelo=buscar_bloque(RADAR, modelos);
    if(modelo==NULL){
        desapilar_rototraslacion(transformacion);
        desapilar_rototraslacion(transformacion);
        return false;
    }
    if(!apilar_rototraslacion(transformacion, pos_rel_radar, tanque->ang_radar)){
        desapilar_rototraslacion(transformacion);
        desapilar_rototraslacion(transformacion);
        return false;
    }
    if(!bloque_imprimir(RADAR, modelo, transformacion, pantalla, renderer)){
        desapilar_rototraslacion(transformacion);
        desapilar_rototraslacion(transformacion);
        desapilar_rototraslacion(transformacion);
        return false;
    }
    desapilar_rototraslacion(transformacion);
    desapilar_rototraslacion(transformacion);
    desapilar_rototraslacion(transformacion);
    return true;
}

bool mundo_imprimir(lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer){
    modelo_t* modelo=buscar_modelo(etiquetas[HORIZONTE], modelos);
    if(modelo==NULL || !bloque_imprimir(HORIZONTE, modelo, transformacion, pantalla, renderer)){
        return false;
    }
    modelo=buscar_modelo(etiquetas[MONTANA], modelos);
    if(modelo==NULL || !bloque_imprimir(MONTANA, modelo, transformacion, pantalla, renderer)){
        return false;
    }
    modelo=buscar_modelo(etiquetas[LUNA], modelos);
    if(modelo==NULL || !bloque_imprimir(LUNA, modelo, transformacion, pantalla, renderer)){
        return false;
    }
    return true;
}

bool animacion_destruccion(float pos[3], int t_animacion, lista_t* modelos, pila_t* transformacion, matriz_t* pantalla, SDL_Renderer* renderer){
    float t=(T_ANIM*JUEGO_FPS-t_animacion)/JUEGO_FPS;
    float tiro_oblicuo[3]={t*V0X, 0, 3+t*V0Z+0.5*g*t*t};
    
    // velocidad angular de rotacion para las piezas voladoras
    float velocidad_angular = 5.0f;
    float angulo_rotacion = t * velocidad_angular;

    bloque_t bloques[6]={RESTO1, RESTO2, TORRETA, RESTO1, RESTO2, RADAR};
    matriz_t* tras_pos=matriz_crear_tras(pos);
    if(tras_pos==NULL || !apilar_transformacion(transformacion, tras_pos)){
        return false;
    }
    for(size_t i=0; i<6; i++){
        modelo_t* modelo=buscar_modelo(etiquetas[bloques[i]], modelos);
        if(modelo==NULL){
            desapilar_transformacion(transformacion);
            return false;
        }
        
        // Rotacion radial de la explosion
        matriz_t* rotz=matriz_crear_rotz(i*M_PI/3);
        if(rotz==NULL || !apilar_transformacion(transformacion, rotz)){
            desapilar_transformacion(transformacion);
            return false;
        }
        
        //traslacion del tiro oblicuo
        matriz_t* tras=matriz_crear_tras(tiro_oblicuo);
        if(tras==NULL || !apilar_transformacion(transformacion, tras)){
            desapilar_transformacion(transformacion);
            return false;
        }

        //rotacion sobre su propio eje en el aire
        matriz_t* rot_propia = (i % 2 == 0) ? matriz_crear_rotz(angulo_rotacion) : matriz_crear_roty(angulo_rotacion);
        if(rot_propia==NULL || !apilar_transformacion(transformacion, rot_propia)){
            desapilar_transformacion(transformacion); // saca tras
            desapilar_transformacion(transformacion); // saca rotz
            desapilar_transformacion(transformacion); // saca tras_pos
            return false;
        }

        //imprimir el bloque con las transformaciones combinadas
        if (!bloque_imprimir(bloques[i], modelo, transformacion, pantalla, renderer)){
            desapilar_transformacion(transformacion); // saca rot_propia
            desapilar_transformacion(transformacion); // saca tras
            desapilar_transformacion(transformacion); // saca rotz
            desapilar_transformacion(transformacion); // saca tras_pos
            return false;
        }
        
        desapilar_transformacion(transformacion); // libera rot_propia
        desapilar_transformacion(transformacion); // libera tras
        desapilar_transformacion(transformacion); // libera rotz
    }
    desapilar_transformacion(transformacion); // libera tras_pos
    return true;
}
