#include "fonction_p.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    SDL_Surface *ecran = NULL;
    SDL_Event event;
    int quitter = 0;
    int hoveredIndex = -1;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Erreur init SDL : %s\n", SDL_GetError());
        return 1;
    }

    ecran = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!ecran) {
        fprintf(stderr, "Erreur mode vidéo : %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "Erreur audio : %s\n", Mix_GetError());
        SDL_Quit();
        return 1;
    }

    Menu menu;
    init_menu(&menu);
    Mix_PlayMusic(menu.bgMusic, -1);

    while (!quitter) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quitter = 1;
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
                    if (event.button.button == SDL_BUTTON_LEFT && hoveredIndex == 4) {
                        quitter = 1;
                    }
                    break;
            }
        }

        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));
        afficher_menu(menu, ecran, hoveredIndex);
        SDL_Delay(16);
    }

    liberer_menu(&menu);
    Mix_CloseAudio();
    SDL_Quit();
    return 0;
}

