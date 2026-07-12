#include "cola.h"
#include <stdlib.h>

// Nodo interno de la cola
typedef struct nodo_cola {
    void *dato;
    struct nodo_cola *sig;
} nodo_cola_t;

// Estructura principal de la cola
struct cola {
    nodo_cola_t *primero;
    nodo_cola_t *ultimo;
};

cola_t *cola_crear(void) {
    cola_t *c = malloc(sizeof(cola_t));
    if (c == NULL) return NULL;
    
    c->primero = NULL;
    c->ultimo = NULL;
    return c;
}

void cola_destruir(cola_t *cola, void (*destruir_dato)(void *)) {
    if (cola == NULL) return;
    
    while (!cola_esta_vacia(cola)) {
        void *dato = cola_desencolar(cola);
        if (destruir_dato != NULL) {
            destruir_dato(dato);
        }
    }
    free(cola);
}

bool cola_esta_vacia(const cola_t *cola) {
    if (cola == NULL) return true;
    return cola->primero == NULL;
}

bool cola_encolar(cola_t *cola, void *valor) {
    if (cola == NULL) return false;

    nodo_cola_t *nuevo = malloc(sizeof(nodo_cola_t));
    if (nuevo == NULL) return false;
    
    nuevo->dato = valor;
    nuevo->sig = NULL;

    if (cola_esta_vacia(cola)) {
        cola->primero = nuevo;
    } else {
        cola->ultimo->sig = nuevo;
    }
    
    cola->ultimo = nuevo;
    return true;
}

void *cola_ver_primero(const cola_t *cola) {
    if (cola == NULL || cola_esta_vacia(cola)) return NULL;
    return cola->primero->dato;
}

void *cola_desencolar(cola_t *cola) {
    if (cola == NULL || cola_esta_vacia(cola)) return NULL;
    
    nodo_cola_t *aux = cola->primero;
    void *dato = aux->dato;
    
    cola->primero = aux->sig;
    
    // Si la cola se quedó vacía, el último también debe apuntar a NULL
    if (cola->primero == NULL) {
        cola->ultimo = NULL;
    }
    
    free(aux);
    return dato;
}