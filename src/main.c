/*
 * main.c — Punto de entrada y loop principal del Game of Life.
 *
 * Este archivo orquesta todos los modulos del programa:
 *   1. Parsea argumentos de linea de comandos para configurar la simulacion.
 *   2. Inicializa SDL2 y crea las estructuras Game y Renderer.
 *   3. Carga un patron predefinido o genera un grid aleatorio.
 *   4. Ejecuta el loop principal: eventos → simulacion → rendering → delay.
 *   5. Limpia todos los recursos al salir.
 *
 * Controles interactivos:
 *   SPACE — Pausar / reanudar la simulacion.
 *   R     — Regenerar el grid con celdas aleatorias.
 *   +/=   — Aumentar la velocidad (FPS target + 2).
 *   -     — Disminuir la velocidad (FPS target - 2).
 *   ESC   — Salir del programa.
 */

#include <stdio.h>   /* fprintf, stderr */
#include <stdlib.h>  /* atoi, atof, srand, EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h>  /* strcmp */
#include <time.h>    /* time, para semilla de rand */
#include <SDL.h>     /* SDL_Init, SDL_Quit, SDL_Event, SDL_Delay, etc. */
#include "game.h"
#include "render.h"
#include "patterns.h"

/*
 * usage — Imprime las opciones de linea de comandos en stderr.
 *
 * Se invoca cuando el usuario pasa --help / -h o un argumento invalido.
 * Documenta cada opcion con su valor por defecto.
 */
static void usage(const char *prog) {
    fprintf(stderr, "Usage: %s [options]\n", prog);
    fprintf(stderr, "  --width N       Grid width (default 80)\n");
    fprintf(stderr, "  --height N      Grid height (default 60)\n");
    fprintf(stderr, "  --cell-size N   Pixel size per cell (default 10)\n");
    fprintf(stderr, "  --pattern NAME  Pattern: random, glider, blinker, toad, beacon, pulsar, gosper (default random)\n");
    fprintf(stderr, "  --density F     Random fill density 0.0-1.0 (default 0.3)\n");
    fprintf(stderr, "  --fps N         Target FPS (default 10)\n");
}

/*
 * main — Funcion principal del programa.
 *
 * Flujo de ejecucion:
 *   1. Parseo de argumentos con un loop sobre argv.
 *   2. Inicializacion de la semilla aleatoria con time(NULL).
 *   3. Inicializacion de SDL2 (solo subsistema de video).
 *   4. Creacion del Game (logica) y Renderer (grafico).
 *   5. Carga del patron inicial o randomizacion.
 *   6. Loop principal con control de FPS por frame timing.
 *   7. Cleanup de recursos en orden inverso a la creacion.
 */
