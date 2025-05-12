#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "perso.h"

void initialiser_imageBACK(image *imge) {
    imge->url = "back.png";
    imge->img = IMG_Load(imge->url);
    if (imge->img == NULL) {
        printf("Unable to load background image %s \n", SDL_GetError());
        return;
    }
    imge->pos_img_ecran.x = 0;
    imge->pos_img_ecran.y = 0;
    imge->pos_img_affiche.x = 0;
    imge->pos_img_affiche.y = 0;
}

void afficher_imageBMP(SDL_Surface *screen, image imge) {
    SDL_BlitSurface(imge.img, NULL, screen, &imge.pos_img_ecran);
}

void liberer_image(image imge) {
    SDL_FreeSurface(imge.img);
}

void initPerso(Personne *p) {
    p->current_image = 0;
    p->direction = 1;
    p->move = 0;
    p->is_attacking = 0;
    p->attack_frame = 0;
    p->jump = 0;
    p->jump_speed = 22;
    p->healthpoints=3;
    
    p->images[0] = IMG_Load("./image/jou.png");
    p->images[1] = IMG_Load("./image/jou1.png");
    p->images[2] = IMG_Load("./image/jou2.png");
    p->images[3] = IMG_Load("./image/jou3.png");
    p->images[4] = IMG_Load("./image/jou4.png");
    p->images[5] = IMG_Load("./image/jou5.png");
    p->images[6] = IMG_Load("./image/jou6.png");
    p->images[7] = IMG_Load("./image/jou7.png");
    p->images[8] = IMG_Load("./image/jour.png");
    p->images[9] = IMG_Load("./image/jour1.png");
    p->images[10] = IMG_Load("./image/jour2.png");
    p->images[11] = IMG_Load("./image/jour3.png");
    p->images[12] = IMG_Load("./image/jour4.png");
    p->images[13] = IMG_Load("./image/jour5.png");
    p->images[14] = IMG_Load("./image/jou_attack_right.png"); // Nouveau sprite attaque droite
    p->images[15] = IMG_Load("./image/jou_attack_left.png");  // Nouveau sprite attaque gauche
    p->images[16] = IMG_Load("./image/die1.png");
   

    for (int i = 0; i < 16; i++) {
        if (p->images[i] == NULL) {
            printf("Erreur de chargement de l'image %d : %s\n", i, IMG_GetError());
            // Si les sprites d'attaque ne sont pas disponibles, utiliser jou4.png et jour4.png
            if (i == 14) p->images[14] = p->images[4];  // Fallback sur jou4.png
            if (i == 15) p->images[15] = p->images[12]; // Fallback sur jour4.png
        }
    }
}

void afficherPerso(Personne p, SDL_Surface *screen) {
    SDL_Rect pos = p.position;
    if (p.is_attacking) {
        printf("AfficherPerso: Attaque active, attack_frame=%d\n", p.attack_frame);
        int attack_right = 14, attack_left = 15;
        // Décalage visuel pendant l'attaque
        pos.x += (p.direction >= 0) ? 5 : -5; // Décalage de 5 pixels
        if (p.direction >= 0) {
            SDL_BlitSurface(p.images[attack_right], NULL, screen, &pos);
        } else {
            SDL_BlitSurface(p.images[attack_left], NULL, screen, &pos);
        }
    } else {
        SDL_BlitSurface(p.images[p.current_image], NULL, screen, &pos);
    }
}

void deplacerPerso(Personne *p) {
    if (p->move == 1 && !p->is_attacking) {
        p->position.x += p->direction * 2;
        if (p->position.x > SCREEN_W - 60 || p->position.x < 0) {
            p->direction = 0;
            p->move = 0;
        }
    }
}

