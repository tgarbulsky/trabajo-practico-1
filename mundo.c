#include "mundo.h"
#include "modelo.h"
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define NUM_OBSTACULOS 50
#define LIMITE_MUNDO 150.0f
#define RADIO_IMPACTO 3.0f          // radio de colisión de los misiles
#define RADIO_DESPEJADO_ORIGEN 10.0f // zona sin obstáculos alrededor del jugador inicial
#define DISTANCIA_ENEMIGO 50.0f
#define VIDAS_INICIALES 4
#define PUNTOS_POR_ENEMIGO 1000
#define VELOCIDAD_TORRETA 0.5f      // rad/s con los que el enemigo apunta
#define UMBRAL_PUNTERIA 0.1f        // rad: dispara si el jugador queda a menos de esto
#define MAX_INTENTOS_SPAWN 1000

struct mundo {
    tanque_t *jugador;
    tanque_t *enemigo;
    obstaculo_t *obstaculos[NUM_OBSTACULOS];
    size_t num_obstaculos;

    int puntaje;
    bool terminado;

    // Eventos de un solo uso para las animaciones
    bool ev_jugador_impactado;
    bool ev_enemigo_destruido;
    float ev_x, ev_y;
};

static float normalizar_angulo(float a) {
    while (a > (float)M_PI) a -= 2 * (float)M_PI;
    while (a <= -(float)M_PI) a += 2 * (float)M_PI;
    return a;
}

static float angulo_aleatorio(void) {
    return ((float)rand() / (float)RAND_MAX) * 2 * (float)M_PI - (float)M_PI;
}

static float coordenada_aleatoria(void) {
    return ((float)rand() / (float)RAND_MAX) * 2 * LIMITE_MUNDO - LIMITE_MUNDO;
}

static float distancia2(float x0, float y0, float x1, float y1) {
    float dx = x1 - x0, dy = y1 - y0;
    return dx * dx + dy * dy;
}

static bool posicion_choca_obstaculo(const mundo_t *m, float x, float y, float radio) {
    for (size_t i = 0; i < m->num_obstaculos; i++) {
        if (distancia2(x, y, obstaculo_x(m->obstaculos[i]),
                       obstaculo_y(m->obstaculos[i])) < radio * radio)
            return true;
    }
    return false;
}

// Crea un enemigo a 50 metros del jugador en una dirección al azar,
// reintentando hasta encontrar una posición libre de obstáculos.
static tanque_t *spawn_enemigo(mundo_t *m) {
    for (int i = 0; i < MAX_INTENTOS_SPAWN; i++) {
        float ang = angulo_aleatorio();
        float x = tanque_x(m->jugador) + DISTANCIA_ENEMIGO * cosf(ang);
        float y = tanque_y(m->jugador) + DISTANCIA_ENEMIGO * sinf(ang);
        tanque_t *e = crear_tanque_enemigo(x, y, angulo_aleatorio(), 1,
                                           m->obstaculos, m->num_obstaculos);
        if (e) return e;
    }
    return NULL;
}

mundo_t *mundo_crear(lista_t *lista_modelos) {
    mundo_t *m = calloc(1, sizeof(mundo_t));
    if (!m) return NULL;

    // Formas disponibles para los obstáculos
    const char *nombres[] = {"CUBO1", "CUBO2", "CUBO3",
                             "PIRAMIDE1", "PIRAMIDE2", "PIRAMIDE3"};
    const modelo_t *formas[6];
    size_t nformas = 0;
    for (size_t i = 0; i < 6; i++) {
        modelo_t *mod = modelo_buscar(lista_modelos, nombres[i]);
        if (mod) formas[nformas++] = mod;
    }

    for (size_t i = 0; nformas > 0 && i < NUM_OBSTACULOS; i++) {
        float x, y;
        do {
            x = coordenada_aleatoria();
            y = coordenada_aleatoria();
        } while (distancia2(x, y, 0, 0) <
                 RADIO_DESPEJADO_ORIGEN * RADIO_DESPEJADO_ORIGEN);
        obstaculo_t *o = obstaculo_crear(x, y, angulo_aleatorio(),
                                         formas[rand() % nformas]);
        if (o) m->obstaculos[m->num_obstaculos++] = o;
    }

    m->jugador = tanque_crear(0, 0, (float)M_PI / 2, VIDAS_INICIALES);
    if (!m->jugador) {
        mundo_destruir(m);
        return NULL;
    }
    tanque_asignar_obstaculos(m->jugador, m->obstaculos, m->num_obstaculos);

    m->enemigo = spawn_enemigo(m);
    if (!m->enemigo) {
        mundo_destruir(m);
        return NULL;
    }
    return m;
}

