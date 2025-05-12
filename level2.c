#include "integrated.h"
#include <stdio.h>
#include <stdlib.h>

static int isMouseOver(SDL_Rect rect, int x, int y) {
    return x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h;
}

GameState run_level2() {
    SDL_Surface *screen = NULL, *background = NULL, *gameOverText = NULL, *winText = NULL;
    SDL_Surface *pauseBtn = NULL, *playBtn = NULL;
    SDL_Rect pauseBtnPos = {10, 10, 0, 0}; 
    SDL_Event event;
    int running = 1, paused = 0;
    GameState nextState = STATE_MENU;

    screen = SDL_SetVideoMode(1920, 1080, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!screen) {
        fprintf(stderr, "Could not set video mode for level 2: %s\n", SDL_GetError());
        return STATE_EXIT;
    }

    background = IMG_Load("gamebackground.png");
    if (!background) {
        fprintf(stderr, "Could not load background: %s\n", IMG_GetError());
    }

    pauseBtn = IMG_Load("pause.png");
    playBtn = IMG_Load("play.png");

    if (!pauseBtn || !playBtn) {
        printf("Erreur chargement boutons : %s\n", IMG_GetError());
        return STATE_EXIT;
    }

    pauseBtnPos.w = pauseBtn->w;
    pauseBtnPos.h = pauseBtn->h;

    TTF_Font *font = TTF_OpenFont("bubble.ttf", 72);
    if (!font)
        font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 72);

    SDL_Color red = {255, 0, 0}, green = {0, 255, 0};
    gameOverText = TTF_RenderText_Solid(font, "GAME OVER!", red);
    winText = TTF_RenderText_Solid(font, "YOU WIN!", green);

    Joueur j;
    Ennemi e;
    initJoueur(&j);
    initEnnemi(&e);

    while (running) {
        Uint32 currentTime = SDL_GetTicks();
        Uint8 *keystates = SDL_GetKeyState(NULL);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
                nextState = STATE_EXIT;
            }
            else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                running = 0;
                nextState = STATE_MENU;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mx = event.button.x;
                int my = event.button.y;
                if (isMouseOver(pauseBtnPos, mx, my)) {
                    paused = !paused;
                }
            }
        }

        SDL_BlitSurface(background, NULL, screen, NULL);
        SDL_BlitSurface(paused ? playBtn : pauseBtn, NULL, screen, &pauseBtnPos);

        if (!paused) {
            if (j.health > 0) {
                deplacerJoueur(&j, keystates);
                deplacerIA(&e, j.pos);

                if (e.currentAction == 3 && collision(j.pos, e.pos) &&
                    currentTime - j.lastDamageTime > 1500) {
                    j.health = (j.health > 20) ? j.health - 20 : 0;
                    j.lastDamageTime = currentTime;
                    if (j.health <= 0) j.currentAction = 5;
                }
            } else {
                e.currentAction = -1;
            }

            if (j.health > 0 && j.currentAction == 4 &&
                collision(j.pos, e.pos) &&
                currentTime - e.lastDamageTime > 1500) {
                e.health = (e.health > 20) ? e.health - 20 : 0;
                e.lastDamageTime = currentTime;
                if (e.health <= 0) mourir(&e);
            }

            animerJoueur(&j);
            animerEnnemi(&e);
        }

        afficherJoueur(j, screen);
        afficherEnnemi(e, screen);
        
        if (j.pos.x >= screen->w - 100) {
            printf("Character reached end of level 2, transitioning to level 3 (final boss)\n");
            running = 0;
            nextState = STATE_LEVEL3;
        }

        if (j.health <= 0 && gameOverText) {
            SDL_Rect pos = {screen->w / 2 - gameOverText->w / 2, screen->h / 2 - gameOverText->h / 2};
            SDL_BlitSurface(gameOverText, NULL, screen, &pos);
            // Add automatic return to menu after a delay
            static Uint32 deathTime = 0;
            if (deathTime == 0) deathTime = SDL_GetTicks();
            else if (SDL_GetTicks() - deathTime > 3000) {
                running = 0;
                nextState = STATE_MENU;
            }
        } else if (e.health <= 0 && winText) {
            SDL_Rect pos = {screen->w / 2 - winText->w / 2, screen->h / 2 - winText->h / 2};
            SDL_BlitSurface(winText, NULL, screen, &pos);
            // Add automatic return to menu after a delay
            static Uint32 winTime = 0;
            if (winTime == 0) winTime = SDL_GetTicks();
            else if (SDL_GetTicks() - winTime > 3000) {
                running = 0;
                nextState = STATE_MENU;
            }
        }

        SDL_Flip(screen);
        SDL_Delay(30);
    }

    if (gameOverText) SDL_FreeSurface(gameOverText);
    if (winText) SDL_FreeSurface(winText);
    if (background) SDL_FreeSurface(background);
    if (pauseBtn) SDL_FreeSurface(pauseBtn);
    if (playBtn) SDL_FreeSurface(playBtn);
    if (font) TTF_CloseFont(font);
    
    // Free the joueur and ennemi resources
    for (int a = 0; a < 6; a++) {
        for (int i = 0; i < 4; i++) {
            if (j.sprites[a][i]) SDL_FreeSurface(j.sprites[a][i]);
        }
    }
    
    if (e.stand) SDL_FreeSurface(e.stand);
    for (int a = 0; a < 5; a++) {
        for (int i = 0; i < 4; i++) {
            if (e.sprites[a][i]) SDL_FreeSurface(e.sprites[a][i]);
        }
    }
    
    return nextState;
}