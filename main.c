#include <SDL2/SDL.h>
#include "modelo.h"
#include "obstaculo.h"
#include "tanque.h"
#include "stl.h"
#include "matriz.h"
#include "pila.h"
#include "lista.h"
#include "mundo.h"
#include "animaciones.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define VENTANA_ANCHO 1024
#define VENTANA_ALTO 768
#define JUEGO_FPS 24

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// BEGIN código del alumno (funciones auxiliares)

#define ALCANCE_MISIL 48.0f   // 24 m/s durante 2 segundos
#define UMBRAL_PUNTERIA 0.1f
// Mitad del campo visual horizontal: atan(1024/768) ≈ 0.93 rad
#define MEDIO_FOV 0.93f

static float normalizar_angulo(float a) {
    while (a > (float)M_PI) a -= 2 * (float)M_PI;
    while (a <= -(float)M_PI) a += 2 * (float)M_PI;
    return a;
}

static void destruir_modelo_void(void *m) {
    modelo_destruir(m);
}

// Carga todos los modelos del archivo STL binario en la lista.
static bool cargar_modelos(lista_t *lista, const char *ruta) {
    FILE *f = fopen(ruta, "rb");
    if (f == NULL) return false;

    unidades_t unidades;
    size_t maxlong;
    if (!leer_encabezado_stl(f) || !leer_formato_STL(f, &unidades, &maxlong)) {
        fclose(f);
        return false;
    }

    while (1) {
        float *coordenadas = NULL;
        size_t ncoordenadas = 0;
        size_t *lineas = NULL;
        size_t nlineas = 0;
        char etiqueta[maxlong + 1];
        if (!leer_modelo_3d(f, maxlong, etiqueta, &ncoordenadas, &coordenadas,
                            &nlineas, &lineas))
            break;

        modelo_t *m = modelo_crear(etiqueta, coordenadas, ncoordenadas, lineas, nlineas);
        free(coordenadas);
        free(lineas);
        if (m != NULL && !lista_insertar_ultimo(lista, m))
            modelo_destruir(m);
    }
    fclose(f);
    return !lista_esta_vacia(lista);
}

// Construye la matriz de cámara del enunciado:
// Mper · Mz(π/2 + angz) · My(π/2 − angx) · Mz(−phi) · Mt(−x, −y, −3)
static matriz_t *camara_crear(float angz, float angx, float phi, float x, float y) {
    matriz_t *cam = matriz_crear_mper(4);
    const float traslacion[3] = {-x, -y, -3.0f};
    matriz_t *pasos[4];
    pasos[0] = matriz_crear_mz((float)M_PI / 2 + angz);
    pasos[1] = matriz_crear_my((float)M_PI / 2 - angx);
    pasos[2] = matriz_crear_mz(-phi);
    pasos[3] = matriz_crear_mt(traslacion);

    for (int i = 0; i < 4; i++) {
        matriz_t *tmp = matriz_multiplicar(cam, pasos[i]);
        matriz_destruir(cam);
        matriz_destruir(pasos[i]);
        cam = tmp;
    }
    return cam;
}

static void punto_a_pantalla(float x, float y, int *sx, int *sy) {
    *sx = (int)(x * VENTANA_ALTO / 2 + VENTANA_ANCHO / 2);
    *sy = (int)(VENTANA_ALTO / 2 - y * VENTANA_ALTO / 2);
}

