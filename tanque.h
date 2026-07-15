#ifndef TANQUE_H
#define TANQUE_H

#include <stdbool.h>
#include <math.h>
#include "lista.h"
#include "matriz.h"

/* --- Constantes del Tanque --- */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define V_TANQUE 7          // Velocidad de avance (m/s) [1]
#define WZ_TANQUE 0.36      // Velocidad de giro del cuerpo (rad/s) [1]
#define WZ_TORRETA_ON 0.12  // Velocidad de giro torreta buscando al jugador [1]
#define WZ_TORRETA_OFF 0.24 // Velocidad de giro torreta volviendo a reposo [1]
#define WZ_RADAR (2 * M_PI) // Velocidad de giro del radar [1]
#define T_ACCION 0.5        // Duración de los movimientos del jugador [1]
#define COOLDOWN 2          // Tiempo de enfriamiento del disparo [1, 10]

/* --- Enumerativos para Control y IA --- */
enum tras { TRAS_NONE, TRAS_FWD, TRAS_BWD }; // Movimiento lineal [2]
enum rot { ROT_NONE, ROT_CW, ROT_CCW };     // Rotación del cuerpo [2]
enum turr { TURR_NONE, TURR_ON_CW, TURR_ON_CCW, TURR_OFF }; // Estado torreta [2]
enum acciones_ia { IA_NONE, IA_FWD, IA_BWD, IA_CW, IA_CCW }; // Acciones del enemigo [2]
enum enemy_to { FRONT, LEFT, RIGHT, BACK }; // Posición relativa para el HUD [2]

/* --- Estructura del Tanque --- */
typedef struct tanque {
    float pos[3];       // x, y, z [3]
    float angz;         // Ángulo phi del cuerpo [3, 11]
    float ang_torreta;  // Ángulo relativo de la torreta [3, 10]
    float ang_radar;    // Ángulo relativo del radar [3]
} tanque_t;

/* --- Primitivas de Creación --- */

// Crea un tanque en (x, y) con ángulo angz. Radar y torreta en 0 [4, 12].
tanque_t* crear_tanque(float x, float y, float angz);

// Crea un tanque enemigo a 50m del jugador evitando obstáculos [4, 12, 13].
tanque_t* crear_tanque_enemigo(float x_fp, float y_fp, lista_t* obstaculos);

/* --- Primitivas de Movimiento y Control --- */

// Mueve el tanque validando colisiones con obstáculos y el otro tanque [5, 14].
void tanque_mover(tanque_t* tanque, enum tras tras, lista_t* obstaculos, tanque_t* otro_tanque);

// Rota el cuerpo del tanque y normaliza el ángulo entre -PI y PI [5, 15].
void tanque_rotar(tanque_t* tanque, enum rot rot);

// Actualiza el giro constante del radar [6, 15].
void tanque_radar(tanque_t* tanque);

// Mueve la torreta según su estado (buscando jugador o volviendo a reposo) [6, 16].
void tanque_rotar_torreta(tanque_t* tanque, enum turr turr);

/* --- Lógica de Visión y IA --- */

// Determina si un objeto está en el cono de visión angular de un tanque [7, 17].
bool _en_rango_vision(tanque_t* pov, tanque_t* obj, float low, float high);

// Determina la siguiente acción aleatoria del "firmware de aspiradora" [9, 18, 19].
void decidir_accion_ia(enum acciones_ia* accion, int* t_accion_ia, tanque_t* ia, tanque_t* fp);

// Ejecuta la acción de IA sobre el tanque enemigo [9, 20].
void ia_acciones(tanque_t* ia, enum acciones_ia accion, lista_t* obstaculos, tanque_t* otro_tanque);

/* --- Wrappers de Visión --- */
bool en_rango_vision_sim(tanque_t* pov, tanque_t* obj, float dang); [7, 21]
bool en_rango_vision_ia(tanque_t* pov, tanque_t* obj, float dang, enum turr* turr); [8, 21]
bool en_rango_vision_turr(tanque_t* pov, tanque_t* obj, float low, float high); [8, 21]

#endif // TANQUE_H