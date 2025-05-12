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
    
    while (currentState != STATE_EXIT) {
        switch (currentState) {
            case STATE_MENU:
                currentState = run_menu();
                break;
                
            case STATE_GAME:
                currentState = run_game();
                break;
                
            default:
                currentState = STATE_EXIT;
                break;
        }
    }

    TTF_Quit();
    Mix_CloseAudio();
    SDL_Quit();
    
    return 0;
}