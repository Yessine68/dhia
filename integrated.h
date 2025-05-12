#ifndef INTEGRATED_H
#define INTEGRATED_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include "fonction_p.h"
#include "perso.h"

// Game states
typedef enum {
    STATE_MENU,
    STATE_GAME,
    STATE_EXIT
} GameState;

// Function to run the menu
GameState run_menu();

// Function to run the game
GameState run_game();

#endif