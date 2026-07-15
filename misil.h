#ifndef MISIL_H
#define MISIL_H

#include <stdbool.h>
#include "mundo.h"
#include "tanque.h"
#include "lista.h"

// Mueve el misil según su ángulo. 
// Devuelve false si impacta contra un obstáculo o tanque.
// Si impacta al tanque enemigo, pone kill en true.
bool misil_mover(cuerpo_t* misil, lista_t* obstaculos, tanque_t* otro_tanque, bool* kill);

#endif