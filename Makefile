# Compiler and flags
CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer

# Source files
MENU_SRC = fonction_p.c
GAME_SRC = perso.c
INTEGRATED_SRC = main.c menu.c game.c

# Object files
MENU_OBJ = $(MENU_SRC:.c=.o)
GAME_OBJ = $(GAME_SRC:.c=.o)
INTEGRATED_OBJ = $(INTEGRATED_SRC:.c=.o)

# All object files combined
ALL_OBJ = $(MENU_OBJ) $(GAME_OBJ) $(INTEGRATED_OBJ)

# Executable name
EXEC = harmonia

# Default target
all: $(EXEC)

# Linking rule
$(EXEC): $(ALL_OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

# Compilation rule for .c files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Dependencies
main.o: main.c integrated.h
menu.o: menu.c integrated.h fonction_p.h
game.o: game.c integrated.h perso.h
fonction_p.o: fonction_p.c fonction_p.h
perso.o: perso.c perso.h

# Clean target (remove object files)
clean:
	rm -f $(ALL_OBJ)

# Deeper cleaning (remove executable too)
mrproper: clean
	rm -f $(EXEC)

# Phony targets (not actual files)
.PHONY: all clean mrproper