#ifndef FONCTION_H
#define FONCTION_H
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#define SCREEN_H 800
#define SCREEN_W 1200

typedef struct {
    char *url;
    SDL_Rect pos_img_affiche;
    SDL_Rect pos_img_ecran;
    SDL_Surface *img;
} image;

typedef struct {
    SDL_Rect position;
    SDL_Surface *images[17];
    int current_image;
    int direction;
    int move;
    int jump;
    int jump_speed;
    int is_attacking;  // Ajout pour gérer l'état d'attaque
    int attack_frame;  // Ajout pour gérer l'animation de l'attaque
    int healthpoints;
} Personne;

void initialiser_imageBACK(image *imge);
void afficher_imageBMP(SDL_Surface *screen, image imge);
void liberer_image(image imge);
void initPerso(Personne *p);
void afficherPerso(Personne p, SDL_Surface *screen);
void deplacerPerso(Personne *p);
void saut(Personne* p, int *jump, int* jump_speed);
void animerPersoP1(Personne* p, int left);
void animerPersoP2(Personne* p, int left);
void animerAttaque(Personne *p);  // Déclaration de la nouvelle fonction
void updateScore(int* score, Uint32* last_update_time);
void afficherScore(int score, TTF_Font* font, SDL_Surface *screen);
int chargerImages(SDL_Surface *images[]);
void afficherImage(SDL_Surface *screen, SDL_Surface *image);
void renderHealth(SDL_Surface* screen, SDL_Surface* hearts3, SDL_Surface* hearts2,
                  SDL_Surface* hearts1, SDL_Surface* hearts0, int healthPoints, SDL_Rect* destRect);

#endif
