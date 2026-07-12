#include "lista.h"
#include <stdlib.h>

// Estructura interna del nodo de la lista
typedef struct nodo_lista {
    void *dato;
    struct nodo_lista *sig;
} nodo_lista_t;

// Estructura principal de la lista
struct lista {
    nodo_lista_t *primero;
    nodo_lista_t *ultimo;
    size_t largo;
};

// Estructura principal del iterador externo
struct lista_iter {
    lista_t *lista;
    nodo_lista_t *anterior;
    nodo_lista_t *actual;
};

lista_t *lista_crear(void) {
    lista_t *l = malloc(sizeof(lista_t));
    if (l == NULL) return NULL;
    
    l->primero = NULL;
    l->ultimo = NULL;
    l->largo = 0;
    return l;
}

bool lista_esta_vacia(const lista_t *lista) {
    if (lista == NULL) return true;
    return lista->largo == 0;
}

bool lista_insertar_primero(lista_t *lista, void *dato) {
    if (lista == NULL) return false;

    nodo_lista_t *nuevo = malloc(sizeof(nodo_lista_t));
    if (nuevo == NULL) return false;
    
    nuevo->dato = dato;
    nuevo->sig = lista->primero;
    
    if (lista_esta_vacia(lista)) {
        lista->ultimo = nuevo;
    }
    
    lista->primero = nuevo;
    lista->largo++;
    return true;
}

bool lista_insertar_ultimo(lista_t *lista, void *dato) {
    if (lista == NULL) return false;

    nodo_lista_t *nuevo = malloc(sizeof(nodo_lista_t));
    if (nuevo == NULL) return false;
    
    nuevo->dato = dato;
    nuevo->sig = NULL;

    if (lista_esta_vacia(lista)) {
        lista->primero = nuevo;
    } else {
        lista->ultimo->sig = nuevo;
    }
    
    lista->ultimo = nuevo;
    lista->largo++;
    return true;
}

size_t lista_largo(const lista_t *lista) {
    if (lista == NULL) return 0;
    return lista->largo;
}

void *lista_borrar_primero(lista_t *lista) {
    if (lista == NULL || lista_esta_vacia(lista)) return NULL;
    
    nodo_lista_t *aux = lista->primero;
    void *dato = aux->dato;
    
    lista->primero = aux->sig;
    if (lista->primero == NULL) {
        lista->ultimo = NULL;
    }
    
    free(aux);
    lista->largo--;
    return dato;
}

void lista_destruir(lista_t *lista, void (*destruir_dato)(void *)) {
    if (lista == NULL) return;
    
    while (!lista_esta_vacia(lista)) {
        void *dato = lista_borrar_primero(lista);
        if (destruir_dato != NULL) {
            destruir_dato(dato);
        }
    }
    free(lista);
}

void lista_recorrer(lista_t *lista, bool (*visitar)(void *dato, void *extra), void *extra) {
    if (lista == NULL || visitar == NULL) return;
    
    nodo_lista_t *actual = lista->primero;
    while (actual != NULL) {
        if (!visitar(actual->dato, extra)) {
            break;
        }
        actual = actual->sig;
    }
}

/* --- ITERADOR EXTERNO --- */

lista_iter_t *lista_iter_crear(lista_t *lista) {
    if (lista == NULL) return NULL;

    lista_iter_t *iter = malloc(sizeof(lista_iter_t));
    if (iter == NULL) return NULL;
    
    iter->lista = lista;
    iter->anterior = NULL;
    iter->actual = lista->primero;
    return iter;
}

bool lista_iter_avanzar(lista_iter_t *iter) {
    if (iter == NULL || lista_iter_al_final(iter)) return false;
    
    iter->anterior = iter->actual;
    iter->actual = iter->actual->sig;
    return true;
}

void *lista_iter_ver_actual(const lista_iter_t *iter) {
    if (iter == NULL || lista_iter_al_final(iter)) return NULL;
    return iter->actual->dato;
}

bool lista_iter_al_final(const lista_iter_t *iter) {
    if (iter == NULL) return true;
    return iter->actual == NULL;
}

void lista_iter_destruir(lista_iter_t *iter) {
    free(iter);
}

bool lista_iter_insertar(lista_iter_t *iter, void *dato) {
    if (iter == NULL) return false;

    // Caso 1: Insertar al principio de la lista
    if (iter->anterior == NULL) {
        bool ok = lista_insertar_primero(iter->lista, dato);
        if (ok) {
            iter->actual = iter->lista->primero;
        }
        return ok;
    }

    // Caso 2: Insertar en medio o al final
    nodo_lista_t *nuevo = malloc(sizeof(nodo_lista_t));
    if (nuevo == NULL) return false;
    
    nuevo->dato = dato;
    nuevo->sig = iter->actual;
    iter->anterior->sig = nuevo;
    
    // Si insertamos justo al final (donde actual era NULL)
    if (iter->actual == NULL) {
        iter->lista->ultimo = nuevo;
    }
    
    iter->actual = nuevo;
    iter->lista->largo++;
    return true;
}

void *lista_iter_borrar(lista_iter_t *iter) {
    if (iter == NULL || lista_iter_al_final(iter)) return NULL;
    
    nodo_lista_t *aux = iter->actual;
    void *dato = aux->dato;

    // Caso 1: Borrar el primer elemento
    if (iter->anterior == NULL) {
        iter->lista->primero = aux->sig;
        iter->actual = aux->sig;
        if (iter->actual == NULL) {
            iter->lista->ultimo = NULL;
        }
    } else {
        // Caso 2: Borrar en el medio o al final
        iter->anterior->sig = aux->sig;
        iter->actual = aux->sig;
        if (iter->actual == NULL) {
            iter->lista->ultimo = iter->anterior;
        }
    }
    
    free(aux);
    iter->lista->largo--;
    return dato;
}