// Dibuja un modelo en (x, y, z) rotado 'rot' alrededor de Z, usando la
// cámara que está en el tope de la pila. Las líneas que cruzan el plano
// w = 1 se recortan interpolando en coordenadas homogéneas.
static void render_modelo(SDL_Renderer *renderer, pila_t *pila, const modelo_t *m,
                          float x, float y, float z, float rot) {
    if (m == NULL) return;

    float tv[3] = {x, y, z};
    matriz_t *mt = matriz_crear_mt(tv);
    matriz_t *mr = matriz_crear_mz(rot);
    matriz_t *obj = matriz_multiplicar(mt, mr);
    matriz_t *final = matriz_multiplicar(pila_ver_tope(pila), obj);
    matriz_destruir(mt);
    matriz_destruir(mr);
    matriz_destruir(obj);
    pila_apilar(pila, final);

    const float *c = modelo_coordenadas(m);
    const size_t *l = modelo_lineas(m);
    size_t nv = modelo_ncoordenadas(m);
    size_t nl = modelo_nlineas(m);

    matriz_t *pts = _matriz_crear(nv, 3);
    if (pts != NULL) {
        for (size_t k = 0; k < nv; k++) {
            matriz_establecer(pts, k, 0, c[3 * k]);
            matriz_establecer(pts, k, 1, c[3 * k + 1]);
            matriz_establecer(pts, k, 2, c[3 * k + 2]);
        }

        matriz_t *proj = matriz_aplicar(pila_ver_tope(pila), pts);
        if (proj != NULL) {
            for (size_t j = 0; j < 2 * nl; j += 2) {
                size_t i0 = l[j], i1 = l[j + 1];
                float w0 = matriz_obtener(proj, i0, 2);
                float w1 = matriz_obtener(proj, i1, 2);
                if (w0 < 1.0f && w1 < 1.0f) continue;

                // Coordenadas homogéneas (x·w) para poder recortar en w = 1
                float xh0 = matriz_obtener(proj, i0, 0) * w0;
                float yh0 = matriz_obtener(proj, i0, 1) * w0;
                float xh1 = matriz_obtener(proj, i1, 0) * w1;
                float yh1 = matriz_obtener(proj, i1, 1) * w1;

                if (w0 < 1.0f) {
                    float t = (1.0f - w0) / (w1 - w0);
                    xh0 += t * (xh1 - xh0);
                    yh0 += t * (yh1 - yh0);
                    w0 = 1.0f;
                } else if (w1 < 1.0f) {
                    float t = (1.0f - w1) / (w0 - w1);
                    xh1 += t * (xh0 - xh1);
                    yh1 += t * (yh0 - yh1);
                    w1 = 1.0f;
                }

                int sx0, sy0, sx1, sy1;
                punto_a_pantalla(xh0 / w0, yh0 / w0, &sx0, &sy0);
                punto_a_pantalla(xh1 / w1, yh1 / w1, &sx1, &sy1);
                SDL_RenderDrawLine(renderer, sx0, sy0, sx1, sy1);
            }
            matriz_destruir(proj);
        }
        matriz_destruir(pts);
    }
    matriz_destruir(pila_desapilar(pila));
}

// Tabla de búsqueda del display de 7 segmentos (A B C D E F G por dígito)
static const bool DIGITO_SEGMENTOS[10][7] = {
    {1, 1, 1, 1, 1, 1, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1}, // 2
    {1, 1, 1, 1, 0, 0, 1}, // 3
    {0, 1, 1, 0, 0, 1, 1}, // 4
    {1, 0, 1, 1, 0, 1, 1}, // 5
    {1, 0, 1, 1, 1, 1, 1}, // 6
    {1, 1, 1, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 1, 0, 1, 1}, // 9
};

static void dibujar_digito(SDL_Renderer *r, int x, int y, int s, int d) {
    // Extremos de cada segmento en unidades de 's' (dígito de s x 2s)
    static const int SEG[7][4] = {
        {0, 0, 1, 0}, // A: arriba
        {1, 0, 1, 1}, // B: derecha arriba
        {1, 1, 1, 2}, // C: derecha abajo
        {0, 2, 1, 2}, // D: abajo
        {0, 1, 0, 2}, // E: izquierda abajo
        {0, 0, 0, 1}, // F: izquierda arriba
        {0, 1, 1, 1}, // G: medio
    };
    for (int i = 0; i < 7; i++)
        if (DIGITO_SEGMENTOS[d][i])
            SDL_RenderDrawLine(r, x + SEG[i][0] * s, y + SEG[i][1] * s,
                               x + SEG[i][2] * s, y + SEG[i][3] * s);
}

