/*
 * patterns.c — Implementacion de patrones clasicos del Game of Life.
 *
 * Cada patron se define como un array estatico de coordenadas (x, y)
 * relativas a un origen. La funcion auxiliar set_cells aplica un offset
 * (ox, oy) a cada coordenada y llama a game_set_cell para activar
 * la celda correspondiente.
 *
 * Las coordenadas fueron tomadas de la referencia canonica del
 * LifeWiki. Cada patron se encapsula en una funcion static place_*
 * para mantener la organizacion interna del modulo.
 */

#include <string.h>  /* strcmp */
#include "patterns.h"

/*
 * set_cells — Funcion auxiliar que activa un conjunto de celdas.
 *
 * Recibe un array de pares [x, y] relativos y un offset (ox, oy).
 * Itera sobre las coordenadas y llama a game_set_cell para cada una.
 *
 * El tipo const int (*coords)[2] es un puntero a arrays de 2 enteros,
 * lo que permite pasar arrays bidimensionales de tamanio variable.
 * count indica cuantos pares contiene el array.
 */
static void set_cells(Game *g, int ox, int oy, const int (*coords)[2], int count) {
    int i;
    for (i = 0; i < count; i++) {
        game_set_cell(g, ox + coords[i][0], oy + coords[i][1], 1);
    }
}

/*
 * place_glider — Glider (deslizador), la nave mas pequenia.
 *
 * Patron de 5 celdas que se desplaza una celda en diagonal
 * cada 4 generaciones. Descubierto por Richard Guy en 1970.
 *
 *   .X.     Coordenadas relativas:
 *   ..X     (1,0), (2,1), (0,2), (1,2), (2,2)
 *   XXX
 */
static void place_glider(Game *g, int x, int y) {
    const int c[][2] = {{1,0},{2,1},{0,2},{1,2},{2,2}};
    set_cells(g, x, y, c, 5);
}

/*
 * place_blinker — Oscilador periodo 2, el mas simple.
 *
 * Tres celdas en linea horizontal que alternan a vertical.
 *
 *   XXX     Coordenadas relativas:
 *           (0,0), (1,0), (2,0)
 */
static void place_blinker(Game *g, int x, int y) {
    const int c[][2] = {{0,0},{1,0},{2,0}};
    set_cells(g, x, y, c, 3);
}

/*
 * place_toad — Oscilador periodo 2.
 *
 * Dos filas de tres celdas desfasadas una posicion.
 *
 *   .XXX    Coordenadas relativas:
 *   XXX.    (1,0), (2,0), (3,0), (0,1), (1,1), (2,1)
 */
static void place_toad(Game *g, int x, int y) {
    const int c[][2] = {{1,0},{2,0},{3,0},{0,1},{1,1},{2,1}};
    set_cells(g, x, y, c, 6);
}

/*
 * place_beacon — Oscilador periodo 2.
 *
 * Dos bloques 2x2 posicionados en diagonal que parpadean
 * en sus esquinas internas compartidas.
 *
 *   XX..    Coordenadas relativas:
 *   X...    (0,0), (1,0), (0,1), (3,2), (2,3), (3,3)
 *   ...X
 *   ..XX
 */
static void place_beacon(Game *g, int x, int y) {
    const int c[][2] = {{0,0},{1,0},{0,1},{3,2},{2,3},{3,3}};
    set_cells(g, x, y, c, 6);
}

/*
 * place_pulsar — Oscilador periodo 3.
 *
 * Patron grande (13x13) con simetria cuadruple. Tiene 48 celdas vivas
 * y es uno de los osciladores mas reconocibles del Game of Life.
 * Cada cuadrante contiene una estructura identica de barras y pilares.
 *
 * Las coordenadas se organizan por filas para facilitar la lectura.
 * Filas 0, 5, 7, 12 contienen las barras horizontales.
 * Filas 2-4, 8-10 contienen los pilares verticales.
 */
