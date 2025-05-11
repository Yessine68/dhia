#include "fonction_p.h"
#include <stdio.h>
#include <stdlib.h>

int lastHoveredIndex = -1;

void init_menu(Menu *menu) {
    menu->background = IMG_Load("BACK.png");
    if (!menu->background) {
        fprintf(stderr, "Erreur chargement background : %s\n", IMG_GetError());
        exit(1);
    }

    char *buttonPaths[5] = {"play.png", "option.png", "score.png", "history.png", "exit.png"};
    char *buttonHoverPaths[5] = {"playhover.png", "optionhover.png", "scorehover.png", "historyhover.png", "exithover.png"};

    for (int i = 0; i < 5; i++) {
        menu->buttons[i] = IMG_Load(buttonPaths[i]);
        menu->buttons_hover[i] = IMG_Load(buttonHoverPaths[i]);

        if (!menu->buttons[i] || !menu->buttons_hover[i]) {
            fprintf(stderr, "Erreur chargement bouton %d : %s\n", i, IMG_GetError());
            exit(1);
        }
    }

    menu->bgMusic = Mix_LoadMUS("Epic and Dramatic Trailer Music.mp3");
    if (!menu->bgMusic) {
        fprintf(stderr, "Erreur chargement musique : %s\n", Mix_GetError());
        exit(1);
    }

    menu->hoverSound = Mix_LoadWAV("/home/nourhene/harmonia/Click-Sound-Effect.wav");
    if (!menu->hoverSound) {
        fprintf(stderr, "Erreur chargement son hover : %s\n", Mix_GetError());
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "Erreur init TTF : %s\n", TTF_GetError());
        exit(1);
    }

    menu->font = TTF_OpenFont("bubble.ttf", 80); // bigger font for top-center
    if (!menu->font) {
        fprintf(stderr, "Erreur chargement police : %s\n", TTF_GetError());
        exit(1);
    }

    menu->textColor = (SDL_Color){255, 182, 193, 0}; // pastel pink

    menu->logo = IMG_Load("Fichier 3logo.png");
    if (!menu->logo) {
        fprintf(stderr, "Erreur chargement logo : %s\n", IMG_GetError());
    }
    menu->poslogo.x = SCREEN_WIDTH - 390;
    menu->poslogo.y = 20;
}

void afficher_menu(Menu menu, SDL_Surface *ecran, int hoveredIndex) {
    SDL_BlitSurface(menu.background, NULL, ecran, NULL);

    // Top-center "HARMONIA"
    SDL_Surface *titleSurface = TTF_RenderText_Solid(menu.font, "HARMONIA", menu.textColor);
    if (titleSurface) {
        SDL_Rect titlePosition;
        titlePosition.x = (SCREEN_WIDTH - titleSurface->w) / 2;
        titlePosition.y = 50;  // top
        SDL_BlitSurface(titleSurface, NULL, ecran, &titlePosition);
        SDL_FreeSurface(titleSurface);
    }

    SDL_BlitSurface(menu.logo, NULL, ecran, &menu.poslogo);

    int buttonY = 300;
    for (int i = 0; i < 5; i++) {
        SDL_Surface *buttonToDraw = (i == hoveredIndex) ? menu.buttons_hover[i] : menu.buttons[i];
        SDL_Rect buttonPosition;

        if (i == 4) {
            // Exit button on bottom-left
            buttonPosition.x = 80;
            buttonPosition.y = SCREEN_HEIGHT - buttonToDraw->h - 150;
        } else {
            // Others center
            buttonPosition.x = SCREEN_WIDTH / 2 - buttonToDraw->w / 2;
            buttonPosition.y = buttonY;
            buttonY += 100;
        }

        SDL_BlitSurface(buttonToDraw, NULL, ecran, &buttonPosition);

        if (i == hoveredIndex && i != lastHoveredIndex) {
            Mix_PlayChannel(-1, menu.hoverSound, 0);
            lastHoveredIndex = i;
        }
    }

    SDL_Flip(ecran);
}

void liberer_menu(Menu *menu) {
    SDL_FreeSurface(menu->background);
    for (int i = 0; i < 5; i++) {
        SDL_FreeSurface(menu->buttons[i]);
        SDL_FreeSurface(menu->buttons_hover[i]);
    }
    SDL_FreeSurface(menu->logo);
    Mix_FreeMusic(menu->bgMusic);
    Mix_FreeChunk(menu->hoverSound);
    TTF_CloseFont(menu->font);
    TTF_Quit();
}

