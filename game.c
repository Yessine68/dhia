#include "integrated.h"
#include <stdio.h>
#include <stdlib.h>

GameState run_game() {
    SDL_Surface *screen;
    image IMAGE;
    SDL_Event event;
    Personne p1, p2;
    SDL_Surface *images[4];
    int current_image = 0;
    int score = 0;
    Uint32 last_score_update_time = SDL_GetTicks();
    TTF_Font* font;
    SDL_Color textColor = {255, 255, 255};

    int boucle = 1;
    int flag1 = 0, flag2 = 0;
    int left1 = 0, left2 = 0;
    int jump1 = 0, jump2 = 0;
    int jump_speed1 = 22, jump_speed2 = 22;
    int p2_visible = 0;
    int afficher_image_speciale = 0;
    int was_moving1 = 0, was_moving2 = 0;
    GameState nextState = STATE_MENU;

    // Setup game video mode
    SDL_WM_SetCaption("Free Palestine", "Free Palestine");
    screen = SDL_SetVideoMode(SCREEN_W, SCREEN_H, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!screen) {
        printf("Could not set video mode: %s.\n", SDL_GetError());
        return STATE_EXIT;
    }

    // Load heart images
    SDL_Surface* hearts3 = IMG_Load("h1.png"); // 3 hearts
    SDL_Surface* hearts2 = IMG_Load("h2.png"); // 2 hearts
    SDL_Surface* hearts1 = IMG_Load("h3.png"); // 1 heart
    SDL_Surface* hearts0 = IMG_Load("h4.png"); // 0 hearts   
    SDL_Rect heartPosition = {100, 50, 0, 0};

    initialiser_imageBACK(&IMAGE);
    initPerso(&p1);
    initPerso(&p2);
    p2.position.x = 1000;
    p2.position.y = SCREEN_H - 450;
    p1.position.x = 20;
    p1.position.y = SCREEN_H - 450;

    font = TTF_OpenFont("arial.ttf", 35);
    if (!font) {
        printf("Erreur de chargement de la police: %s\n", TTF_GetError());
        return STATE_EXIT;
    }

    if (!chargerImages(images)) {
        return STATE_EXIT;
    }

    while (boucle) {
        afficher_imageBMP(screen, IMAGE);
        afficherPerso(p1, screen);
        if (p1.is_attacking) {
            printf("Main: p1 est en train d'attaquer\n");
            animerAttaque(&p1);
        } else {
            animerPersoP1(&p1, left1);
        }

        if (p2_visible) {
            afficherPerso(p2, screen);
            if (p2.is_attacking) {
                printf("Main: p2 est en train d'attaquer\n");
                animerAttaque(&p2);
            } else {
                animerPersoP2(&p2, left2);
            }
        }

        updateScore(&score, &last_score_update_time);
        afficherScore(score, font, screen);

        if (afficher_image_speciale) {
            afficherImage(screen, images[current_image]);
        }

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    boucle = 0;
                    nextState = STATE_EXIT;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT && 
                        (event.motion.y <= 300 && event.motion.y >= 250 && 
                         event.motion.x <= 423 && event.motion.x > 213)) {
                        boucle = 0;
                    }
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            boucle = 0;  // Return to menu when Escape is pressed
                            break;
                        case SDLK_RIGHT:
                            left1 = 0;
                            p1.direction = 1;
                            if (p1.position.x < SCREEN_W - 60) {
                                p1.move = 1;
                                was_moving1 = 1;
                            } else {
                                p1.move = 0;
                            }
                            break;
                        case SDLK_h:
                            p1.healthpoints--;
                            score-=10;
                            break;
                        case SDLK_LEFT:
                            left1 = 1;
                            p1.direction = -1;
                            if (p1.position.x > 0) {
                                p1.move = 1;
                                was_moving1 = 1;
                            } else {
                                p1.move = 0;
                            }
                            break;
                        case SDLK_UP:
                            if (!jump1 && !p1.is_attacking) {
                                jump1 = 1;
                                p1.jump_speed = 22;
                            }
                            break;
                        case SDLK_SPACE:
                            if (!p1.is_attacking && !jump1) {
                                printf("Main: Touche ESPACE détectée pour p1\n");
                                p1.is_attacking = 1;
                                p1.attack_frame = 0;
                                p1.move = 0;
                            }
                            break;
                        case SDLK_LSHIFT:
                            if (p1.direction >= 0) {
                                p1.direction = 5;
                            } else {
                                p1.direction = -5;
                            }
                            break;
                        case SDLK_d:
                            left2 = 0;
                            p2.direction = 1;
                            if (p2.position.x < SCREEN_W - 60) {
                                p2.move = 1;
                                was_moving2 = 1;
                            } else {
                                p2.move = 0;
                            }
                            break;
                        case SDLK_q:
                            left2 = 1;
                            p2.direction = -1;
                            if (p2.position.x > 0) {
                                p2.move = 1;
                                was_moving2 = 1;
                            } else {
                                p2.move = 0;
                            }
                            break;
                        case SDLK_z:
                            if (!jump2 && !p2.is_attacking) {
                                jump2 = 1;
                                p2.jump_speed = 22;
                            }
                            break;
                        case SDLK_s:
                            if (!p2.is_attacking && !jump2) {
                                printf("Main: Touche S détectée pour p2\n");
                                p2.is_attacking = 1;
                                p2.attack_frame = 0;
                                p2.move = 0;
                            }
                            break;
                        case SDLK_f:
                            p2_visible = 1;
                            break;
                        case SDLK_LCTRL:
                            if (p2.direction >= 0) {
                                p2.direction = 5;
                            } else {
                                p2.direction = -5;
                            }
                            break;
                        case SDLK_v:
                            current_image++;
                            if (current_image >= 4) {
                                current_image = 0;
                                afficher_image_speciale = 0;
                            } else {
                                afficher_image_speciale = 1;
                            }
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                        case SDLK_RIGHT:
                            p1.move = 0;
                            if (!p1.is_attacking) p1.current_image = 0;
                            was_moving1 = 0;
                            break;
                        case SDLK_LEFT:
                            p1.move = 0;
                            if (!p1.is_attacking) p1.current_image = 8;
                            was_moving1 = 0;
                            break;
                        case SDLK_d:
                            p2.move = 0;
                            if (!p2.is_attacking) p2.current_image = 0;
                            was_moving2 = 0;
                            break;
                        case SDLK_q:
                            p2.move = 0;
                            if (!p2.is_attacking) p2.current_image = 8;
                            was_moving2 = 0;
                            break;
                        case SDLK_LSHIFT:
                            if (p1.direction >= 0) {
                                p1.direction = 1;
                            } else {
                                p1.direction = -1;
                            }
                            break;
                        case SDLK_LCTRL:
                            if (p2.direction >= 0) {
                                p2.direction = 1;
                            } else {
                                p2.direction = -1;
                            }
                            break;
                    }
                    break;
            }
        }

        if (!p1.is_attacking && was_moving1) {
            p1.move = 1;
        }
        if (!p2.is_attacking && was_moving2) {
            p2.move = 1;
        }
        if (p1.position.x >= SCREEN_W - 100) {
         printf("Character reached edge, transitioning to puzzle game\n");
        boucle = 0;
        nextState = STATE_PUZZLE;  // Changed from STATE_LEVEL2 to STATE_PUZZLE
        }
        saut(&p1, &jump1, &jump_speed1);
        saut(&p2, &jump2, &jump_speed2);
        SDL_Delay(5);
        deplacerPerso(&p1);
        deplacerPerso(&p2);

        renderHealth(screen, hearts3, hearts2, hearts1, hearts0, p1.healthpoints, &heartPosition);
        SDL_Flip(screen);
    }

    TTF_CloseFont(font);
    for (int i = 0; i < 16; i++) {
        if (p1.images[i]) SDL_FreeSurface(p1.images[i]);
        if (p2.images[i]) SDL_FreeSurface(p2.images[i]);
    }
    for (int i = 0; i < 4; i++) {
        if (images[i]) SDL_FreeSurface(images[i]);
    }
    if (hearts3) SDL_FreeSurface(hearts3);
    if (hearts2) SDL_FreeSurface(hearts2);
    if (hearts1) SDL_FreeSurface(hearts1);
    if (hearts0) SDL_FreeSurface(hearts0);
    liberer_image(IMAGE);
    
    return nextState;
}