/*
 * patterns.h — Interfaz del modulo de patrones predefinidos.
 *
 * Define un enum PatternType con los patrones clasicos del Game of Life
 * y expone funciones para cargarlos en el grid en una posicion arbitraria.
 *
 * Los patrones incluidos cubren las tres categorias fundamentales:
 *   - Still lifes (vidas estaticas): no incluidos, triviales.
 *   - Oscillators (osciladores): blinker, toad, beacon, pulsar.
 *   - Spaceships (naves): glider.
 *   - Guns (canones): Gosper Glider Gun, el primer patron infinito descubierto.
 */

#ifndef PATTERNS_H
#define PATTERNS_H

#include "game.h"

/*
 * PatternType — Enumeracion de los patrones disponibles.
 *
 * PATTERN_GLIDER     — Nave mas pequenia, se desplaza diagonalmente (periodo 4).
 * PATTERN_BLINKER    — Oscilador mas simple, periodo 2, tres celdas en linea.
 * PATTERN_TOAD       — Oscilador periodo 2, dos filas de tres celdas desfasadas.
 * PATTERN_BEACON     — Oscilador periodo 2, dos bloques diagonales que parpadean.
 * PATTERN_PULSAR     — Oscilador periodo 3, simetria cuadruple, 48 celdas vivas.
 * PATTERN_GOSPER_GUN — Canon de Bill Gosper (1970), emite un glider cada 30 gen.
 */
typedef enum {
    PATTERN_GLIDER,
    PATTERN_BLINKER,
    PATTERN_TOAD,
    PATTERN_BEACON,
    PATTERN_PULSAR,
    PATTERN_GOSPER_GUN
} PatternType;

/*
 * pattern_load — Coloca el patron especificado en la posicion (x, y) del grid.
 * Las coordenadas (x, y) corresponden a la esquina superior izquierda
 * del bounding box del patron. Las celdas fuera del grid se ignoran
 * gracias a la verificacion de limites en game_set_cell.
 */
void pattern_load(Game *g, PatternType type, int x, int y);

/*
 * pattern_from_name — Convierte un nombre de patron (string) a PatternType.
 * Retorna 1 si el nombre es valido y escribe el tipo en *out.
 * Retorna 0 si el nombre no coincide con ningun patron conocido.
 * Acepta variantes como "gosper" y "gosper_gun" para el canon.
 */
int pattern_from_name(const char *name, PatternType *out);

#endif