void saut(Personne* p, int *jump, int *jump_speed) {
    const int sol = SCREEN_H - 350;
    if (*jump) {
        p->position.y -= *jump_speed;
        (*jump_speed)--;

        if (*jump_speed < -10) {
            *jump = 0;
            *jump_speed = 22;
        }
    } else {
        if (p->position.y < sol) {
            p->position.y += 10;
        } else if (p->position.y > sol) {
            p->position.y = sol;
        }
    }
}
void animerPersoP1(Personne* p, int left) {
     if (p->healthpoints == 0) {
        p->current_image = 16;
        
    }else{
    
    if (p->is_attacking || p->jump) return;

    static int counter = 0;
    const int animation_speed = 8;
    static int change = 0;
    counter++;

    if (counter >= animation_speed) {
        if (p->move == 0) {
            if (left) {
                p->current_image = 8 + change;
                change = (change + 1) % 2;
            } else {
                p->current_image = 0 + change;
                change = (change + 1) % 2;
            }
        } else {
            if (left) {
                p->current_image++;
                if (p->current_image < 10 || p->current_image > 13) {
                    p->current_image = 10;
                }
            } else {
                p->current_image++;
                if (p->current_image < 2 || p->current_image > 7) {
                    p->current_image = 2;
                }
            }
        }
        counter = 0;
    }
    }
}


void animerPersoP2(Personne* p, int left) {
    if (p->is_attacking || p->jump) return;

    static int counter = 0;
    const int animation_speed = 8;
    static int change = 0;
    counter++;

    if (counter >= animation_speed) {
        if (p->move == 0) {
            if (left) {
                p->current_image = 8 + change;
                change = (change + 1) % 2;
            } else {
                p->current_image = 0 + change;
                change = (change + 1) % 2;
            }
        } else {
            if (left) {
                p->current_image++;
                if (p->current_image < 10 || p->current_image > 13) {
                    p->current_image = 10;
                }
            } else {
                p->current_image++;
                if (p->current_image < 2 || p->current_image > 7) {
                    p->current_image = 2;
                }
            }
        }
        counter = 0;
    }
}

void animerAttaque(Personne *p) {
    printf("AnimerAttaque: attack_frame=%d, is_attacking=%d\n", p->attack_frame, p->is_attacking);
    const int attack_speed = 4;
    static int counter = 0;
    counter++;

    if (counter >= attack_speed) {
        p->attack_frame++;
        if (p->attack_frame >= 4) { // Durée totale de l'attaque : 4 frames
            p->is_attacking = 0;
            p->attack_frame = 0;
            p->current_image = (p->direction < 0) ? 8 : 0;
            printf("AnimerAttaque: Attaque terminée\n");
        }
        counter = 0;
    }
}

void updateScore(int* score, Uint32* last_update_time) {
    Uint32 current_time = SDL_GetTicks();
    if (current_time - *last_update_time >= 1000) {
        (*score) += 1;
        *last_update_time = current_time;
    }
}

void afficherScore(int score, TTF_Font* font, SDL_Surface* ecran) {
    char score_text[20];
    sprintf(score_text, "Score: %d", score);
    SDL_Color textColor = {255, 255, 255};
    SDL_Surface* score_surface = TTF_RenderText_Solid(font, score_text, textColor);

    if (score_surface != NULL) {
        SDL_Rect pos_score;
        pos_score.x = 1000;
        pos_score.y = 10;
        SDL_BlitSurface(score_surface, NULL, ecran, &pos_score);
        SDL_FreeSurface(score_surface);
    } else {
        printf("Erreur lors de la création de la surface du score: %s\n", TTF_GetError());
    }
}

int chargerImages(SDL_Surface *images[]) {
    images[0] = IMG_Load("h1.png");
    images[1] = IMG_Load("h2.png");
    images[2] = IMG_Load("h3.png");
    images[3] = IMG_Load("h4.png");

    if (!images[0] || !images[1] || !images[2] || !images[3]) {
        printf("Erreur de chargement des images: %s\n", IMG_GetError());
        return 0;
    }
    return 1;
}

void afficherImage(SDL_Surface *screen, SDL_Surface *image) {
    SDL_Rect pos;
    pos.x = 20;
    pos.y = 200;
    SDL_BlitSurface(image, NULL, screen, &pos);
}


void renderHealth(SDL_Surface* screen, SDL_Surface* hearts3, SDL_Surface* hearts2,
                  SDL_Surface* hearts1, SDL_Surface* hearts0, int healthPoints, SDL_Rect* destRect) {
    SDL_Surface* currentHeart = NULL;

    if (healthPoints == 3) {
        currentHeart = hearts3;
    } else if (healthPoints == 2) {
        currentHeart = hearts2;
    } else if (healthPoints == 1) {
        currentHeart = hearts1;
    } else {
        currentHeart = hearts0;
    }

    SDL_BlitSurface(currentHeart, NULL, screen, destRect);
}











