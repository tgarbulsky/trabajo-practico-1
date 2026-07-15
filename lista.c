#include "lista.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct nodo {
    void *dato;
    struct nodo *prox;
} nodo_t;

struct lista {
    nodo_t *prim;
    nodo_t *ult;
    size_t largo;
};

struct lista_iter {
    lista_t *lista;
    nodo_t *ant;
    nodo_t *act;
};

static nodo_t *crear_nodo(void *dato, nodo_t *prox) {
    nodo_t *n = malloc(sizeof(nodo_t));
    if (n == NULL) return NULL;
    n->dato = dato;
    n->prox = prox;
    return n;
}

lista_t *lista_crear() {
    lista_t *l = malloc(sizeof(lista_t));
    if (l == NULL) return NULL;
    l->prim = NULL;
    l->ult = NULL;
    l->largo = 0;
    return l;
}

bool lista_esta_vacia(const lista_t *l) { return l->largo == 0; }

size_t lista_largo(const lista_t *l) { return l->largo; }

bool lista_insertar_primero(lista_t *l, void *dato) {
    nodo_t *n = crear_nodo(dato, l->prim);
    if (n == NULL) return false;
    l->prim = n;
    if (l->largo == 0) l->ult = n;
    l->largo++;
    return true;
}

bool lista_insertar_ultimo(lista_t *l, void *dato) {
    nodo_t *n = crear_nodo(dato, NULL);
    if (n == NULL) return false;
    if (l->largo == 0) l->prim = n;
    else l->ult->prox = n;
    l->ult = n;
    l->largo++;
    return true;
}

void *lista_borrar_primero(lista_t *l) {
    if (l->prim == NULL) return NULL;
    nodo_t *n = l->prim;
    void *dato = n->dato;
    l->prim = n->prox;
    if (l->largo == 1) l->ult = NULL;
    free(n);
    l->largo--;
    return dato;
}

void lista_destruir(lista_t *l, void (*destruir_dato)(void *)) {
    while (l->prim != NULL) {
        void *dato = lista_borrar_primero(l);
        if (destruir_dato) destruir_dato(dato);
    }
    free(l);
}

// Primitivas del Iterador
lista_iter_t *lista_iter_crear(lista_t *l) {
    lista_iter_t *iter = malloc(sizeof(lista_iter_t));
    if (iter == NULL) return NULL;
    iter->lista = l;
    iter->ant = NULL;
    iter->act = l->prim;
    return iter;
}

bool lista_iter_avanzar(lista_iter_t *iter) {
    if (iter->act == NULL) return false;
    iter->ant = iter->act;
    iter->act = iter->act->prox;
    return true;
}

void *lista_iter_ver_actual(const lista_iter_t *iter) {
    return (iter->act == NULL) ? NULL : iter->act->dato;
}

bool lista_iter_al_final(const lista_iter_t *iter) { return iter->act == NULL; }

void lista_iter_destruir(lista_iter_t *iter) { free(iter); }