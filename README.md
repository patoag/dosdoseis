# Game of Life

Implementacion del automata celular de John Conway en C con SDL2.

El [Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) es un automata celular ideado por el matematico britanico John Horton Conway en 1970. Opera sobre un grid bidimensional donde cada celda puede estar viva o muerta, y su estado evoluciona en generaciones discretas segun cuatro reglas deterministas basadas unicamente en la cantidad de vecinos vivos.

A pesar de la simplicidad de sus reglas, el Game of Life es Turing-completo: capaz de computar cualquier funcion computable. Esto lo convierte en un objeto de estudio fundamental en teoria de la computacion, matematicas recreativas y sistemas complejos.

## Reglas

Cada celda evalua sus 8 vecinos adyacentes (horizontal, vertical y diagonal):

| Condicion | Resultado |
|---|---|
| Celda viva con menos de 2 vecinos | Muere (soledad) |
| Celda viva con 2 o 3 vecinos | Sobrevive |
| Celda viva con mas de 3 vecinos | Muere (sobrepoblacion) |
| Celda muerta con exactamente 3 vecinos | Nace |

## Compilacion

### Requisitos

- Compilador C con soporte para C99 (gcc, clang)
- [SDL2](https://www.libsdl.org/)

Instalacion de SDL2:

```bash
# macOS
brew install sdl2

# Debian / Ubuntu
sudo apt install libsdl2-dev

# Arch Linux
sudo pacman -S sdl2
```

### Compilar y ejecutar

```bash
make        # Compila el binario game_of_life
make run    # Compila (si es necesario) y ejecuta
make clean  # Elimina el binario
```

## Uso

```bash
./game_of_life [opciones]
```

### Opciones

| Opcion | Descripcion | Default |
|---|---|---|
| `--width N` | Ancho del grid en celdas | 80 |
| `--height N` | Alto del grid en celdas | 60 |
| `--cell-size N` | Tamanio de cada celda en pixeles | 10 |
| `--pattern NAME` | Patron inicial | random |
| `--density F` | Densidad de celdas vivas (0.0 - 1.0) | 0.3 |
| `--fps N` | Generaciones por segundo | 10 |

### Patrones disponibles

| Nombre | Tipo | Descripcion |
|---|---|---|
| `random` | Aleatorio | Poblacion aleatoria segun densidad configurada |
| `glider` | Nave | La nave mas pequenia, se desplaza en diagonal cada 4 generaciones |
| `blinker` | Oscilador (p2) | Tres celdas en linea, alterna entre horizontal y vertical |
| `toad` | Oscilador (p2) | Dos filas de tres celdas desfasadas |
| `beacon` | Oscilador (p2) | Dos bloques 2x2 en diagonal que parpadean |
| `pulsar` | Oscilador (p3) | Patron simetrico de 48 celdas con simetria cuadruple |
| `gosper` | Canon | Gosper Glider Gun, emite un glider cada 30 generaciones |

### Ejemplos

```bash
# Grid aleatorio con configuracion por defecto
./game_of_life

# Gosper Glider Gun en un grid grande
./game_of_life --pattern gosper --width 120 --height 80

# Pulsar centrado con celdas grandes
./game_of_life --pattern pulsar --cell-size 15

# Grid denso y rapido
./game_of_life --density 0.5 --fps 30
```

## Controles

| Tecla | Accion |
|---|---|
| `SPACE` | Pausar / reanudar la simulacion |
| `R` | Regenerar grid aleatorio |
| `+` / `=` | Aumentar velocidad (+2 FPS) |
| `-` | Disminuir velocidad (-2 FPS) |
| `ESC` | Salir |

## Arquitectura

```
src/
├── main.c       Punto de entrada, parseo de argumentos, loop principal SDL2
├── game.c/.h    Logica del automata celular con double buffering
├── render.c/.h  Rendering SDL2: ventana, grid, celdas, HUD
└── patterns.c/.h  Patrones clasicos predefinidos
```

### Decisiones tecnicas

- **Double buffering en la logica**: dos arrays (`cells` y `next`) se intercambian por puntero tras cada generacion, evitando copias de memoria.
- **Grid como array 1D**: el mapeo `[y * width + x]` ofrece localidad de cache superior a un array de punteros (`int **`), relevante en grids grandes.
- **Bordes muertos**: las celdas fuera del grid se consideran muertas. La verificacion de limites en `game_get_cell` simplifica el conteo de vecinos sin casos especiales.
- **HUD en titulo de ventana**: evita la dependencia de SDL2_ttf, manteniendo SDL2 como unica dependencia externa.
- **Frame rate por delay**: `SDL_GetTicks` + `SDL_Delay` proporcionan control de FPS suficiente para esta aplicacion sin necesidad de timers de alta precision.

## Referencias

- Conway, J.H. — [The Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life), 1970.
- Gosper, R.W. — [Gosper Glider Gun](https://conwaylife.com/wiki/Gosper_glider_gun), 1970.
- [LifeWiki](https://conwaylife.com/wiki/Main_Page) — Enciclopedia de patrones del Game of Life.
- [SDL2 Documentation](https://wiki.libsdl.org/SDL2/FrontPage) — Referencia oficial de Simple DirectMedia Layer 2.
