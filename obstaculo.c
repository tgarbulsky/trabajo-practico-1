#include "obstaculo.h"

// generador de obstaculos
cuerpo_t* crear_obstaculo(){
    cuerpo_t* obstaculo=malloc(sizeof(cuerpo_t));
    if(obstaculo==NULL){
        return NULL;
    }
    obstaculo->bloque=random_int(CUBO1, PIRAMIDE3);
    obstaculo->posicion[0]=random_float(-150, 150);
    obstaculo->posicion[1]=random_float(-150, 150);
    obstaculo->posicion[2]=0;
    obstaculo->angz=random_float(-M_PI, M_PI);
    return obstaculo;
}
