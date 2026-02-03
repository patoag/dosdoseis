/*
 * game.c — Implementacion de la logica del Game of Life de Conway.
 *
 * Este modulo encapsula toda la mecanica del automata celular:
 * creacion/destruccion del grid, acceso a celdas individuales,
 * conteo de vecinos, avance de generacion y utilidades de
 * inicializacion (randomizar, limpiar).
 *
 * Complejidad por paso: O(width * height) — se evalua cada celda exactamente
 * una vez, con un conteo de vecinos O(1) constante (siempre 8 adyacentes).
 */

#include <stdlib.h>  /* malloc, calloc, free, rand, RAND_MAX */
#include <string.h>  /* memset */
#include "game.h"

/*
 * game_create — Constructor del Game.
 *
 * 1. Aloca la estructura Game con malloc.
 * 2. Calcula el tamanio total del grid (width * height).
 * 3. Aloca ambos buffers con calloc, que inicializa a cero.
 *    calloc(size, sizeof(int)) garantiza que todas las celdas
 *    comienzan muertas sin necesidad de un memset adicional.
 * 4. Si cualquier alocacion falla, libera lo que se haya alocado
 *    y retorna NULL. free(NULL) es seguro segun el estandar C.
 */
Game *game_create(int width, int height) {
    Game *g = malloc(sizeof(Game));
    if (!g) return NULL;
    g->width = width;
    g->height = height;
    int size = width * height;
    g->cells = calloc(size, sizeof(int));
    g->next = calloc(size, sizeof(int));
    if (!g->cells || !g->next) {
        free(g->cells);
        free(g->next);
        free(g);
        return NULL;
    }
    return g;
}

/*
 * game_destroy — Destructor del Game.
 *
 * Libera los dos buffers dinamicos y la estructura misma.
 * La verificacion de NULL al inicio permite llamar game_destroy(NULL)
 * sin riesgo, siguiendo la convencion de free().
 */
void game_destroy(Game *g) {
    if (!g) return;
    free(g->cells);
    free(g->next);
    free(g);
}

/*
 * game_get_cell — Lectura segura de una celda.
 *
 * La verificacion de limites retorna 0 para coordenadas fuera del grid.
 * Esto cumple dos propositos:
 *   1. Evita accesos fuera de rango en el array.
 *   2. Implementa bordes muertos: las celdas virtuales mas alla del
 *      borde siempre estan muertas, lo que simplifica count_neighbors.
 *
 * El mapeo 2D->1D usa row-major order: indice = y * width + x.
 */
int game_get_cell(Game *g, int x, int y) {
    if (x < 0 || x >= g->width || y < 0 || y >= g->height)
        return 0;
    return g->cells[y * g->width + x];
}

/*
 * game_set_cell — Escritura segura de una celda.
 *
 * Normaliza el valor a 0 o 1 mediante el operador ternario (alive ? 1 : 0),
 * asegurando que el grid solo contenga valores binarios.
 * Las coordenadas fuera de rango se ignoran sin error.
 */
void game_set_cell(Game *g, int x, int y, int alive) {
    if (x < 0 || x >= g->width || y < 0 || y >= g->height)
        return;
    g->cells[y * g->width + x] = alive ? 1 : 0;
}

/*
 * count_neighbors — Cuenta las celdas vivas adyacentes a (x, y).
 *
 * Itera sobre las 8 posiciones vecinas usando desplazamientos (dx, dy)
 * en el rango [-1, +1]. Se excluye (0, 0) que es la celda misma.
 *
 * Dado que game_get_cell retorna 0 o 1, la suma directa de sus
 * retornos produce el conteo correcto. Las celdas fuera del grid
 * retornan 0, por lo que los bordes se manejan automaticamente.
 *
 * static: visibilidad limitada a este archivo (enlace interno).
 */
static int count_neighbors(Game *g, int x, int y) {
    int count = 0;
    int dx, dy;
    for (dy = -1; dy <= 1; dy++) {
        for (dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;
            count += game_get_cell(g, x + dx, y + dy);
        }
    }
    return count;
}

/*
 * game_step — Avanza una generacion aplicando las reglas de Conway.
 *
 * Recorre todas las celdas del grid en order row-major. Para cada celda:
 *   - Cuenta sus vecinos vivos con count_neighbors.
 *   - Aplica las 4 reglas de Conway (condensadas en 2 condiciones):
 *       * Celda viva: sobrevive si tiene exactamente 2 o 3 vecinos.
 *       * Celda muerta: nace si tiene exactamente 3 vecinos.
 *   - Escribe el resultado en el buffer next.
 *
 * Al finalizar, intercambia los punteros cells y next mediante una
 * variable temporal. Esto evita copiar width*height enteros y convierte
 * el swap en una operacion O(1) de tres asignaciones de puntero.
 */
void game_step(Game *g) {
    int x, y;
    for (y = 0; y < g->height; y++) {
        for (x = 0; x < g->width; x++) {
            int n = count_neighbors(g, x, y);
            int alive = g->cells[y * g->width + x];
            if (alive) {
                /* Reglas 1-3: viva con 2 o 3 vecinos sobrevive, si no muere */
                g->next[y * g->width + x] = (n == 2 || n == 3) ? 1 : 0;
            } else {
                /* Regla 4: muerta con exactamente 3 vecinos nace */
                g->next[y * g->width + x] = (n == 3) ? 1 : 0;
            }
        }
    }
    /* Swap de punteros: O(1) en lugar de memcpy O(n) */
    int *tmp = g->cells;
    g->cells = g->next;
    g->next = tmp;
}

/*
 * game_randomize — Poblacion aleatoria del grid.
 *
 * Para cada celda genera un float aleatorio en [0, 1) usando
 * rand() / RAND_MAX. Si el valor es menor que density, la celda
 * se marca como viva. Un density de 0.3 produce aproximadamente
 * un 30% de celdas vivas, que es un buen punto de partida para
 * observar patrones emergentes.
 *
 * Nota: rand() no es criptograficamente seguro ni tiene la mejor
 * distribucion, pero es suficiente para este proposito visual.
 */
void game_randomize(Game *g, float density) {
    int x, y;
    for (y = 0; y < g->height; y++) {
        for (x = 0; x < g->width; x++) {
            float r = (float)rand() / (float)RAND_MAX;
            g->cells[y * g->width + x] = (r < density) ? 1 : 0;
        }
    }
}

/*
 * game_clear — Reinicia ambos buffers a cero.
 *
 * Usa memset sobre el tamanio total (width * height * sizeof(int)).
 * Se limpian ambos buffers para evitar que datos residuales del buffer
 * next aparezcan en la siguiente generacion tras un swap.
 */
void game_clear(Game *g) {
    memset(g->cells, 0, g->width * g->height * sizeof(int));
    memset(g->next, 0, g->width * g->height * sizeof(int));
}
