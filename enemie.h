#ifndef ENEMIE_H_INCLUDED
#define ENEMIE_H_INCLUDED

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

typedef struct Ennemi {
    SDL_Surface *sprites[5][4]; // walk, run, jump, attack, die
    SDL_Surface *stand;
    SDL_Rect pos;
    int direction;
    int currentAction;
    int frame;
    int health;
    int lastDamageTime;
} Ennemi;

void initEnnemi(Ennemi *e);
void afficherEnnemi(Ennemi e, SDL_Surface *screen);
void animerEnnemi(Ennemi *e);
void deplacer(Ennemi *e);
int collision(SDL_Rect A, SDL_Rect B);
void deplacerIA(Ennemi *E, SDL_Rect posperso);
void mourir(Ennemi *E);

#endif

