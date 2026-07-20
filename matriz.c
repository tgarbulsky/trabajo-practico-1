#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "matriz.h"

struct matriz{
    float *m;
    size_t filas, columnas;
};

size_t matriz_filas(const matriz_t *matriz){
    return matriz->filas;
}

size_t matriz_columnas(const matriz_t *matriz){
    return matriz->columnas;
}

float matriz_obtener(const matriz_t *matriz, size_t fila, size_t columna){
    return matriz->m[matriz_columnas(matriz)*(fila-1)+(columna-1)];
}

void matriz_establecer(matriz_t *matriz, size_t fila, size_t columna, float valor){
    matriz->m[matriz_columnas(matriz)*(fila-1)+(columna-1)]=valor;
}

static void _matriz_destruir(matriz_t *matriz){
    free(matriz->m);
    free(matriz);
}

void matriz_destruir(void* matriz){
    _matriz_destruir(matriz);
}

matriz_t *_matriz_crear(size_t n, size_t m){
    matriz_t *mx_s=malloc(sizeof(matriz_t));
    if(mx_s==NULL){
        return NULL;
    }
    mx_s->filas=n;
    mx_s->columnas=m;
    mx_s->m=malloc(((n)*(m))*sizeof(float));
    if(mx_s->m==NULL){
        free(mx_s);
        return NULL;
    }
    return mx_s;
}

void set_id_mx(matriz_t *mx_s){
    for(size_t i=0; i<matriz_filas(mx_s); i++){
        for(size_t j=0; j<matriz_columnas(mx_s); j++){
            if(i==j){
                matriz_establecer(mx_s, i+1, j+1, 1);
            }else{
                matriz_establecer(mx_s, i+1, j+1, 0);
            }
        }
    }
}

matriz_t *matriz_crear_proy(size_t n){
    matriz_t *matriz_mp=_matriz_crear(n, n);
    if(matriz_mp==NULL){
        return NULL;
    }
    set_id_mx(matriz_mp);
    matriz_establecer(matriz_mp, n, n-1, -1);
    matriz_establecer(matriz_mp, n, n, 0);
    return matriz_mp;
}

matriz_t *matriz_crear_rotz(double angz){
    double cos_angz=cos(angz);
    double sin_angz=sin(angz);
    matriz_t *matriz_mz=_matriz_crear(4, 4);
    if(matriz_mz==NULL){
        return NULL;
    }
    set_id_mx(matriz_mz);
    matriz_establecer(matriz_mz, 1, 1, cos_angz);
    matriz_establecer(matriz_mz, 1, 2, -sin_angz);
    matriz_establecer(matriz_mz, 2, 1, sin_angz);
    matriz_establecer(matriz_mz, 2, 2, cos_angz);
    return matriz_mz;
}

matriz_t *matriz_crear_roty(double angy){
    double cos_angy=cos(angy);
    double sin_angy=sin(angy);
    matriz_t *matriz_my=_matriz_crear(4, 4);
    if(matriz_my==NULL){
        return NULL;
    }
    set_id_mx(matriz_my);
    matriz_establecer(matriz_my, 1, 1, cos_angy);
    matriz_establecer(matriz_my, 1, 3, sin_angy);
    matriz_establecer(matriz_my, 3, 1, -sin_angy);
    matriz_establecer(matriz_my, 3, 3, cos_angy);
    return matriz_my;
}

matriz_t *matriz_crear_rotx(double angx){
    double cos_angx=cos(angx);
    double sin_angx=sin(angx);
    matriz_t *matriz_mx=_matriz_crear(4, 4);
    if(matriz_mx==NULL){
        return NULL;
    }
    set_id_mx(matriz_mx);
    matriz_establecer(matriz_mx, 2, 2, cos_angx);
    matriz_establecer(matriz_mx, 2, 3, -sin_angx);
    matriz_establecer(matriz_mx, 3, 2, sin_angx);
    matriz_establecer(matriz_mx, 3, 3, cos_angx);
    return matriz_mx;
}

matriz_t *matriz_crear_tras(const float vector[3]){
    matriz_t *matriz_mt=_matriz_crear(4, 4);
    if(matriz_mt==NULL){
        return NULL;
    }
    set_id_mx(matriz_mt);
    for(size_t i=0; i<3; i++){
        matriz_establecer(matriz_mt, i+1, 4, vector[i]);
    }
    return matriz_mt;
}

matriz_t *matriz_crear_escalar(size_t n, float* factor){
    matriz_t* escala=_matriz_crear(n, n);
    if(escala==NULL){
        return NULL;
    }
    set_id_mx(escala);
    for(size_t i=0; i<n; i++){
        for(size_t j=0; j<n; j++){
            if(i==j){
                matriz_establecer(escala, i+1, j+1, factor[i]);
            }
        }
    }
    return escala;
}

matriz_t *matriz_multiplicar(const matriz_t *a, const matriz_t *b){
    if(matriz_columnas(a)!=matriz_filas(b)){
        return NULL;
    }
    matriz_t *prod_mx=_matriz_crear(matriz_filas(a), matriz_columnas(b));
    if(prod_mx==NULL){
        return NULL;
    }
    for(size_t i=0; i<matriz_filas(a); i++){
        for(size_t j=0; j<matriz_columnas(b); j++){
            float coord_aux=0;
            for(size_t k=0; k<matriz_columnas(a); k++){
                coord_aux+=matriz_obtener(a, i+1, k+1)*matriz_obtener(b, k+1, j+1);
            }
            matriz_establecer(prod_mx, i+1, j+1, coord_aux);
        }
    }
    return prod_mx;        
}

matriz_t *matriz_aplicar(const matriz_t *matriz, const matriz_t *ps){
    size_t n=matriz_filas(ps);
    matriz_t* aux_mx=matriz_extender(ps);
    if(aux_mx==NULL){
        return NULL;
    }
    matriz_t* aux2_mx=matriz_multiplicar(matriz, aux_mx);
    matriz_destruir(aux_mx);
    if(aux2_mx==NULL){
        return NULL;
    }
    for(size_t j=0; j<n; j++){
        float w=matriz_obtener(aux2_mx, 4, j+1);
        if(w==0){
            w=1;
        }
        float app_x=matriz_obtener(aux2_mx, 1, j+1)/w;
        float app_y=matriz_obtener(aux2_mx, 2, j+1)/w;
        matriz_establecer(aux2_mx, 1, j+1, app_x);
        matriz_establecer(aux2_mx, 2, j+1, app_y);
        matriz_establecer(aux2_mx, 3, j+1, w);
        matriz_establecer(aux2_mx, 4, j+1, 1);
    }
    return aux2_mx;
}

matriz_t *matriz_extender(const matriz_t *matriz){
    size_t n=matriz_filas(matriz);
    size_t m=matriz_columnas(matriz);
    matriz_t* extendida=_matriz_crear(m+1, n);
    if(extendida==NULL){
        return NULL;
    }
    for(size_t i=0; i<n; i++){
        size_t j=0;
        for(j=0; j<m; j++){
            matriz_establecer(extendida, j+1, i+1, matriz_obtener(matriz, i+1, j+1));
        }
        matriz_establecer(extendida, j+1, i+1, 1);
    }
    return extendida;
}

//Debug
void print_mx(matriz_t *mx){
    for(size_t i=0; i<matriz_filas(mx); i++){
        for(size_t j=0; j<matriz_columnas(mx); j++){
            printf("%f ", matriz_obtener(mx, i+1, j+1));
        }
        putchar('\n');
    }
}
//
