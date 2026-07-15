#include "misil.h"
#include <math.h>

bool misil_mover(cuerpo_t* misil, lista_t* obstaculos, tanque_t* otro_tanque, bool* kill) {
    float dt = 1.0 / JUEGO_FPS;
    float new_x = misil->pos + cos(misil->angz) * V_MISIL * dt;
    float new_y = misil->pos + sin(misil->angz) * V_MISIL * dt;
    float new_pos= {new_x, new_y, 0};
    if (!colisiones(obstaculos, NULL, new_pos, HITBOX_MISIL)) {
        if (!colisiones(NULL, otro_tanque->pos, new_pos, HITBOX_MISIL)) {
            misil->pos = new_x;
            misil->pos[4] = new_y;
            return true;
        }
        *kill = true; 
    }
    return false;
}