#ifndef OBSTACULO_H
#define OBSTACULO_H

#include "modelo.h" // Aporta modelo_t (redefinir el typedef acá rompe en C99)

typedef struct obstaculo obstaculo_t;

// Constructor y Destructor
obstaculo_t *obstaculo_crear(float x, float y, float phi, const modelo_t *modelo);
void         obstaculo_destruir(obstaculo_t *o);

// Getters públicos
float           obstaculo_x(const obstaculo_t *o);
float           obstaculo_y(const obstaculo_t *o);
float           obstaculo_phi(const obstaculo_t *o);
const modelo_t *obstaculo_modelo(const obstaculo_t *o);

#endif