void mundo_destruir(mundo_t *m) {
    if (!m) return;
    tanque_destruir(m->jugador);
    tanque_destruir(m->enemigo);
    for (size_t i = 0; i < m->num_obstaculos; i++)
        obstaculo_destruir(m->obstaculos[i]);
    free(m);
}

void mundo_actualizar(mundo_t *m, float dt) {
    if (m->terminado) return;

    tanque_actualizar(m->jugador, dt);
    tanque_actualizar(m->enemigo, dt);

    float px = tanque_x(m->jugador), py = tanque_y(m->jugador);
    float ex = tanque_x(m->enemigo), ey = tanque_y(m->enemigo);

    // Puntería del enemigo: gira la torreta hacia el jugador y dispara
    // cuando la desviación es menor a 0.1 radianes
    float objetivo = atan2f(py - ey, px - ex);
    float error = normalizar_angulo(objetivo - tanque_phi(m->enemigo) -
                                    tanque_torreta(m->enemigo));
    float paso = VELOCIDAD_TORRETA * dt;
    float giro = error;
    if (giro > paso) giro = paso;
    else if (giro < -paso) giro = -paso;
    tanque_girar_torreta(m->enemigo, giro);

    error = normalizar_angulo(objetivo - tanque_phi(m->enemigo) -
                              tanque_torreta(m->enemigo));
    if (fabsf(error) < UMBRAL_PUNTERIA && tanque_puede_disparar(m->enemigo))
        tanque_disparar(m->enemigo);

    // Misil del jugador: contra el enemigo o contra obstáculos
    if (tanque_misil_activo(m->jugador)) {
        float mx = tanque_misil_x(m->jugador), my = tanque_misil_y(m->jugador);
        if (distancia2(mx, my, ex, ey) < RADIO_IMPACTO * RADIO_IMPACTO) {
            tanque_misil_terminar(m->jugador);
            m->puntaje += PUNTOS_POR_ENEMIGO;
            m->ev_enemigo_destruido = true;
            m->ev_x = ex;
            m->ev_y = ey;
            tanque_destruir(m->enemigo);
            m->enemigo = spawn_enemigo(m);
            if (!m->enemigo) m->terminado = true; // sin lugar donde aparecer
        } else if (posicion_choca_obstaculo(m, mx, my, RADIO_IMPACTO)) {
            tanque_misil_terminar(m->jugador);
        }
    }

    // Misil del enemigo: contra el jugador o contra obstáculos
    if (m->enemigo && tanque_misil_activo(m->enemigo)) {
        float mx = tanque_misil_x(m->enemigo), my = tanque_misil_y(m->enemigo);
        if (distancia2(mx, my, px, py) < RADIO_IMPACTO * RADIO_IMPACTO) {
            tanque_misil_terminar(m->enemigo);
            tanque_recibir_impacto(m->jugador);
            m->ev_jugador_impactado = true;
            if (tanque_vidas(m->jugador) <= 0) m->terminado = true;
        } else if (posicion_choca_obstaculo(m, mx, my, RADIO_IMPACTO)) {
            tanque_misil_terminar(m->enemigo);
        }
    }
}

tanque_t *mundo_jugador(const mundo_t *m) { return m->jugador; }
tanque_t *mundo_enemigo(const mundo_t *m) { return m->enemigo; }
size_t mundo_num_obstaculos(const mundo_t *m) { return m->num_obstaculos; }
obstaculo_t *mundo_obstaculo(const mundo_t *m, size_t i) { return m->obstaculos[i]; }

int mundo_puntaje(const mundo_t *m) { return m->puntaje; }
int mundo_vidas(const mundo_t *m) { return tanque_vidas(m->jugador); }
bool mundo_terminado(const mundo_t *m) { return m->terminado; }

bool mundo_evento_jugador_impactado(mundo_t *m) {
    bool e = m->ev_jugador_impactado;
    m->ev_jugador_impactado = false;
    return e;
}

bool mundo_evento_enemigo_destruido(mundo_t *m, float *x, float *y) {
    if (!m->ev_enemigo_destruido) return false;
    m->ev_enemigo_destruido = false;
    *x = m->ev_x;
    *y = m->ev_y;
    return true;
}
