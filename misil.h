#ifndef MISIL_H
#define MISIL_H

#include "mundo.h"
#include "lista.h"

// comportamiento del misil
bool misil_mover(cuerpo_t* misil, lista_t* obstaculos, tanque_t* otro_tanque, bool* kill);

#endif
