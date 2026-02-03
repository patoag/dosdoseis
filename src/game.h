/*
 * game.h — Interfaz publica del modulo de logica del Game of Life.
 *
 * Define la estructura Game y las funciones para manipular el automata
 * celular. El diseno utiliza double buffering: dos arrays lineales
 * (cells y next) que se intercambian en cada paso de simulacion,
 * evitando asi la necesidad de copiar memoria entre generaciones.
 *
 * El grid se almacena como un array unidimensional de enteros donde
 * la posicion (x, y) se mapea al indice [y * width + x]. Las celdas
 * fuera de los limites del grid se consideran muertas (bordes no toroidales).
 */

#ifndef GAME_H
#define GAME_H

/*
 * Estructura principal del juego.
 *
 * width  — Numero de columnas del grid.
 * height — Numero de filas del grid.
 * cells  — Buffer actual: array 1D de tamanio width*height.
 *           Cada elemento es 0 (muerta) o 1 (viva).
 * next   — Buffer secundario donde se escribe la siguiente generacion.
 *           Tras cada paso, cells y next se intercambian por puntero.
 */
typedef struct {
    int width;
    int height;
    int *cells;
    int *next;
} Game;

/*
 * game_create — Reserva memoria para un Game con las dimensiones dadas.
 * Retorna NULL si la alocacion falla. Ambos buffers se inicializan a cero
 * mediante calloc, lo que equivale a un grid completamente muerto.
 */
Game *game_create(int width, int height);

/*
 * game_destroy — Libera ambos buffers y la estructura Game.
 * Acepta NULL de forma segura (no-op).
 */
void game_destroy(Game *g);

/*
 * game_step — Avanza la simulacion una generacion.
 * Recorre cada celda, cuenta sus 8 vecinos en el buffer actual,
 * aplica las reglas de Conway y escribe el resultado en el buffer next.
 * Finalmente intercambia los punteros cells y next (swap sin copia).
 */
void game_step(Game *g);

/*
 * game_set_cell — Establece el estado de la celda en (x, y).
 * alive != 0 la marca como viva; alive == 0 como muerta.
 * Las coordenadas fuera de rango se ignoran silenciosamente.
 */
void game_set_cell(Game *g, int x, int y, int alive);

/*
 * game_get_cell — Retorna el estado de la celda en (x, y).
 * Devuelve 0 para coordenadas fuera de rango, lo que implementa
 * implicitamente los bordes muertos del automata.
 */
int game_get_cell(Game *g, int x, int y);

/*
 * game_randomize — Llena el grid con celulas vivas de forma aleatoria.
 * density es un valor entre 0.0 y 1.0 que indica la probabilidad
 * de que cada celda individual este viva. Usa rand() de stdlib.
 */
void game_randomize(Game *g, float density);

/*
 * game_clear — Establece todas las celdas a 0 (muertas) en ambos buffers.
 * Utiliza memset para eficiencia sobre el array completo.
 */
void game_clear(Game *g);

#endif
