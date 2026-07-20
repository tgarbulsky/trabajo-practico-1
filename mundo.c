#include "mundo.h"

float random_entre01(){
    return rand()/(RAND_MAX+1.0);
}

float random_float(float a, float b){
    return random_entre01()*(b-a) + a;
}

int random_int(int a, int b){
    return rand() % (b-a+1) + a;
}

float norma_r3(float v1[3], float v2[3]){
    float dx=v2[0]-v1[0];
    float dy=v2[1]-v1[1];
    float dz=v2[2]-v1[2];
    return pow(dx*dx+dy*dy+dz*dz, 0.5);
}

bool colisiones(lista_t* obstaculos, float otros_colisionables[3], float posicion[3], float hitbox){
    if(otros_colisionables!=NULL){
        if(norma_r3(posicion,otros_colisionables)<hitbox){
            return true;
        }
    }
    lista_iter_t* iterador=lista_iter_crear(obstaculos);
    while(!lista_iter_al_final(iterador)){
        cuerpo_t* obstaculo=lista_iter_ver_actual(iterador);
        if(norma_r3(posicion, obstaculo->posicion)<hitbox){
            lista_iter_destruir(iterador);
            return true;
        }
        lista_iter_avanzar(iterador);
    }
    lista_iter_destruir(iterador);
    return false;
}
