#ifndef HARMONIA_MENU_H
#define HARMONIA_MENU_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1000

typedef struct {
    SDL_Surface *background;
    SDL_Surface *buttons[5];
    SDL_Surface *buttons_hover[5];
    Mix_Music *bgMusic;
    Mix_Chunk *hoverSound;
    TTF_Font *font;
    SDL_Color textColor;
    SDL_Surface *logo;
    SDL_Rect poslogo;
} Menu;

void init_menu(Menu *menu);
void afficher_menu(Menu menu, SDL_Surface *ecran, int hoveredIndex);
void liberer_menu(Menu *menu);

#endif

