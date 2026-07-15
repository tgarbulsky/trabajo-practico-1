#include "misil.h"
#include <math.h>

bool misil_mover(cuerpo_t* misil, lista_t* obstaculos, tanque_t* otro_tanque, bool* kill) {
    float dt = 1.0 / JUEGO_FPS;
    float new_x = misil->pos + cos(misil->angz) * V_MISIL * dt;
    float new_y = misil->pos[4] + sin(misil->angz) * V_MISIL * dt;
    float new_pos[3] = {new_x, new_y, 0};

    // Primero verificamos colisión contra obstáculos (radio 3m)
    if (!colisiones(obstaculos, NULL, new_pos, HITBOX_MISIL)) {
        // Luego verificamos colisión contra el tanque enemigo/jugador (radio 3m)
        if (!colisiones(NULL, otro_tanque->pos, new_pos, HITBOX_MISIL)) {
            misil->pos = new_x;
            misil->pos[4] = new_y;
            return true; // Sigue viajando
        }
        *kill = true; // Impacto directo al tanque
    }
    return false; // El misil se destruye por impacto
}
