#ifndef INTEGRATED_H
#define INTEGRATED_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include <stdbool.h>
#include "fonction_p.h"
#include "perso.h"
#include "joueur.h"
#include "enemie.h"

// Game states
typedef enum {
    STATE_MENU,
    STATE_GAME,
    STATE_PUZZLE,
    STATE_LEVEL2,
    STATE_LEVEL3,
    STATE_HIGHSCORE,  // New state for high scores
    STATE_EXIT
} GameState;

// Function to run the menu
GameState run_menu();

// Function to run the game
GameState run_game();

// Function to run puzzle mini-game
GameState run_puzzle();

// Function to run level 2 with enemies
GameState run_level2();

// Function to run level 3 with final boss
GameState run_level3();

// Function to run high score screen
GameState run_highscore(int score);

// Shared global score variable
extern int global_score;

#endif