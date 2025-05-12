#include "score_header.h"
#include <stdio.h>
#include <string.h>

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    
    SDL_Surface *ecran = SDL_SetVideoMode(1920, 1080, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

    SDL_WM_SetCaption("High score", NULL);

    TTF_Font *font = TTF_OpenFont("arial.ttf", 30);
    SDL_Color blanc = {255, 255, 255};

    Background bg3 = {chargerImage("bg3.png"), {0, 0}};						
    
    Bouton Valider = {chargerImage("Valider.png"), {790, 600}};
    Bouton Retour = {chargerImage("Retour.png"), {550, 615}};
    Bouton Quitter = {chargerImage("Quitter.png"), {1130, 600}};

    SDL_Surface *etoile1 = chargerImage("etoile1.png");
    SDL_Surface *etoile2 = chargerImage("etoile2.png");
    SDL_Surface *etoile3 = chargerImage("etoile3.png");

    SDL_Rect posEtoile1, posEtoile2, posEtoile3;

    // Declare and initialize menu
    Menu *menu = malloc(sizeof(Menu));
    if (!menu) {
        printf("Erreur d'allocation mémoire pour le menu.\n");
        return 1;
    }

    // Chargement de la musique
    menu->music = Mix_LoadMUS("Misic.mp3");
    if (!menu->music) {
        printf("Erreur lors du chargement de la musique: %s\n", Mix_GetError());
    }
    Mix_PlayMusic(menu->music, -1); // Jouer la musique en boucle

    // Chargement du son de survol
    menu->hoverSound = Mix_LoadWAV("click.wav");
    if (!menu->hoverSound) {
        printf("Erreur lors du chargement du son de survol: %s\n", Mix_GetError());
    }

    // Positionnement des étoiles VERTICALEMENT (même X, Y différents)
    posEtoile1.x = 660;
   
// Positionnement des étoiles sous le texte "Meilleurs Scores"
int y_titre_score = 340; // Y du texte "Meilleurs Scores"
int decalage_apres_titre = 60; // espace entre le titre et la première étoile

posEtoile1.y = y_titre_score + decalage_apres_titre; // 340 + 60 = 400
posEtoile2.y = posEtoile1.y + 80; // 480
posEtoile3.y = posEtoile2.y + 80; // 560
    posEtoile2.x = 660;
    

    posEtoile3.x = 660;
    

    // Noms des joueurs gagnants (exemples réalistes)
    char *joueurs[3] = {"Alice", "Bob", "Charlie"};

    // Affichage sur l'écran
    SDL_BlitSurface(etoile1, NULL, ecran, &posEtoile1);
    SDL_BlitSurface(etoile2, NULL, ecran, &posEtoile2);
    SDL_BlitSurface(etoile3, NULL, ecran, &posEtoile3);

    SDL_Flip(ecran); // Rafraîchir l'affichage

    if (!bg3.image || !Valider.image || !Retour.image || !Quitter.image || !etoile1 || !etoile2 || !etoile3) {
        printf("Erreur de chargement d'images.\n");
        return 1;
    }

    bool running = true;
    bool afficherScores = false;
    SDL_Event event;

    char pseudo[20] = "";
    SDL_Rect zoneTexte = {750, 400, 400, 50}; // Zone de texte aussi déplacée à droite

    while (running) {
        SDL_BlitSurface(bg3.image, NULL, ecran, &bg3.position);

        if (!afficherScores) {
            afficherTexte(ecran, font, "Enter Name/Nickname", 750, 340, blanc); // Plus à droite

            SDL_FillRect(ecran, &zoneTexte, SDL_MapRGB(ecran->format, 255, 255, 255));
            afficherTexte(ecran, font, pseudo, zoneTexte.x + 5, zoneTexte.y + 5, (SDL_Color){0, 0, 0});
            afficherBouton(ecran, Valider);
        } else {
            afficherTexte(ecran, font, "High score", 750, 340, blanc);
            afficherImage(ecran, etoile1, posEtoile1.x, posEtoile1.y);
            afficherImage(ecran, etoile2, posEtoile2.x, posEtoile2.y);
            afficherImage(ecran, etoile3, posEtoile3.x, posEtoile3.y);
          afficherTexte(ecran, font, joueurs[0], posEtoile1.x + 120, posEtoile1.y + 23, blanc); // À droite de l'étoile et un peu plus bas
afficherTexte(ecran, font, joueurs[1], posEtoile2.x + 120, posEtoile2.y + 23, blanc); // À droite de l'étoile et un peu plus bas
afficherTexte(ecran, font, joueurs[2], posEtoile3.x + 120, posEtoile3.y + 23, blanc); // À droite de l'étoile et un peu plus bas

            afficherBouton(ecran, Retour);
            afficherBouton(ecran, Quitter);
        }

        SDL_Flip(ecran);

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        int x = event.button.x, y = event.button.y;
                        if (!afficherScores && x >= Valider.position.x && x <= Valider.position.x + Valider.image->w &&
                            y >= Valider.position.y && y <= Valider.position.y + Valider.image->h) {
                            afficherScores = true;
                        }
                        if (afficherScores && x >= Retour.position.x && x <= Retour.position.x + Retour.image->w &&
                            y >= Retour.position.y && y <= Retour.position.y + Retour.image->h) {
                            afficherScores = false;
                        }
                        if (afficherScores && x >= Quitter.position.x && x <= Quitter.position.x + Quitter.image->w &&
                            y >= Quitter.position.y && y <= Quitter.position.y + Quitter.image->h) {
                            running = false;
                        }
                    }
                    break;
                case SDL_KEYDOWN:
                    if (!afficherScores) {
                        if (event.key.keysym.sym == SDLK_RETURN) {
                            afficherScores = true;
                        } else if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(pseudo) > 0) {
                            pseudo[strlen(pseudo) - 1] = '\0';
                        } else if (strlen(pseudo) < 19 && event.key.keysym.unicode != 0) {
                            pseudo[strlen(pseudo)] = (char)event.key.keysym.unicode;
                            pseudo[strlen(pseudo) + 1] = '\0';
                        }
                    }
                    break;
            }
        }
    }

    SDL_FreeSurface(bg3.image);
    SDL_FreeSurface(Valider.image);
    SDL_FreeSurface(Retour.image);
    SDL_FreeSurface(Quitter.image);
    Mix_FreeMusic(menu->music);
    Mix_FreeChunk(menu->hoverSound);
    SDL_FreeSurface(etoile1);
    SDL_FreeSurface(etoile2);
    SDL_FreeSurface(etoile3);
    TTF_CloseFont(font);
    free(menu); // Free the allocated memory for menu
    SDL_Quit();

