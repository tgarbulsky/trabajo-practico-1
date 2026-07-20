#ifndef TANQUE_H
#define TANQUE_H

#include "mundo.h"
#include "lista.h"

// funciones de creacion de tanques

// crea el tanque en la posicion x,y con un angulo angz
tanque_t* crear_tanque(float x, float y, float angz);
tanque_t* crear_tanque_enemigo(float x_fp, float y_fp, lista_t* obstaculos);

// primitivas de movimiento del tanque
void tanque_mover(tanque_t* tanque, enum tras tras, lista_t* obstaculos, tanque_t* otro_tanque);
void tanque_rotar(tanque_t* tanque, enum rot rot);
void tanque_radar(tanque_t* tanque);
void tanque_rotar_torreta(tanque_t* tanque, enum turr turr);

// primitivas de visión del tanque
bool _en_rango_vision(tanque_t* pov, tanque_t* obj, float low, float high);
bool en_rango_vision_sim(tanque_t* pov, tanque_t* obj, float dang);
bool en_rango_vision_ia(tanque_t* pov, tanque_t* obj, float dang, enum turr* turr);
bool en_rango_vision_turr(tanque_t* pov, tanque_t* obj, float low, float high);

// funciones de decisión de la ia
void decidir_accion_ia(enum acciones_ia* accion, int* t_accion_ia, tanque_t* ia, tanque_t* fp);
void ia_acciones(tanque_t* ia, enum acciones_ia accion, lista_t* obstaculos, tanque_t* otro_tanque);

#endif
