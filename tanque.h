#ifndef TANQUE_H
#define TANQUE_H

#include "mundo.h"
#include "lista.h"

// funciones de creacion de tanques

// crea el tanque en la posicion x,y con un angulo angz
tanque_t* crear_tanque(float x, float y, float angz);

//crea el tanque enemigo en funcion de la posicion del tanque del usuario
tanque_t* crear_tanque_enemigo(float x_fp, float y_fp, lista_t* obstaculos);

// primitivas de movimiento del tanque

//mueve el tanque en la direccion del angulo z con velocidad constante (V_TANQUE) definida en mundo.h 
void tanque_mover(tanque_t* tanque, enum tras tras, lista_t* obstaculos, tanque_t* otro_tanque);

//rota el angulo z del tanque con velocidad constante (WZ_TANQUE) definida en mundo.h
void tanque_rotar(tanque_t* tanque, enum rot rot);

//rota el radar del tanque con velocidad constante (WZ_RADAR) definida en mundo.h
//mantiene el angulo entre menos pi y pi. 
void tanque_radar(tanque_t* tanque);

//rota la torreta del tanque
void tanque_rotar_torreta(tanque_t* tanque, enum turr turr);

// primitivas de visión del tanque

//indica si el tanque enemigo esta en un rango de vision entre low y high del tanque
bool _en_rango_vision(tanque_t* pov, tanque_t* obj, float low, float high);

//analiza caso borde de low=high
//rango de vision pov del tanque definido por dang
bool en_rango_vision_sim(tanque_t* pov, tanque_t* obj, float dang);

//chequea vision para un rango simetrico de amplitud dang de vision del tanque enemigo
bool en_rango_vision_ia(tanque_t* pov, tanque_t* obj, float dang, enum turr* turr);

//vision respecto a la torreta
//analiza vision en rango low, high respecto de la torreta de pov
bool en_rango_vision_turr(tanque_t* pov, tanque_t* obj, float low, float high);

// funciones de decisión de la ia

//implementacion software de aspiradora
//decide de forma aleatoria la decision del tanque enemigo
void decidir_accion_ia(enum acciones_ia* accion, int* t_accion_ia, tanque_t* ia, tanque_t* fp);

//ejecuta efectivamente la accion del tanque enemigo
void ia_acciones(tanque_t* ia, enum acciones_ia accion, lista_t* obstaculos, tanque_t* otro_tanque);

#endif