// Dibuja un número alineado a la derecha en x_derecha.
static void dibujar_numero(SDL_Renderer *r, int x_derecha, int y, int s, int n) {
    if (n < 0) n = 0;
    int x = x_derecha;
    do {
        x -= 2 * s;
        dibujar_digito(r, x, y, s, n % 10);
        n /= 10;
    } while (n > 0);
}



// END código del alumno (funciones auxiliares)

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;

    SDL_CreateWindowAndRenderer(VENTANA_ANCHO, VENTANA_ALTO, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "Battle Zone");

    int dormir = 0;

    // BEGIN código del alumno
    srand((unsigned int)time(NULL));

    lista_t *lista_modelos = lista_crear();
    if (lista_modelos == NULL) return 1;
    if (!cargar_modelos(lista_modelos, "modelos.stl"))
        fprintf(stderr, "ADVERTENCIA: no se pudieron cargar modelos de modelos.stl\n");

    const modelo_t *modelo_tanque = modelo_buscar(lista_modelos, "TANQUE");
    const modelo_t *modelo_torreta = modelo_buscar(lista_modelos, "TORRETA");
    const modelo_t *modelo_radar = modelo_buscar(lista_modelos, "RADAR");
    const modelo_t *modelo_misil = modelo_buscar(lista_modelos, "MISIL");
    const modelo_t *modelo_horizonte = modelo_buscar(lista_modelos, "HORIZONTE");
    const modelo_t *modelo_montanas = modelo_buscar(lista_modelos, "MONTANAS");
    const modelo_t *modelo_luna = modelo_buscar(lista_modelos, "LUNA");

    mundo_t *mundo = mundo_crear(lista_modelos);
    animacion_t *anim_cristal = animacion_crear();
    animacion_t *anim_explosion = animacion_crear();
    pila_t *stack = pila_crear();
    matriz_t *ident = matriz_crear_identidad(4);
    if (mundo == NULL || anim_cristal == NULL || anim_explosion == NULL ||
        stack == NULL || ident == NULL) {
        fprintf(stderr, "ERROR: no se pudo inicializar el juego\n");
        return 1;
    }
    pila_apilar(stack, ident);

    unsigned int prev_ticks = SDL_GetTicks();
    float angx = 0, angz = 0;
    float tiempo_fin = 0;
    // END código del alumno

    unsigned int ticks = SDL_GetTicks();
    int done = 0;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                done = 1;
                break;
            }

            // BEGIN código del alumno
            if (event.type == SDL_KEYDOWN && !mundo_terminado(mundo)) {
                tanque_t *jugador = mundo_jugador(mundo);
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        tanque_iniciar_movimiento(jugador, MOV_ADELANTE);
                        break;
                    case SDLK_DOWN:
                        tanque_iniciar_movimiento(jugador, MOV_ATRAS);
                        break;
                    case SDLK_RIGHT:
                        tanque_iniciar_movimiento(jugador, MOV_GIRAR_DER);
                        break;
                    case SDLK_LEFT:
                        tanque_iniciar_movimiento(jugador, MOV_GIRAR_IZQ);
                        break;
                    case ' ':
                        if (tanque_puede_disparar(jugador))
                            tanque_disparar(jugador);
                        break;
                }
            }
            // END código del alumno
        }
        if (done) break;

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);

        // BEGIN código del alumno
        float dt = (SDL_GetTicks() - prev_ticks) / 1000.0f;
        if (dt > 0.1f) dt = 0.1f;
        prev_ticks = SDL_GetTicks();

        tanque_t *jugador = mundo_jugador(mundo);
        tanque_t *enemigo = mundo_enemigo(mundo);

        // Sacudida de la cámara mientras el tanque se mueve, con
        // decaimiento para que no derive con el tiempo
        movimiento_e mov = tanque_movimiento(jugador);
        if (mov == MOV_GIRAR_IZQ || mov == MOV_GIRAR_DER)
            angz += ((rand() % 2001) - 1000) / 100000.0f;
        else if (mov != MOV_NINGUNO)
            angx += ((rand() % 2001) - 1000) / 100000.0f;
        angx -= angx * 2.0f * dt;
        angz -= angz * 2.0f * dt;

        if (!mundo_terminado(mundo))
            mundo_actualizar(mundo, dt);
        enemigo = mundo_enemigo(mundo); // pudo haber sido reemplazado

        // Eventos del mundo -> animaciones
        float ev_x, ev_y;
        if (mundo_evento_enemigo_destruido(mundo, &ev_x, &ev_y))
            animacion_iniciar_explosion(anim_explosion, ev_x, ev_y);
        if (mundo_evento_jugador_impactado(mundo))
            animacion_iniciar_cristal(anim_cristal);
        animacion_actualizar(anim_cristal, dt);
        animacion_actualizar(anim_explosion, dt);

        float px = tanque_x(jugador), py = tanque_y(jugador);
        float pp = tanque_phi(jugador);

        // Fondo: cámara que solo rota (el horizonte no se traslada)
        matriz_t *cam_fondo = camara_crear(angz, angx, pp, 0, 0);
        pila_apilar(stack, cam_fondo);
        SDL_SetRenderDrawColor(renderer, 0x80, 0x80, 0x80, 0x00);
        render_modelo(renderer, stack, modelo_horizonte, 0, 0, 0, 0);
        SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0x40, 0x00);
        render_modelo(renderer, stack, modelo_montanas, 0, 0, 0, 0);
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xCC, 0x00);
        render_modelo(renderer, stack, modelo_luna, 0, 0, 0, 0);
        matriz_destruir(pila_desapilar(stack));

        // Mundo: cámara completa
        matriz_t *cam = camara_crear(angz, angx, pp, px, py);
        pila_apilar(stack, cam);

        // Obstáculos (blanco)
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0x00);
        for (size_t i = 0; i < mundo_num_obstaculos(mundo); i++) {
            obstaculo_t *o = mundo_obstaculo(mundo, i);
            render_modelo(renderer, stack, obstaculo_modelo(o),
                          obstaculo_x(o), obstaculo_y(o), 0.0f, obstaculo_phi(o));
        }

        // Tanque enemigo: casco + torreta + radar
        if (enemigo != NULL) {
            float ex = tanque_x(enemigo), ey = tanque_y(enemigo);
            float ep = tanque_phi(enemigo), et = tanque_torreta(enemigo);

            SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0x00);
            render_modelo(renderer, stack, modelo_tanque, ex, ey, 0.0f, ep);

            SDL_SetRenderDrawColor(renderer, 0xFF, 0x80, 0x00, 0x00);
            render_modelo(renderer, stack, modelo_torreta, ex, ey, 3.0f, ep + et);

            if (modelo_radar != NULL) {
                float radar_x = ex + (-1.5f) * cosf(ep + et);
                float radar_y = ey + (-1.5f) * sinf(ep + et);
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0x00, 0x00);
                render_modelo(renderer, stack, modelo_radar, radar_x, radar_y,
                              3.5f, ep + et);
            }
        }

        // Misiles en vuelo
        if (tanque_misil_activo(jugador)) {
            SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0x00);
            render_modelo(renderer, stack, modelo_misil, tanque_misil_x(jugador),
                          tanque_misil_y(jugador), 0.0f, tanque_misil_phi(jugador));
        }
        if (enemigo != NULL && tanque_misil_activo(enemigo)) {
            SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0x00);
            render_modelo(renderer, stack, modelo_misil, tanque_misil_x(enemigo),
                          tanque_misil_y(enemigo), 0.0f, tanque_misil_phi(enemigo));
        }

        // Piezas del enemigo destruido (tiro oblicuo)
        if (animacion_activa(anim_explosion)) {
            SDL_SetRenderDrawColor(renderer, 0xFF, 0x80, 0x00, 0x00);
            for (size_t i = 0; i < EXPLOSION_PIEZAS; i++) {
                float ax, ay, az, arot;
                if (!animacion_pieza_posicion(anim_explosion, i, &ax, &ay, &az, &arot))
                    continue;
                render_modelo(renderer, stack,
                              modelo_buscar(lista_modelos,
                                            animacion_pieza_modelo(anim_explosion, i)),
                              ax, ay, az, arot);
            }
        }

        matriz_destruir(pila_desapilar(stack));

