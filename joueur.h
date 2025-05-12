#ifndef JOUEUR_H_INCLUDED
#define JOUEUR_H_INCLUDED

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

typedef struct Joueur {
    SDL_Surface *sprites[6][4]; // stand, walk, run, jump, attack, die
    SDL_Rect pos;
    int direction;
    int currentAction;
    int frame;
    int health;
    int lastDamageTime;
} Joueur;

void initJoueur(Joueur *j);
void afficherJoueur(Joueur j, SDL_Surface *screen);
void animerJoueur(Joueur *j);
void deplacerJoueur(Joueur *j, const Uint8 *keystates);

#endif

