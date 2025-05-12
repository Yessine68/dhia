#include "integrated.h"
#include <stdio.h>
#include <stdlib.h>

static int lastHoveredIndex = -1;

GameState run_menu() {
    SDL_Surface *ecran = NULL;
    SDL_Event event;
    int quit = 0;
    int hoveredIndex = -1;
    GameState nextState = STATE_MENU;

    ecran = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!ecran) {
        fprintf(stderr, "Erreur mode vidéo : %s\n", SDL_GetError());
        return STATE_EXIT;
    }

    Menu menu;
    init_menu(&menu);
    Mix_PlayMusic(menu.bgMusic, -1);

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = 1;
                    nextState = STATE_EXIT;
                    break;

                case SDL_MOUSEMOTION:
                    hoveredIndex = -1;
                    for (int i = 0, buttonY = 300; i < 5; i++) {
                        SDL_Rect buttonRect;

                        if (i == 4) {
                            buttonRect.x = 80;
                            buttonRect.y = SCREEN_HEIGHT - menu.buttons[i]->h - 150;
                        } else {
                            buttonRect.x = SCREEN_WIDTH / 2 - menu.buttons[i]->w / 2;
                            buttonRect.y = buttonY;
                            buttonY += 100;
                        }

                        buttonRect.w = menu.buttons[i]->w;
                        buttonRect.h = menu.buttons[i]->h;

                        if (event.motion.x >= buttonRect.x && event.motion.x <= buttonRect.x + buttonRect.w &&
                            event.motion.y >= buttonRect.y && event.motion.y <= buttonRect.y + buttonRect.h) {
                            hoveredIndex = i;
                        }
                    }
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        // Check if play button (index 0) was clicked
                        if (hoveredIndex == 0) {
                            quit = 1;
                            nextState = STATE_GAME;
                            Mix_HaltMusic(); // Stop menu music before starting game
                        }
                        // Exit button
                        else if (hoveredIndex == 4) {
                            quit = 1;
                            nextState = STATE_EXIT;
                        }
                    }
                    break;
            }
        }

        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));
        afficher_menu(menu, ecran, hoveredIndex);
        SDL_Flip(ecran);
        SDL_Delay(16);
    }

    liberer_menu(&menu);
    SDL_FreeSurface(ecran);
    
    return nextState;
}