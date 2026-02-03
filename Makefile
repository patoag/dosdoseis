# Makefile — Construccion del Game of Life con SDL2.
#
# Compilador: cc (enlace simbolico a clang en macOS o gcc en Linux).
# Estandar: C99 (-std=c99), con warnings completos (-Wall -Wextra).
# SDL2: las flags de compilacion y enlace se obtienen dinamicamente
#       mediante sdl2-config, que resuelve las rutas de instalacion
#       automaticamente (Homebrew en macOS, pkg-config en Linux).
#
# Targets:
#   all   — Compila el binario (target por defecto).
#   run   — Compila (si es necesario) y ejecuta.
#   clean — Elimina el binario compilado.

CC = cc
CFLAGS = -Wall -Wextra -std=c99

# sdl2-config --cflags produce flags como -I/opt/homebrew/include/SDL2
# sdl2-config --libs produce flags como -L/opt/homebrew/lib -lSDL2
SDL_CFLAGS = $(shell sdl2-config --cflags)
SDL_LIBS = $(shell sdl2-config --libs)

# Lista de archivos fuente y nombre del binario resultante
SRC = src/main.c src/game.c src/render.c src/patterns.c
TARGET = game_of_life

# Target por defecto: compilar el binario
all: $(TARGET)

# Regla de compilacion: todos los .c se compilan y enlazan en un solo paso.
# $(CC) $(CFLAGS) $(SDL_CFLAGS) — compila con warnings y headers SDL2.
# -o $@ — nombre del binario de salida ($@ es la variable automatica del target).
# $(SRC) — archivos fuente a compilar.
# $(SDL_LIBS) — flags de enlace de SDL2 (van al final, despues de los .c).
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SDL_CFLAGS) -o $@ $(SRC) $(SDL_LIBS)

# Target de conveniencia: compila si es necesario y ejecuta
run: $(TARGET)
	./$(TARGET)

# Limpieza: elimina el binario
clean:
	rm -f $(TARGET)

# Declaracion de targets que no corresponden a archivos
.PHONY: all clean run
