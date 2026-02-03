/*
 * render.c — Implementacion del rendering SDL2 para el Game of Life.
 *
 * Responsable de toda la interaccion con SDL2 para la salida visual.
 * El pipeline de rendering por frame es:
 *   1. Limpiar el backbuffer con el color de fondo.
 *   2. Dibujar las celdas vivas como rectangulos solidos.
 *   3. Dibujar las lineas del grid (si cell_size >= 4px).
 *   4. Presentar el backbuffer (SDL_RenderPresent).
 *
 * El renderer usa aceleracion por hardware (SDL_RENDERER_ACCELERATED),
 * delegando las operaciones de dibujo a la GPU cuando esta disponible.
 */

#include <stdio.h>   /* snprintf */
#include "render.h"

/*
 * renderer_create — Inicializa la ventana y el renderer SDL2.
 *
 * 1. Aloca la estructura Renderer con malloc.
 * 2. Almacena las dimensiones del grid y el tamanio de celda.
 * 3. Calcula el tamanio de la ventana en pixeles (grid * cell_size).
 * 4. Crea la ventana SDL2 centrada en la pantalla con SDL_WINDOW_SHOWN
 *    para que sea visible inmediatamente.
 * 5. Crea el renderer con SDL_RENDERER_ACCELERATED para usar GPU.
 *    El indice -1 indica que SDL elija el primer driver disponible.
 * 6. Si cualquier paso falla, limpia los recursos previos y retorna NULL.
 */
Renderer *renderer_create(int grid_w, int grid_h, int cell_size) {
    Renderer *r = malloc(sizeof(Renderer));
    if (!r) return NULL;
    r->cell_size = cell_size;
    r->grid_w = grid_w;
    r->grid_h = grid_h;
    int win_w = grid_w * cell_size;
    int win_h = grid_h * cell_size;
    r->window = SDL_CreateWindow(
        "Game of Life",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        win_w, win_h, SDL_WINDOW_SHOWN
    );
    if (!r->window) {
        free(r);
        return NULL;
    }
    r->renderer = SDL_CreateRenderer(r->window, -1, SDL_RENDERER_ACCELERATED);
    if (!r->renderer) {
        SDL_DestroyWindow(r->window);
        free(r);
        return NULL;
    }
    return r;
}

/*
 * renderer_destroy — Libera todos los recursos SDL2 y la estructura.
 *
 * El orden de destruccion importa: primero el renderer (que depende
 * de la ventana), luego la ventana, y finalmente la estructura.
 * Las verificaciones de NULL previenen crashes con punteros invalidos.
 */
void renderer_destroy(Renderer *r) {
    if (!r) return;
    if (r->renderer) SDL_DestroyRenderer(r->renderer);
    if (r->window) SDL_DestroyWindow(r->window);
    free(r);
}

/*
 * renderer_draw — Renderiza un frame completo del estado del juego.
 *
 * Paso 1: Limpiar fondo.
 *   SDL_SetRenderDrawColor establece el color de dibujo a gris oscuro
 *   (R=20, G=20, B=20, A=255). SDL_RenderClear llena todo el
 *   backbuffer con este color.
 *
 * Paso 2: Dibujar celdas vivas.
 *   Se cambia el color a verde (R=0, G=200, B=0) y se itera sobre
 *   todo el grid. Para cada celda viva, se crea un SDL_Rect con:
 *     - Posicion: (x * cell_size, y * cell_size)
 *     - Tamanio: (cell_size - 1, cell_size - 1)
 *   El -1 en el tamanio deja un pixel de separacion entre celdas,
 *   creando un efecto visual de grid sin lineas explicitas.
 *   SDL_RenderFillRect dibuja el rectangulo solido.
 *
 * Paso 3: Lineas del grid (condicional).
 *   Solo se dibujan si cell_size >= 4 pixeles, ya que en tamanios
 *   menores las lineas saturarian visualmente la imagen.
 *   Se usa gris medio (R=40, G=40, B=40) para lineas sutiles.
 *   SDL_RenderDrawLine traza lineas verticales y horizontales
 *   que delimitan cada celda del grid.
 *
 * Paso 4: Presentar.
 *   SDL_RenderPresent intercambia el backbuffer con el frontbuffer,
 *   mostrando el frame completo en la ventana. SDL2 usa double
 *   buffering internamente para evitar flickering.
 */
void renderer_draw(Renderer *r, Game *g) {
    int x, y;
    int cs = r->cell_size;

    /* Paso 1: fondo gris oscuro */
    SDL_SetRenderDrawColor(r->renderer, 20, 20, 20, 255);
    SDL_RenderClear(r->renderer);

    /* Paso 2: celdas vivas en verde */
    SDL_SetRenderDrawColor(r->renderer, 0, 200, 0, 255);
    for (y = 0; y < g->height; y++) {
        for (x = 0; x < g->width; x++) {
            if (game_get_cell(g, x, y)) {
                SDL_Rect rect = { x * cs, y * cs, cs - 1, cs - 1 };
                SDL_RenderFillRect(r->renderer, &rect);
            }
        }
    }

    /* Paso 3: lineas del grid, solo si las celdas son >= 4px */
    if (cs >= 4) {
        SDL_SetRenderDrawColor(r->renderer, 40, 40, 40, 255);
        for (x = 0; x <= g->width; x++) {
            SDL_RenderDrawLine(r->renderer, x * cs, 0, x * cs, g->height * cs);
        }
        for (y = 0; y <= g->height; y++) {
            SDL_RenderDrawLine(r->renderer, 0, y * cs, g->width * cs, y * cs);
        }
    }

    /* Paso 4: presentar el frame */
    SDL_RenderPresent(r->renderer);
}

/*
 * renderer_draw_hud — Muestra informacion del estado en el titulo de ventana.
 *
 * Construye un string con snprintf que incluye:
 *   - Numero de generacion actual.
 *   - FPS target configurado.
 *   - Indicador "PAUSED" si la simulacion esta pausada.
 *
 * Se usa el titulo de la ventana (SDL_SetWindowTitle) como HUD ligero
 * para evitar la dependencia adicional de SDL2_ttf, que requeriria
 * cargar fuentes y gestionar texturas de texto.
 *
 * El buffer de 128 bytes es mas que suficiente para el formato usado.
 */
void renderer_draw_hud(Renderer *r, int generation, int paused, int fps) {
    char title[128];
    snprintf(title, sizeof(title), "Game of Life | Gen: %d | FPS: %d%s",
             generation, fps, paused ? " | PAUSED" : "");
    SDL_SetWindowTitle(r->window, title);
}
