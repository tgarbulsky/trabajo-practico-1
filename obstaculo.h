#ifndef OBSTACULO_H
#define OBSTACULO_H

// Tipos de bloques que se pueden renderizar en el mapa (tu propia lista)
typedef enum {
    CUBO1, 
    CUBO2, 
    CUBO3, 
    PIRAMIDE1, 
    PIRAMIDE2, 
    PIRAMIDE3
} tipo_obstaculo_t;

// Estructura limpia para tus obstáculos
typedef struct obstaculo {
    tipo_obstaculo_t tipo;
    float pos[3];
    float angz;
} obstaculo_t;

// Funciones para tus obstáculos
obstaculo_t* obstaculo_crear(void);
void obstaculo_destruir(obstaculo_t* obs);

#endif // OBSTACULO_H