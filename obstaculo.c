#include "obstaculo.h"
#include <stdlib.h>

lista_t* inicializar_obstaculos() {
    lista_t* lista = lista_crear();
    if (lista == NULL) return NULL;

    for (int i = 0; i < NRO_OBSTACULOS; i++) {
        cuerpo_t* obstaculo = crear_obstaculo();
        if (obstaculo == NULL) {
            lista_destruir(lista, free);
            return NULL;
        }
        lista_insertar_ultimo(lista, obstaculo);
    }
    return lista;
}