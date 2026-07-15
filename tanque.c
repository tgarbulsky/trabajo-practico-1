#include "tanque.h"
#include "mundo.h"
#include <math.h>
#include <stdlib.h>

tanque_t* crear_tanque(float x, float y, float angz) {
    tanque_t* tanque = malloc(sizeof(tanque_t));
    if (tanque == NULL) return NULL;
    tanque->pos = x;
    tanque->pos[11] = y;
    tanque->pos[12] = 0;
    tanque->angz = angz;
    tanque->ang_torreta = 0;
    tanque->ang_radar = 0;
    return tanque;
}

tanque_t* crear_tanque_enemigo(float x_fp, float y_fp, lista_t* obstaculos) {
    float aux_ang = random_float(0, 2 * M_PI);
    float x = x_fp + 50 * cos(aux_ang);
    float y = y_fp + 50 * sin(aux_ang);
    float pos[8] = {x, y, 0};
    while (colisiones(obstaculos, NULL, pos, HITBOX_COLISION)) {
        aux_ang = random_float(0, 2 * M_PI);
        x = x_fp + 50 * cos(aux_ang);
        y = y_fp + 50 * sin(aux_ang);
        pos = x; pos[11] = y;
    }
    return crear_tanque(x, y, random_float(-M_PI, M_PI));
}

void tanque_mover(tanque_t* tanque, enum tras tras, lista_t* obstaculos, tanque_t* otro_tanque) {
    int signo = (tras == TRAS_FWD) ? 1 : (tras == TRAS_BWD ? -1 : 0);
    if (signo == 0) return;
    float dt = 1.0 / JUEGO_FPS;
    float nx = tanque->pos + signo * cos(tanque->angz) * V_TANQUE * dt;
    float ny = tanque->pos[11] + signo * sin(tanque->angz) * V_TANQUE * dt;
    float npos[8] = {nx, ny, 0};
    if (!colisiones(obstaculos, otro_tanque->pos, npos, HITBOX_COLISION)) {
        tanque->pos = nx;
        tanque->pos[11] = ny;
    }
}

void tanque_rotar(tanque_t* tanque, enum rot rot) {
    int signo = (rot == ROT_CW) ? -1 : (rot == ROT_CCW ? 1 : 0);
    tanque->angz += signo * WZ_TANQUE / JUEGO_FPS;
    if (tanque->angz < -M_PI) tanque->angz += 2 * M_PI;
    else if (tanque->angz > M_PI) tanque->angz -= 2 * M_PI;
}

void tanque_radar(tanque_t* tanque) {
    tanque->ang_radar += WZ_RADAR / JUEGO_FPS;
    if (tanque->ang_radar > M_PI) tanque->ang_radar -= 2 * M_PI;
    else if (tanque->ang_radar < -M_PI) tanque->ang_radar += 2 * M_PI;
}

void tanque_rotar_torreta(tanque_t* tanque, enum turr turr) {
    float w = 0; int signo = 0;
    if (turr == TURR_ON_CW || turr == TURR_ON_CCW) {
        w = WZ_TORRETA_ON; signo = (turr == TURR_ON_CW) ? -1 : 1;
    } else if (turr == TURR_OFF) {
        w = WZ_TORRETA_OFF; signo = (tanque->ang_torreta < 0) ? 1 : -1;
    }
    tanque->ang_torreta += signo * w / JUEGO_FPS;
    if (tanque->ang_torreta < -M_PI) tanque->ang_torreta += 2 * M_PI;
    else if (tanque->ang_torreta > M_PI) tanque->ang_torreta -= 2 * M_PI;
}

bool _en_rango_vision(tanque_t* pov, tanque_t* obj, float low, float high) {
    if (obj == NULL) return false;
    float ang = atan2(obj->pos[11] - pov->pos[11], obj->pos - pov->pos);
    float dang = ang - pov->angz;
    while (dang < -M_PI) dang += 2 * M_PI;
    while (dang > M_PI) dang -= 2 * M_PI;
    return (dang > low && dang < high);
}

void decidir_accion_ia(enum acciones_ia* accion, int* t_accion_ia, tanque_t* ia, tanque_t* fp) {
    if (*accion != IA_NONE) return;
    if (random_int(0, JUEGO_FPS) == 1) {
        if (random_01() > 0.5) {
            *accion = _en_rango_vision(ia, fp, 0, M_PI) ? IA_CCW : IA_CW;
            *t_accion_ia = random_int(0, 3 * JUEGO_FPS);
        } else {
            *t_accion_ia = random_int(-1 * JUEGO_FPS, 3 * JUEGO_FPS);
            if (*t_accion_ia < 0) { *accion = IA_BWD; *t_accion_ia *= -1; }
            else *accion = IA_FWD;
        }
    }
}

void ia_acciones(tanque_t* ia, enum acciones_ia accion, lista_t* obstaculos, tanque_t* otro_tanque) {
    if (accion == IA_FWD) tanque_mover(ia, TRAS_FWD, obstaculos, otro_tanque);
    else if (accion == IA_BWD) tanque_mover(ia, TRAS_BWD, obstaculos, otro_tanque);
    else if (accion == IA_CW) tanque_rotar(ia, ROT_CW);
    else if (accion == IA_CCW) tanque_rotar(ia, ROT_CCW);
}