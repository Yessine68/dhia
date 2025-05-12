#include "integrated.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Helper function to scale a surface to a new size
static SDL_Surface* scaleSurface(SDL_Surface* surface, int width, int height) {
    if (!surface) return NULL;
    
    SDL_Surface* scaled = SDL_CreateRGBSurface(surface->flags, width, height,
        surface->format->BitsPerPixel,
        surface->format->Rmask, surface->format->Gmask,
        surface->format->Bmask, surface->format->Amask);
    if (!scaled) return NULL;
    SDL_Rect srcRect = {0, 0, surface->w, surface->h};
    SDL_Rect dstRect = {0, 0, width, height};
    SDL_SoftStretch(surface, &srcRect, scaled, &dstRect);
    return scaled;
}

static int isMouseOver(SDL_Rect rect, int x, int y) {
    return x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h;
}

static void drawMinimap(SDL_Surface *screen, Joueur j, Ennemi e, SDL_Surface* minimap_bg) {
    int minimap_width = 300;
    int minimap_height = 40;
    int minimap_x = (screen->w - minimap_width) / 2;
    int minimap_y = 10;
    SDL_Rect minimapRect = {minimap_x, minimap_y, minimap_width, minimap_height};

    // Draw minimap background
    if (minimap_bg) {
        SDL_BlitSurface(minimap_bg, NULL, screen, &minimapRect);
    } else {
        SDL_FillRect(screen, &minimapRect, SDL_MapRGB(screen->format, 50, 50, 50));
    }

    // Map world coordinates to minimap
    int world_width = 1920;
    int world_height = 1080;

    // Player dot
    SDL_Surface *playerDot = IMG_Load("assets/player_dot.png");
    SDL_Rect playerDotRect;
    playerDotRect.w = 8;
    playerDotRect.h = 8;
    playerDotRect.x = minimap_x + (j.pos.x * minimap_width) / world_width - playerDotRect.w / 2;
    playerDotRect.y = minimap_y + (j.pos.y * minimap_height) / world_height - playerDotRect.h / 2;

    if (playerDot) {
        SDL_BlitSurface(playerDot, NULL, screen, &playerDotRect);
        SDL_FreeSurface(playerDot);
    } else {
        SDL_FillRect(screen, &playerDotRect, SDL_MapRGB(screen->format, 0, 255, 0));
    }

    // Enemy dot
    SDL_Surface *enemyDot = IMG_Load("assets/enemy_dot.png");
    SDL_Rect enemyDotRect;
    enemyDotRect.w = 8;
    enemyDotRect.h = 8;
    enemyDotRect.x = minimap_x + (e.pos.x * minimap_width) / world_width - enemyDotRect.w / 2;
    enemyDotRect.y = minimap_y + (e.pos.y * minimap_height) / world_height - enemyDotRect.h / 2;

    if (enemyDot) {
        SDL_BlitSurface(enemyDot, NULL, screen, &enemyDotRect);
        SDL_FreeSurface(enemyDot);
    } else {
        SDL_FillRect(screen, &enemyDotRect, SDL_MapRGB(screen->format, 255, 0, 0));
    }
}

