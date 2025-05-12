#include "enemie.h"
#include <stdio.h>
#include <stdlib.h>

void initEnnemi(Ennemi *e) {
    const char *actions[5] = {"walk", "run", "jump", "attack", "die"};
    char filename[100];

    e->stand = IMG_Load("stand.png");
    if (!e->stand) {
        printf("Erreur chargement stand.png : %s\n", IMG_GetError());
    }

    for (int action = 0; action < 5; action++) {
        for (int i = 0; i < 4; i++) {
            sprintf(filename, "%s%d.png", actions[action], i);
            printf("Loading %s\n", filename); // Debug
            e->sprites[action][i] = IMG_Load(filename);
            if (!e->sprites[action][i]) {
                printf("Erreur chargement image %s : %s\n", filename, IMG_GetError());
            }
        }
    }

    e->pos.x = 1500;
    e->pos.y = 410;
    e->pos.w = 100;
    e->pos.h = 100;
    e->direction = 0;
    e->currentAction = -1;
    e->frame = 0;
    e->health = 100;
    e->lastDamageTime = 0;
}

void afficherEnnemi(Ennemi e, SDL_Surface *screen) {
    if (e.currentAction == -1) {
        SDL_BlitSurface(e.stand, NULL, screen, &e.pos);
    } else {
        SDL_BlitSurface(e.sprites[e.currentAction][e.frame], NULL, screen, &e.pos);
    }

    SDL_Rect bar = {e.pos.x, e.pos.y - 20, e.health, 10};
    SDL_Color color = {0, 255, 0};
    if (e.health <= 66) color = (SDL_Color){255, 255, 0};
    if (e.health <= 33) color = (SDL_Color){255, 0, 0};

    SDL_FillRect(screen, &bar, SDL_MapRGB(screen->format, color.r, color.g, color.b));
}

void animerEnnemi(Ennemi *e) {
    if (e->currentAction == -1) return;
    if (e->currentAction == 4 && e->frame >= 3) return;

    e->frame++;
    if (e->frame >= 4)
        e->frame = (e->currentAction == 4) ? 3 : 0;
}

void deplacer(Ennemi *e) {
    if (e->currentAction == -1) return;
    if (e->currentAction == 0)
        e->pos.x -= 1;
    else if (e->currentAction == 1)
        e->pos.x -= 6;
}

int collision(SDL_Rect A, SDL_Rect B) {
    return !(A.x + A.w < B.x || A.x > B.x + B.w || A.y + A.h < B.y || A.y > B.y + B.h);
}

void deplacerIA(Ennemi *E, SDL_Rect posperso) {
    if (E->currentAction == 4) return;

    int dx = posperso.x - E->pos.x;
    E->direction = (dx > 0) ? 0 : 1;

    if (abs(dx) < 60)
        E->currentAction = 3;
    else if (abs(dx) < 250)
        E->currentAction = 1;
    else if (abs(dx) < 500)
        E->currentAction = 0;
    else
        E->currentAction = -1;

    deplacer(E);
}

void mourir(Ennemi *E) {
    if (E->currentAction != 4) {
        E->currentAction = 4;
        E->frame = 0;
    }
}

