#include "misil.h"

bool misil_mover(cuerpo_t* misil, lista_t* obstaculos, tanque_t* otro_tanque, bool* kill){
    float new_x=misil->posicion[0]+cos(misil->angz)*V_MISIL*1.0/JUEGO_FPS;
    float new_y=misil->posicion[1]+sin(misil->angz)*V_MISIL*1.0/JUEGO_FPS;
    float new_posicion[3]={new_x, new_y, 0};
    if(!colisiones(obstaculos, NULL, new_posicion, HITBOX_MISIL)){
        if(!colisiones(obstaculos, otro_tanque->posicion, new_posicion, HITBOX_MISIL)){
            misil->posicion[0] = new_x;
            misil->posicion[1] = new_y;
            return true;    
        }
        *kill=true;
    }
    return false;
}
