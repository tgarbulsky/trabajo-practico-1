#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>
#include <stddef.h>

typedef struct lista lista_t;
typedef struct lista_iter lista_iter_t;

/* --- Primitivas de lista --- */

// Crea una lista vacía.
lista_t *lista_crear(void);

// Verifica si la lista está vacía.
bool lista_esta_vacia(const lista_t *lista);

// Agrega un elemento al principio.
bool lista_insertar_primero(lista_t *lista, void *dato);

// Agrega un elemento al final.
bool lista_insertar_ultimo(lista_t *lista, void *dato);

// Devuelve la cantidad de elementos.
size_t lista_largo(const lista_t *lista);

// Borra el primer elemento y devuelve su dato.
void *lista_borrar_primero(lista_t *lista);

// Destruye la lista y aplica destruir_dato a cada elemento si no es NULL.
void lista_destruir(lista_t *lista, void (*destruir_dato)(void *));

// Iterador interno: recorre la lista llamando a 'visitar' para cada dato.
void lista_recorrer(lista_t *lista, bool (*visitar)(void *dato, void *extra), void *extra);

/* --- Primitivas de iteración externa --- */

// Crea un iterador para la lista.
lista_iter_t *lista_iter_crear(lista_t *lista);

// Avanza a la siguiente posición.
bool lista_iter_avanzar(lista_iter_t *iter);

// Devuelve el dato de la posición actual.
void *lista_iter_ver_actual(const lista_iter_t *iter);

// Verifica si el iterador llegó al final de la lista.
bool lista_iter_al_final(const lista_iter_t *iter);

// Destruye el iterador.
void lista_iter_destruir(lista_iter_t *iter);

// Inserta un elemento en la posición actual.
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

// Elimina el elemento en la posición actual.
void *lista_iter_borrar(lista_iter_t *iter);

#endif