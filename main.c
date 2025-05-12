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
    int current_score = 0;
    
    while (currentState != STATE_EXIT) {
        switch (currentState) {
            case STATE_MENU:
                currentState = run_menu();
                break;
                
            case STATE_GAME:
                currentState = run_game();
                if (currentState == STATE_PUZZLE) {
                    // Pass current score to next level
                    // We might want to calculate this score based on game performance
                    current_score = 100;
                }
                break;
                
            case STATE_PUZZLE:
                currentState = run_puzzle();
                if (currentState == STATE_LEVEL2) {
                    // Add score for completing puzzle
                    current_score += 200;
                }
                break;
                
            case STATE_LEVEL2:
                currentState = run_level2();
                if (currentState == STATE_LEVEL3) {
                    // Add score for completing level 2
                    current_score += 300;
                }
                break;
                
            case STATE_LEVEL3:
                currentState = run_level3();
                if (currentState == STATE_HIGHSCORE) {
                    // Add score for completing level 3
                    current_score += 400;
                }
                break;
                
            case STATE_HIGHSCORE:
                currentState = run_highscore(current_score);
                // Reset score if returning to menu
                if (currentState == STATE_MENU) {
                    current_score = 0;
                }
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