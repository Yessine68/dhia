#include <stdio.h>
#include <stdlib.h>
#include "integrated.h"

int main() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
        fprintf(stderr, "Erreur init SDL : %s\n", SDL_GetError());
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "Erreur audio : %s\n", Mix_GetError());
        SDL_Quit();
        return 1;
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "Erreur init TTF : %s\n", TTF_GetError());
        Mix_CloseAudio();
        SDL_Quit();
        return 1;
    }

    GameState currentState = STATE_MENU;
    
    printf("Starting with state: %d\n", currentState);
    
    while (currentState != STATE_EXIT) {
        switch (currentState) {
            case STATE_MENU:
                printf("Entering MENU state\n");
                currentState = run_menu();
                printf("Exited MENU state, next state: %d\n", currentState);
                break;
                
            case STATE_GAME:
                printf("Entering GAME state\n");
                currentState = run_game();
                printf("Exited GAME state, next state: %d\n", currentState);
                break;
                
            case STATE_LEVEL2:
                printf("Entering LEVEL2 state\n");
                currentState = run_level2();
                printf("Exited LEVEL2 state, next state: %d\n", currentState);
                break;
                
            default:
                printf("Entering default case, exiting\n");
                currentState = STATE_EXIT;
                break;
        }
    }

    printf("Cleaning up and exiting\n");
    TTF_Quit();
    Mix_CloseAudio();
    SDL_Quit();
    
    return 0;
}