int main(int argc, char *argv[]) {
    /* Valores por defecto de configuracion */
    int grid_w = 80;           /* Ancho del grid en celdas */
    int grid_h = 60;           /* Alto del grid en celdas */
    int cell_size = 10;        /* Pixeles por celda */
    const char *pattern_name = "random";  /* Patron inicial */
    float density = 0.3f;      /* Densidad para randomizacion (30%) */
    int target_fps = 10;       /* Generaciones por segundo objetivo */
    int i;

    /*
     * Parseo de argumentos de linea de comandos.
     *
     * Cada opcion tiene formato "--nombre valor". Se verifica que
     * haya un argumento siguiente (i + 1 < argc) antes de consumirlo
     * con ++i. atoi/atof convierten el string a numero.
     *
     * Los argumentos desconocidos provocan un mensaje de error y
     * la impresion del uso, retornando codigo de error 1.
     */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--width") == 0 && i + 1 < argc) {
            grid_w = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--height") == 0 && i + 1 < argc) {
            grid_h = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--cell-size") == 0 && i + 1 < argc) {
            cell_size = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--pattern") == 0 && i + 1 < argc) {
            pattern_name = argv[++i];
        } else if (strcmp(argv[i], "--density") == 0 && i + 1 < argc) {
            density = (float)atof(argv[++i]);
        } else if (strcmp(argv[i], "--fps") == 0 && i + 1 < argc) {
            target_fps = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            usage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            usage(argv[0]);
            return 1;
        }
    }

    /* Clamping del FPS target al rango [1, 60] */
    if (target_fps < 1) target_fps = 1;
    if (target_fps > 60) target_fps = 60;

    /*
     * Semilla del generador aleatorio.
     * time(NULL) retorna los segundos desde epoch, proporcionando
     * una semilla diferente en cada ejecucion. El cast a unsigned
     * satisface la firma de srand().
     */
    srand((unsigned)time(NULL));

    /*
     * Inicializacion de SDL2.
     * SDL_INIT_VIDEO habilita el subsistema de video (ventanas, rendering).
     * No se necesita audio ni otros subsistemas para esta aplicacion.
     * SDL_GetError() proporciona un mensaje descriptivo si falla.
     */
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    /* Creacion de la estructura Game con las dimensiones configuradas */
    Game *game = game_create(grid_w, grid_h);
    if (!game) {
        fprintf(stderr, "Failed to create game\n");
        SDL_Quit();
        return 1;
    }

    /* Creacion de la ventana y renderer SDL2 */
    Renderer *renderer = renderer_create(grid_w, grid_h, cell_size);
    if (!renderer) {
        fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
        game_destroy(game);
        SDL_Quit();
        return 1;
    }

    /*
     * Carga del estado inicial.
     *
     * Si el patron es "random", se llena el grid aleatoriamente con
     * la densidad especificada. De lo contrario, se intenta resolver
     * el nombre del patron con pattern_from_name. Si el nombre no es
     * valido, se cae al modo aleatorio con un aviso en stderr.
     *
     * Los patrones se colocan en grid_w/4, grid_h/4 para centrarlos
     * aproximadamente en el primer cuadrante, dejando espacio para
     * que se expandan.
     */
    if (strcmp(pattern_name, "random") == 0) {
        game_randomize(game, density);
    } else {
        PatternType pt;
        if (pattern_from_name(pattern_name, &pt)) {
            game_clear(game);
            pattern_load(game, pt, grid_w / 4, grid_h / 4);
        } else {
            fprintf(stderr, "Unknown pattern: %s, using random\n", pattern_name);
            game_randomize(game, density);
        }
    }

    /* Variables de estado del loop principal */
    int running = 1;        /* Flag de ejecucion: 0 para salir del loop */
    int paused = 0;         /* Flag de pausa: 1 detiene la simulacion */
    int generation = 0;     /* Contador de generaciones transcurridas */

    /*
     * frame_delay: milisegundos por frame para alcanzar el FPS target.
     * Ejemplo: 10 FPS → 1000/10 = 100ms por frame.
     * Uint32 es el tipo de SDL para timestamps en milisegundos.
     */
    Uint32 frame_delay = 1000 / target_fps;

    /*
     * Loop principal de la aplicacion.
     *
     * Cada iteracion constituye un frame y sigue este pipeline:
     *   1. Registrar el timestamp de inicio del frame.
     *   2. Procesar todos los eventos SDL pendientes (input, cierre).
     *   3. Si no esta en pausa, avanzar la simulacion una generacion.
     *   4. Renderizar el estado actual del grid.
     *   5. Actualizar el HUD con la informacion del estado.
     *   6. Calcular el tiempo consumido y esperar el restante para
     *      mantener el FPS target constante.
     */
    while (running) {
        /* Timestamp de inicio para el control de frame rate */
        Uint32 frame_start = SDL_GetTicks();
        SDL_Event event;

        /*
         * Procesamiento de eventos SDL.
         *
         * SDL_PollEvent extrae eventos de la cola uno por uno sin bloquear.
         * Se procesan todos los pendientes antes de continuar con la
         * simulacion y el rendering.
         */
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    /* El usuario cerro la ventana (boton X o Cmd+Q) */
                    running = 0;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            /* ESC: salir de la aplicacion */
                            running = 0;
                            break;
                        case SDLK_SPACE:
                            /* SPACE: toggle pausa/reanudar */
                            paused = !paused;
                            break;
                        case SDLK_r:
                            /* R: regenerar grid aleatorio y resetear contador */
                            game_randomize(game, density);
                            generation = 0;
                            break;
                        case SDLK_PLUS:
                        case SDLK_EQUALS:
                            /*
                             * +/=: incrementar FPS target.
                             * Se usa SDLK_EQUALS porque en la mayoria de teclados
                             * el + esta en la misma tecla que = (sin shift).
                             * Se recalcula frame_delay inmediatamente.
                             */
                            if (target_fps < 60) {
                                target_fps += 2;
                                frame_delay = 1000 / target_fps;
                            }
                            break;
                        case SDLK_MINUS:
                            /*
                             * -: decrementar FPS target.
                             * Se asegura que no baje de 1 FPS (minimo funcional).
                             */
                            if (target_fps > 1) {
                                target_fps -= 2;
                                if (target_fps < 1) target_fps = 1;
                                frame_delay = 1000 / target_fps;
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
        }

        /* Avanzar la simulacion solo si no esta en pausa */
        if (!paused) {
            game_step(game);
            generation++;
        }

        /* Renderizar el frame actual y actualizar el HUD */
        renderer_draw(renderer, game);
        renderer_draw_hud(renderer, generation, paused, target_fps);

        /*
         * Control de frame rate.
         *
         * Se calcula cuanto tiempo tomo procesar este frame (frame_time).
         * Si fue menor que frame_delay, SDL_Delay duerme el hilo el
         * tiempo restante. Esto produce un frame rate estable cercano
         * al FPS target sin consumir CPU innecesariamente.
         *
         * Nota: SDL_Delay tiene una precision de ~1ms dependiendo del OS.
         * Para aplicaciones criticas se usaria un busy-wait, pero para
         * esta simulacion la precision de SDL_Delay es adecuada.
         */
        Uint32 frame_time = SDL_GetTicks() - frame_start;
        if (frame_time < frame_delay) {
            SDL_Delay(frame_delay - frame_time);
        }
    }

    /*
     * Cleanup de recursos en orden inverso a la creacion.
     * Primero el renderer (depende de SDL), luego el game (independiente),
     * finalmente SDL_Quit que cierra todos los subsistemas SDL.
     */
    renderer_destroy(renderer);
    game_destroy(game);
    SDL_Quit();
    return 0;
}
