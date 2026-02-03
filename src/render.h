/*
 * render.h — Interfaz del modulo de rendering SDL2.
 *
 * Encapsula la ventana y el renderer de SDL2 en una estructura Renderer,
 * proporcionando funciones de alto nivel para dibujar el estado del
 * Game of Life sin exponer detalles de SDL2 al resto del programa.
 *
 * El modulo maneja:
 *   - Creacion y destruccion de la ventana SDL2.
 *   - Dibujado del grid con celdas vivas coloreadas.
 *   - Lineas de grid sutiles para celdas grandes (>= 4px).
 *   - HUD informativo en el titulo de la ventana.
 */

#ifndef RENDER_H
#define RENDER_H

#include <SDL.h>    /* SDL_Window, SDL_Renderer y tipos SDL */
#include "game.h"   /* Game struct para acceso al estado */

/*
 * Renderer — Encapsula los recursos graficos de SDL2.
 *
 * window    — Puntero a la ventana SDL2 creada.
 * renderer  — Puntero al renderer SDL2 (acelerado por hardware).
 * cell_size — Tamanio en pixeles de cada celda del grid.
 * grid_w    — Ancho del grid en celdas (para calculos de ventana).
 * grid_h    — Alto del grid en celdas.
 *
 * El tamanio de la ventana es grid_w * cell_size x grid_h * cell_size pixeles.
 */
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    int cell_size;
    int grid_w;
    int grid_h;
} Renderer;

/*
 * renderer_create — Crea la ventana SDL2 y su renderer.
 * La ventana se centra en la pantalla y tiene tamanio grid_w * cell_size
 * por grid_h * cell_size pixeles. Usa renderer acelerado por hardware.
 * Retorna NULL si la creacion de ventana o renderer falla.
 */
Renderer *renderer_create(int grid_w, int grid_h, int cell_size);

/*
 * renderer_destroy — Libera el renderer, la ventana y la estructura.
 * Acepta NULL de forma segura.
 */
void renderer_destroy(Renderer *r);

/*
 * renderer_draw — Dibuja el estado actual del Game en la ventana.
 * Limpia el fondo a gris oscuro (20, 20, 20), dibuja las celdas vivas
 * como rectangulos verdes, y opcionalmente las lineas del grid.
 * Llama a SDL_RenderPresent al final para mostrar el frame.
 */
void renderer_draw(Renderer *r, Game *g);

/*
 * renderer_draw_hud — Actualiza el titulo de la ventana con informacion.
 * Muestra la generacion actual, FPS target y estado de pausa.
 * Se usa el titulo de ventana en lugar de texto renderizado para
 * evitar la dependencia de SDL2_ttf.
 */
void renderer_draw_hud(Renderer *r, int generation, int paused, int fps);

#endif
