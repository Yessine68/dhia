#include "joueur.h"
#include <stdio.h>
#include <SDL/SDL_image.h>

void initJoueur(Joueur *j) {
    const char *actions[6] = {"stand", "walk", "run", "jump", "attack", "die"};
    char filename[100];

    for (int a = 0; a < 6; a++) {
        for (int i = 0; i < 4; i++) {
            if (a == 0)
                sprintf(filename, "j_stand0.png");
            else
                sprintf(filename, "j_%s%d.png", actions[a], i);

            j->sprites[a][i] = IMG_Load(filename);
            if (!j->sprites[a][i]) {
                printf("Erreur chargement %s : %s\n", filename, IMG_GetError());
            }
        }
    }

    j->pos.x = 200;
    j->pos.y = 470;
    j->pos.w = 100;
    j->pos.h = 100;
    j->direction = 0;
    j->currentAction = 0;
    j->frame = 0;
    j->health = 100;
    j->lastDamageTime = 0;
}

void afficherJoueur(Joueur j, SDL_Surface *screen) {
    SDL_BlitSurface(j.sprites[j.currentAction][j.frame], NULL, screen, &j.pos);

    SDL_Rect bar = {j.pos.x, j.pos.y - 20, j.health, 10};
    SDL_Color color = {255, 105, 180};
    if (j.health < 100) color = (SDL_Color){255, 182, 193};
    if (j.health <= 0) color = (SDL_Color){255, 0, 0};

    SDL_FillRect(screen, &bar, SDL_MapRGB(screen->format, color.r, color.g, color.b));
}

void animerJoueur(Joueur *j) {
    if (j->currentAction == 5 && j->frame >= 3) return;
    j->frame++;
    if (j->frame >= 4)
        j->frame = (j->currentAction == 5) ? 3 : 0;
}

void deplacerJoueur(Joueur *j, const Uint8 *keystates) {
    if (j->health <= 0) {
        j->currentAction = 5;
        return;
    }

    if (keystates[SDLK_RIGHT] && keystates[SDLK_b]) {
        j->pos.x += 10;
        j->currentAction = 2; // courir
    } else if (keystates[SDLK_RIGHT]) {
        j->pos.x += 2;
        j->currentAction = 1; // marcher
    } else if (keystates[SDLK_UP]) {
        j->pos.y -= 5;
        j->currentAction = 3; // sauter vers le haut
    } else if (keystates[SDLK_DOWN]) {
        j->pos.y += 5;
        j->currentAction = 3; // sauter vers le bas
    } else if (keystates[SDLK_SPACE]) {
        j->currentAction = 4; // attaquer
    } else {
        j->currentAction = 0; // rester debout
    }
}