GameState run_level3() {
    SDL_Surface *screen = NULL, *background = NULL, *background_scaled = NULL;
    SDL_Surface *gameOverText = NULL, *winText = NULL;
    SDL_Surface *pauseBtn = NULL, *playBtn = NULL, *pauseBtn_scaled = NULL, *playBtn_scaled = NULL;
    SDL_Surface *minimap_bg_scaled = NULL;
    SDL_Rect pauseBtnPos = {10, 10, 50, 50}; 
    SDL_Event event;
    int running = 1, paused = 0;
    GameState nextState = STATE_MENU;

    // Initialize SDL video mode for level 3
    screen = SDL_SetVideoMode(1920, 1080, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!screen) {
        fprintf(stderr, "Could not set video mode for level 3: %s\n", SDL_GetError());
        return STATE_EXIT;
    }

    // Load background
    background = IMG_Load("back2.jpeg");
    if (!background) {
        fprintf(stderr, "Could not load background for level 3: %s\n", IMG_GetError());
    } else {
        background_scaled = scaleSurface(background, screen->w, screen->h);
    }

    // Load pause/play buttons
    pauseBtn = IMG_Load("pause.png");
    playBtn = IMG_Load("play.png");
    if (pauseBtn) pauseBtn_scaled = scaleSurface(pauseBtn, pauseBtnPos.w, pauseBtnPos.h);
    if (playBtn) playBtn_scaled = scaleSurface(playBtn, pauseBtnPos.w, pauseBtnPos.h);

    // Load minimap
    SDL_Surface *minimap_level3 = IMG_Load("assets/minimap_level3.png");
    if (minimap_level3) {
        minimap_bg_scaled = scaleSurface(minimap_level3, 300, 40);
        SDL_FreeSurface(minimap_level3);
    }

    // Initialize text
    TTF_Font *font = TTF_OpenFont("bubble.ttf", 72);
    if (!font) font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 72);
    SDL_Color red = {255, 0, 0, 0}, green = {0, 255, 0, 0};
    gameOverText = TTF_RenderText_Solid(font, "GAME OVER!", red);
    winText = TTF_RenderText_Solid(font, "YOU WIN!", green);

    // Initialize player and enemy (final boss)
    Joueur j;
    Ennemi e;
    initJoueur(&j);
    initEnnemi(&e);
    
    // Set boss position farther right
    e.pos.x = 1600;
    e.health = 150; // Make the boss tougher

    // Main game loop
    while (running) {
        Uint32 currentTime = SDL_GetTicks();
        Uint8 *keystates = SDL_GetKeyState(NULL);

        // Event handling
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

        // Rendering
        SDL_BlitSurface(background_scaled ? background_scaled : background, NULL, screen, NULL);
        SDL_BlitSurface(paused ? (playBtn_scaled ? playBtn_scaled : playBtn) : (pauseBtn_scaled ? pauseBtn_scaled : pauseBtn), NULL, screen, &pauseBtnPos);

        if (!paused) {
            // Game logic
            if (j.health > 0) {
                deplacerJoueur(&j, keystates);

        if (j.pos.x >= screen->w - 100) {
            printf("Character reached end of level 3, transitioning to high score screen\n");
            running = 0;
            nextState = STATE_HIGHSCORE; // Transition to high score screen
        }
                deplacerIA(&e, j.pos);

                // Combat - boss deals more damage
                if (e.currentAction == 3 && collision(j.pos, e.pos) &&
                    currentTime - j.lastDamageTime > 1500) {
                    j.health = (j.health > 30) ? j.health - 30 : 0; // Boss deals more damage
                    j.lastDamageTime = currentTime;
                    if (j.health <= 0) j.currentAction = 5;
                }
            } else {
                e.currentAction = -1;
            }

            if (j.health > 0 && j.currentAction == 4 &&
                collision(j.pos, e.pos) &&
                currentTime - e.lastDamageTime > 1500) {
                e.health = (e.health > 10) ? e.health - 10 : 0; // Boss takes less damage
                e.lastDamageTime = currentTime;
                if (e.health <= 0) mourir(&e);
            }

            animerJoueur(&j);
            animerEnnemi(&e);
        }

        // Draw characters and minimap
        afficherJoueur(j, screen);
        afficherEnnemi(e, screen);
        drawMinimap(screen, j, e, minimap_bg_scaled);

        
        // Game over/win screens
        if (j.health <= 0 && gameOverText) {
            SDL_Rect pos = {screen->w/2 - gameOverText->w/2, screen->h/2 - gameOverText->h/2, 0, 0};
            SDL_BlitSurface(gameOverText, NULL, screen, &pos);
            
            // Return to menu after delay
            static Uint32 deathTime = 0;
            if (deathTime == 0) deathTime = SDL_GetTicks();
            else if (SDL_GetTicks() - deathTime > 3000) {
                running = 0;
                nextState = STATE_MENU;
            }
        } else if (e.health <= 0 && winText) {
            SDL_Rect pos = {screen->w/2 - winText->w/2, screen->h/2 - winText->h/2, 0, 0};
            SDL_BlitSurface(winText, NULL, screen, &pos);
            
            // Display final victory message
            SDL_Surface *victoryText = TTF_RenderText_Solid(font, "CONGRATULATIONS! YOU COMPLETED THE GAME!", green);
            if (victoryText) {
                SDL_Rect victoryPos = {screen->w/2 - victoryText->w/2, screen->h/2 + 100, 0, 0};
                SDL_BlitSurface(victoryText, NULL, screen, &victoryPos);
                SDL_FreeSurface(victoryText);
            }
            
            // Return to menu after delay
            static Uint32 winTime = 0;
            if (winTime == 0) winTime = SDL_GetTicks();
            else if (SDL_GetTicks() - winTime > 5000) {
                running = 0;
                nextState = STATE_MENU;
            }
        }

        SDL_Flip(screen);
        SDL_Delay(30);
    }

    // Cleanup
    if (gameOverText) SDL_FreeSurface(gameOverText);
    if (winText) SDL_FreeSurface(winText);
    if (background) SDL_FreeSurface(background);
    if (background_scaled) SDL_FreeSurface(background_scaled);
    if (pauseBtn) SDL_FreeSurface(pauseBtn);
    if (playBtn) SDL_FreeSurface(playBtn);
    if (pauseBtn_scaled) SDL_FreeSurface(pauseBtn_scaled);
    if (playBtn_scaled) SDL_FreeSurface(playBtn_scaled);
    if (minimap_bg_scaled) SDL_FreeSurface(minimap_bg_scaled);
    
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
    
    if (font) TTF_CloseFont(font);
    
    return nextState;
}