static void place_pulsar(Game *g, int x, int y) {
    const int c[][2] = {
        /* Barras horizontales superiores (fila 0) */
        {2,0},{3,0},{4,0},{8,0},{9,0},{10,0},
        /* Pilares verticales superiores (filas 2-4) */
        {0,2},{5,2},{7,2},{12,2},
        {0,3},{5,3},{7,3},{12,3},
        {0,4},{5,4},{7,4},{12,4},
        /* Barras horizontales medias-superiores (fila 5) */
        {2,5},{3,5},{4,5},{8,5},{9,5},{10,5},
        /* Barras horizontales medias-inferiores (fila 7) */
        {2,7},{3,7},{4,7},{8,7},{9,7},{10,7},
        /* Pilares verticales inferiores (filas 8-10) */
        {0,8},{5,8},{7,8},{12,8},
        {0,9},{5,9},{7,9},{12,9},
        {0,10},{5,10},{7,10},{12,10},
        /* Barras horizontales inferiores (fila 12) */
        {2,12},{3,12},{4,12},{8,12},{9,12},{10,12}
    };
    set_cells(g, x, y, c, 48);
}

/*
 * place_gosper_gun — Gosper Glider Gun, canon de gliders.
 *
 * Descubierto por Bill Gosper en 1970, fue el primer patron finito
 * que demostro crecimiento indefinido en el Game of Life. Emite
 * un nuevo glider cada 30 generaciones hacia la esquina inferior
 * derecha.
 *
 * El patron ocupa un area de 36x9 celdas y consta de:
 *   - Un bloque 2x2 a la izquierda (celdas 0-1, filas 4-5).
 *   - Una estructura central con forma de diamante (celdas 10-17).
 *   - Una estructura derecha con forma de flecha (celdas 20-24).
 *   - Un bloque 2x2 a la derecha (celdas 34-35, filas 2-3).
 */
static void place_gosper_gun(Game *g, int x, int y) {
    const int c[][2] = {
        /* Bloque izquierdo 2x2 */
        {0,4},{0,5},{1,4},{1,5},
        /* Estructura central: diamante + cola */
        {10,4},{10,5},{10,6},{11,3},{11,7},{12,2},{12,8},{13,2},{13,8},
        {14,5},{15,3},{15,7},{16,4},{16,5},{16,6},{17,5},
        /* Estructura derecha: flecha */
        {20,2},{20,3},{20,4},{21,2},{21,3},{21,4},{22,1},{22,5},
        {24,0},{24,1},{24,5},{24,6},
        /* Bloque derecho 2x2 */
        {34,2},{34,3},{35,2},{35,3}
    };
    set_cells(g, x, y, c, 36);
}

/*
 * pattern_load — Despacha la carga del patron al placer correspondiente.
 *
 * Usa un switch sobre el enum PatternType para llamar a la funcion
 * static place_* adecuada. Las coordenadas (x, y) se pasan como
 * offset de origen al patron.
 */
void pattern_load(Game *g, PatternType type, int x, int y) {
    switch (type) {
        case PATTERN_GLIDER:     place_glider(g, x, y);     break;
        case PATTERN_BLINKER:    place_blinker(g, x, y);    break;
        case PATTERN_TOAD:       place_toad(g, x, y);       break;
        case PATTERN_BEACON:     place_beacon(g, x, y);     break;
        case PATTERN_PULSAR:     place_pulsar(g, x, y);     break;
        case PATTERN_GOSPER_GUN: place_gosper_gun(g, x, y); break;
    }
}

/*
 * pattern_from_name — Traduce un string a PatternType.
 *
 * Compara el nombre recibido contra los identificadores conocidos
 * usando strcmp. Acepta "gosper" y "gosper_gun" como alias para
 * PATTERN_GOSPER_GUN, facilitando su uso desde la linea de comandos.
 *
 * Retorna 1 y escribe en *out si hay match; retorna 0 si no.
 */
int pattern_from_name(const char *name, PatternType *out) {
    if (strcmp(name, "glider") == 0)     { *out = PATTERN_GLIDER;     return 1; }
    if (strcmp(name, "blinker") == 0)    { *out = PATTERN_BLINKER;    return 1; }
    if (strcmp(name, "toad") == 0)       { *out = PATTERN_TOAD;       return 1; }
    if (strcmp(name, "beacon") == 0)     { *out = PATTERN_BEACON;     return 1; }
    if (strcmp(name, "pulsar") == 0)     { *out = PATTERN_PULSAR;     return 1; }
    if (strcmp(name, "gosper") == 0)     { *out = PATTERN_GOSPER_GUN; return 1; }
    if (strcmp(name, "gosper_gun") == 0) { *out = PATTERN_GOSPER_GUN; return 1; }
    return 0;
}