// ---- HUD ----
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0x00);

        // 1. Vidas restantes con el modelo '*' (sin incluir la actual)
        const modelo_t *modelo_vida = modelo_buscar(lista_modelos, "*");
        if (modelo_vida != NULL) {
            float x_vida = -0.9f; // Coordenadas de pantalla normalizadas si render_modelo escala a HUD
            float y_vida = 0.8f;  // Ajustá estos valores si sale corrido de la pantalla
            for (int i = 0; i < (mundo_vidas(mundo) - 1); i++) {
                render_modelo(renderer, stack, modelo_vida, x_vida, y_vida, 0.0f, 0.0f);
                x_vida += 0.1f; // Espaciado
            }
        }

        // 2. Puntaje
        dibujar_numero(renderer, VENTANA_ANCHO - 30, 20, 10, mundo_puntaje(mundo));

        // 3. Mira del tanque con '+' o '-' según puntería
        bool enemigo_en_mira = false;
        float rumbo_error = 0;
        if (enemigo != NULL) {
            float dx = tanque_x(enemigo) - px, dy = tanque_y(enemigo) - py;
            float dist = sqrtf(dx * dx + dy * dy); // Se usa acá para el alcance del misil
            rumbo_error = normalizar_angulo(atan2f(dy, dx) - pp);
            enemigo_en_mira = fabsf(rumbo_error) < 0.15f && dist <= ALCANCE_MISIL;
        }

        const modelo_t *modelo_mira = modelo_buscar(lista_modelos, enemigo_en_mira ? "+" : "-");
        if (modelo_mira != NULL) {
            // Se dibuja en el centro (0,0) de la pantalla
            render_modelo(renderer, stack, modelo_mira, 0.0f, 0.0f, 0.0f, 0.0f);
        }

        // 4. Indicador de dirección del enemigo cuando queda fuera de la vista (> 1 rad)
        if (enemigo != NULL && fabsf(rumbo_error) > 1.0f) {
            // Si tenés funciones de texto de la cátedra podés descomentar y usarlas acá:
            if (rumbo_error >= 1.0f && rumbo_error <= 2.44f) {
                // "IZQUIERDA"
            } else if (rumbo_error <= -1.0f && rumbo_error >= -2.44f) {
                // "DERECHA"
            } else if (fabsf(rumbo_error) > 2.44f) {
                // "DETRAS"
            }
        }