// background

int main(int argc, char *argv[]) {
    SDL_Surface *screen = NULL;
    SDL_Event event;
    int running = 1;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("Erreur d'initialisation de la SDL : %s\n", SDL_GetError());
        return 1;
    }

    screen = SDL_SetVideoMode(1000, 600, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!screen) {
        printf("Erreur lors du chargement du mode vidéo : %s\n", SDL_GetError());
        return 1;
    }

    // Initialisation SDL_Mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erreur Mix_OpenAudio : %s\n", Mix_GetError());
        return 1;
    }

    Mix_Music *music = Mix_LoadMUS("music.mp3");
    if (!music) {
        printf("Erreur chargement musique : %s\n", Mix_GetError());
    } else {
        Mix_PlayMusic(music, -1);
    }

    // Fond
    Background bg;
    initBackground(&bg, "background.jpg");

    // Sprite
    Sprite sprite;
    const char *frames[] = {
        "sprite1.png",
        "sprite2.png",
        "sprite3.png",
        "sprite4.png"
    };
    initSprite(&sprite, frames);

    // Masque pour collision
    SDL_Surface *mask = IMG_Load("mask.png");

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            deplacerSprite(&sprite, event, &bg);
        }

        animerSprite(&sprite);

        afficherBackground(screen, bg);
        afficherSprite(screen, &sprite);

        if (collisionParfaite(mask, bg.camera, sprite.position)) {
            printf("Collision détectée !\n");
        }

        SDL_Flip(screen);
        SDL_Delay(30);
    }

    Mix_FreeMusic(music);
    SDL_Quit();
    return 0;
}
}