// 5. Vidrio roto encima de todo usando el modelo "#"
  // 5. Vidrio roto encima de todo usando el modelo "#"

        if (animacion_activa(anim_cristal)) {
            const modelo_t *modelo_cristal = modelo_buscar(lista_modelos, "#");
            if (modelo_cristal != NULL) {
                size_t rajaduras = animacion_cristal_visibles(anim_cristal);
                for (size_t i = 0; i < rajaduras; i++) {
                    float x0, y0, x1, y1;
                    animacion_cristal_linea(anim_cristal, i, &x0, &y0, &x1, &y1);
                    
                    // x0 e y0 vienen entre -1.0 y 1.0 (espacio normalizado de pantalla)
                    // Los convertimos manualmente a píxeles sin pasar por la cámara 3D
                    float px = (VENTANA_ANCHO / 2.0f) * (1.0f + x0);
                    float py = (VENTANA_ALTO / 2.0f) * (1.0f - y0); // Invertimos Y para SDL
                    
                    // Reseteamos el stack de transformaciones a la matriz del HUD
                    // Si tu render_modelo escala internamente, pasale px, py directamente.
                    // Si render_modelo usa el stack, podés mapearlo con las funciones de tu TP.
                    render_modelo(renderer, stack, modelo_cristal, px, py, 0.0f, 0.0f);
                }
            }
        }
        // Fin del juego: al terminar la animación, activamos el estado estático en vez de cerrar
        if (mundo_terminado(mundo) && !animacion_activa(anim_cristal)) {
            tiempo_fin += dt;
            if (tiempo_fin > 1.5f) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Color Rojo porque pocho lo pidió

                //Letra G
            SDL_RenderDrawLine(renderer, 320, 220, 300, 220);
            SDL_RenderDrawLine(renderer, 300, 220, 300, 260);
            SDL_RenderDrawLine(renderer, 300, 260, 320, 260);
            SDL_RenderDrawLine(renderer, 320, 260, 320, 245);
            SDL_RenderDrawLine(renderer, 320, 245, 310, 245);

            //Letra A
            SDL_RenderDrawLine(renderer, 330, 260, 340, 220);
            SDL_RenderDrawLine(renderer, 340, 220, 350, 260);
            SDL_RenderDrawLine(renderer, 335, 245, 345, 245);

            //Letra M
            SDL_RenderDrawLine(renderer, 360, 260, 360, 220);
            SDL_RenderDrawLine(renderer, 360, 220, 370, 240);
            SDL_RenderDrawLine(renderer, 370, 240, 380, 220);
            SDL_RenderDrawLine(renderer, 380, 220, 380, 260);

            //Letra E
            SDL_RenderDrawLine(renderer, 410, 220, 390, 220);
            SDL_RenderDrawLine(renderer, 390, 220, 390, 260);
            SDL_RenderDrawLine(renderer, 390, 260, 410, 260);
            SDL_RenderDrawLine(renderer, 390, 240, 405, 240);

            //Letra O
            SDL_RenderDrawLine(renderer, 430, 220, 450, 220);
            SDL_RenderDrawLine(renderer, 450, 220, 450, 260);
            SDL_RenderDrawLine(renderer, 450, 260, 430, 260);
            SDL_RenderDrawLine(renderer, 430, 260, 430, 220);

            //Letra V
            SDL_RenderDrawLine(renderer, 460, 220, 470, 260);
            SDL_RenderDrawLine(renderer, 470, 260, 480, 220);

            //Letra E
            SDL_RenderDrawLine(renderer, 510, 220, 490, 220);
            SDL_RenderDrawLine(renderer, 490, 220, 490, 260);
            SDL_RenderDrawLine(renderer, 490, 260, 510, 260);
            SDL_RenderDrawLine(renderer, 490, 240, 505, 240);

            //Letra R
            SDL_RenderDrawLine(renderer, 520, 260, 520, 220);
            SDL_RenderDrawLine(renderer, 520, 220, 540, 220);
            SDL_RenderDrawLine(renderer, 540, 220, 540, 240);
            SDL_RenderDrawLine(renderer, 540, 240, 520, 240);
            SDL_RenderDrawLine(renderer, 520, 240, 540, 260);
        }
    }
        // END código del alumno

        SDL_RenderPresent(renderer);
        ticks = SDL_GetTicks() - ticks;
        if (dormir) {
            SDL_Delay(dormir);
            dormir = 0;
        } else if (ticks < 1000 / JUEGO_FPS)
            SDL_Delay(1000 / JUEGO_FPS - ticks);
        else
            printf("Perdiendo cuadros\n");
        ticks = SDL_GetTicks();
    }

    // El juego solo llega acá cuando el usuario decide salir (por ejemplo, cerrando la ventana o tocando ESC)
    // BEGIN código del alumno
    animacion_destruir(anim_cristal);
    animacion_destruir(anim_explosion);
    mundo_destruir(mundo);
    matriz_destruir(pila_desapilar(stack)); // identidad apilada al inicio
    pila_destruir(stack, NULL);
    lista_destruir(lista_modelos, destruir_modelo_void);
    // END código del alumno

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}
