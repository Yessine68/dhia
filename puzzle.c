#include "integrated.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define PUZZLE_SIZE 4
#define MISSING_PIECES 4 
#define GAME_DURATION 30000
#define SCREEN_WIDTH 1800
#define SCREEN_HEIGHT 900
#define PUZZLE_IMAGE_SIZE 600

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    SDL_Surface *piece;
    SDL_Rect position;
    SDL_Rect correct_position;
    bool is_missing;
    bool is_dragging;
    int offset_x, offset_y;
} PuzzlePiece;

typedef struct {
    SDL_Rect bg_rect;
    SDL_Rect fill_rect;
    Uint32 start_time;
    bool is_active;
} TimerBar;

typedef struct {
    SDL_Rect rect;
    SDL_Surface *image;
} Button;

// Static variables for puzzle game
static SDL_Surface *screen = NULL;
static SDL_Surface *background = NULL;
static SDL_Surface *original_image = NULL;
static TTF_Font *font = NULL;
static int game_state = -1;

static SDL_Surface* victory_msg_surface = NULL;
static SDL_Surface* defeat_msg_surface = NULL;
static SDL_Surface* transformed_victory_msg = NULL;
static SDL_Surface* transformed_defeat_msg = NULL;
static Uint32 last_transform_time = 0;

static PuzzlePiece pieces[PUZZLE_SIZE * PUZZLE_SIZE];
static PuzzlePiece missing_pieces[MISSING_PIECES];
static TimerBar timer_bar;
static Button start_button, restart_button;

static Uint32 last_message_time = 0;
static char current_message[100] = "";
static SDL_Color current_message_color = {255, 255, 255, 0};

static Mix_Chunk *click_sound = NULL;
static Mix_Chunk *correct_sound = NULL;
static Mix_Chunk *timer_sound = NULL;
static Mix_Music *background_music = NULL;

// Utility functions for the puzzle game
static Uint32 get_pixel(SDL_Surface *surface, int x, int y) {
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    
    switch(bpp) {
        case 1: return *p;
        case 2: return *(Uint16 *)p;
        case 3: return SDL_BYTEORDER == SDL_BIG_ENDIAN ? 
            p[0] << 16 | p[1] << 8 | p[2] : p[0] | p[1] << 8 | p[2] << 16;
        case 4: return *(Uint32 *)p;
        default: return 0;
    }
}

static void put_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel) {
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
        case 1: *p = pixel; break;
        case 2: *(Uint16 *)p = pixel; break;
        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;
        case 4: *(Uint32 *)p = pixel; break;
    }
}

static SDL_Surface* rotate_zoom_surface(SDL_Surface* src, double angle, double zoom) {
    double rad_angle = angle * M_PI / 180.0;
    double sin_angle = sin(rad_angle);
    double cos_angle = cos(rad_angle);
    
    int new_width = (int)((fabs(src->w * cos_angle) + fabs(src->h * sin_angle)) * zoom);
    int new_height = (int)((fabs(src->w * sin_angle) + fabs(src->h * cos_angle)) * zoom);
    
    SDL_Surface* rotated = SDL_CreateRGBSurface(SDL_SWSURFACE, 
        new_width, new_height, 
        src->format->BitsPerPixel,
        src->format->Rmask,
        src->format->Gmask,
        src->format->Bmask,
        src->format->Amask);
    
    if(!rotated) return NULL;
    
    SDL_FillRect(rotated, NULL, SDL_MapRGBA(rotated->format, 0, 0, 0, 0));
    
    int center_x_src = src->w / 2;
    int center_y_src = src->h / 2;
    int center_x_dst = rotated->w / 2;
    int center_y_dst = rotated->h / 2;
    
    for(int y = 0; y < rotated->h; y++) {
        for(int x = 0; x < rotated->w; x++) {
            double src_x = (x - center_x_dst) / zoom;
            double src_y = (y - center_y_dst) / zoom;
            
            int orig_x = (int)(center_x_src + (src_x * cos_angle - src_y * sin_angle));
            int orig_y = (int)(center_y_src + (src_x * sin_angle + src_y * cos_angle));
            
            if(orig_x >= 0 && orig_x < src->w && orig_y >= 0 && orig_y < src->h) {
                Uint32 pixel = get_pixel(src, orig_x, orig_y);
                if(pixel != SDL_MapRGBA(src->format, 0, 0, 0, 0)) {
                    put_pixel(rotated, x, y, pixel);
                }
            }
        }
    }
    
    return rotated;
}

// Game functions for the puzzle
static void shuffle_pieces(void) {
    int used_indices[PUZZLE_SIZE * PUZZLE_SIZE] = {0};
    int piece_size = PUZZLE_IMAGE_SIZE / PUZZLE_SIZE;
    
    for(int i = 0; i < MISSING_PIECES; i++) {
            if(missing_pieces[i].piece) {
        printf("Missing piece %d created successfully, size: %dx%d\n", 
               i, missing_pieces[i].piece->w, missing_pieces[i].piece->h);
    } else {
        printf("ERROR: Failed to create missing piece %d\n", i);
    }
        int index;
        do {
            index = rand() % (PUZZLE_SIZE * PUZZLE_SIZE);
        } while(used_indices[index]);
        
        pieces[index].is_missing = true;
        used_indices[index] = 1;
        
        missing_pieces[i].piece = SDL_ConvertSurface(pieces[index].piece, screen->format, 0);
        missing_pieces[i].position.x = SCREEN_WIDTH - 250;
        missing_pieces[i].position.y = 100 + i * (piece_size + 20);
        missing_pieces[i].position.w = piece_size;
        missing_pieces[i].position.h = piece_size;
        missing_pieces[i].is_dragging = false;
    }
}

static void show_message(const char* text, SDL_Color color, int duration) {
    strncpy(current_message, text, sizeof(current_message)-1);
    current_message_color = color;
    last_message_time = SDL_GetTicks() + duration;
}

static bool check_puzzle_completion() {
    for(int i = 0; i < PUZZLE_SIZE * PUZZLE_SIZE; i++) {
        if(pieces[i].is_missing) return false;
    }
    game_state = 1;
    timer_bar.is_active = false;
    show_message("", (SDL_Color){0, 255, 0, 0}, 2000);
    return true;
}

static void init_puzzle() {
    srand(time(NULL));
    
    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
    if(!screen) {
        fprintf(stderr, "Erreur SDL_SetVideoMode: %s\n", SDL_GetError());
        return;
    }

    font = TTF_OpenFont("arial.ttf", 72);
    if(!font) font = TTF_OpenFont("DejaVuSans.ttf", 72);

    if(font) {
        victory_msg_surface = TTF_RenderText_Blended(font, "VICTORY!", (SDL_Color){100, 255, 100, 0});
        defeat_msg_surface = TTF_RenderText_Blended(font, "GAME OVER!", (SDL_Color){255, 100, 100, 0});
    }

    background = IMG_Load("background.png");
    if(!background) {
        background = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
        SDL_FillRect(background, NULL, SDL_MapRGB(background->format, 50, 50, 100));
    }

    original_image = IMG_Load("puzzle.png");
    if(!original_image) {
        original_image = SDL_CreateRGBSurface(SDL_SWSURFACE, PUZZLE_IMAGE_SIZE, PUZZLE_IMAGE_SIZE, 32, 0, 0, 0, 0);
        SDL_FillRect(original_image, NULL, SDL_MapRGB(original_image->format, rand()%256, rand()%256, rand()%256));
    }

    // Load sound effects
    click_sound = Mix_LoadWAV("click.wav");
    correct_sound = Mix_LoadWAV("correct.wav");
    timer_sound = Mix_LoadWAV("timer.wav");
    background_music = Mix_LoadMUS("music.mp3");
    
    if(background_music) {
        Mix_PlayMusic(background_music, -1);
    }

    /* Configure timer at top center */
    timer_bar.bg_rect = (SDL_Rect){
        (SCREEN_WIDTH - 400)/2,  // Centered
        20,                      // 20px from top
        400,                     // Width
        30                       // Height
    };
    timer_bar.fill_rect = timer_bar.bg_rect;
    timer_bar.is_active = false;

    start_button.rect = (SDL_Rect){(SCREEN_WIDTH - 200)/2, SCREEN_HEIGHT/2 - 50, 200, 80};
    start_button.image = IMG_Load("play.png");
    
    restart_button.rect = (SDL_Rect){(SCREEN_WIDTH - 200)/2, SCREEN_HEIGHT - 150, 200, 80};
    restart_button.image = IMG_Load("replay.png");

    int piece_size = PUZZLE_IMAGE_SIZE / PUZZLE_SIZE;
    int puzzle_x = (SCREEN_WIDTH - PUZZLE_IMAGE_SIZE)/2;
    int puzzle_y = (SCREEN_HEIGHT - PUZZLE_IMAGE_SIZE)/2;

    for(int y = 0; y < PUZZLE_SIZE; y++) {
        for(int x = 0; x < PUZZLE_SIZE; x++) {
            int index = y * PUZZLE_SIZE + x;
            pieces[index].piece = SDL_CreateRGBSurface(SDL_SWSURFACE, piece_size, piece_size, 32, 0, 0, 0, 0);
            
            SDL_Rect src = {x*piece_size, y*piece_size, piece_size, piece_size};
            SDL_BlitSurface(original_image, &src, pieces[index].piece, NULL);
            
            pieces[index].correct_position = (SDL_Rect){
                puzzle_x + x*piece_size,
                puzzle_y + y*piece_size,
                piece_size,
                piece_size
            };
            pieces[index].position = pieces[index].correct_position;
            pieces[index].is_missing = false;
        }
    }

    shuffle_pieces();
    game_state = -1;
}

static void clean_puzzle() {
    for(int i = 0; i < PUZZLE_SIZE * PUZZLE_SIZE; i++) {
        if(pieces[i].piece) SDL_FreeSurface(pieces[i].piece);
    }
    for(int i = 0; i < MISSING_PIECES; i++) {
        if(missing_pieces[i].piece) SDL_FreeSurface(missing_pieces[i].piece);
    }

    if(background) SDL_FreeSurface(background);
    if(original_image) SDL_FreeSurface(original_image);
    if(start_button.image) SDL_FreeSurface(start_button.image);
    if(restart_button.image) SDL_FreeSurface(restart_button.image);
    if(victory_msg_surface) SDL_FreeSurface(victory_msg_surface);
    if(defeat_msg_surface) SDL_FreeSurface(defeat_msg_surface);
    if(transformed_victory_msg) SDL_FreeSurface(transformed_victory_msg);
    if(transformed_defeat_msg) SDL_FreeSurface(transformed_defeat_msg);
    
    // Free sound resources
    if(click_sound) Mix_FreeChunk(click_sound);
    if(correct_sound) Mix_FreeChunk(correct_sound);
    if(timer_sound) Mix_FreeChunk(timer_sound);
    if(background_music) Mix_FreeMusic(background_music);
    
    // Reset values
    background = NULL;
    original_image = NULL;
    start_button.image = NULL;
    restart_button.image = NULL;
    victory_msg_surface = NULL;
    defeat_msg_surface = NULL;
    transformed_victory_msg = NULL;
    transformed_defeat_msg = NULL;
    click_sound = NULL;
    correct_sound = NULL;
    timer_sound = NULL;
    background_music = NULL;
}

static void render_timer() {
    if(!timer_bar.is_active) return;
    
    float progress = (float)(SDL_GetTicks() - timer_bar.start_time) / GAME_DURATION;
    if(progress > 1.0) progress = 1.0;
    
    timer_bar.fill_rect.w = timer_bar.bg_rect.w * (1.0 - progress);
    
    SDL_FillRect(screen, &timer_bar.bg_rect, SDL_MapRGB(screen->format, 50, 50, 50));
    
    SDL_Color color = {
        (Uint8)(255 * (progress < 0.5 ? progress * 2 : 1.0)),
        (Uint8)(255 * (progress < 0.5 ? 1.0 : (2.0 - progress * 2))),
        0,
        0
    };
    
    SDL_FillRect(screen, &timer_bar.fill_rect, SDL_MapRGB(screen->format, color.r, color.g, color.b));
    
    if(font) {
        char time_text[32];
        int remaining = (GAME_DURATION - (SDL_GetTicks() - timer_bar.start_time)) / 1000;
        snprintf(time_text, sizeof(time_text), "%ds", remaining < 0 ? 0 : remaining);
        
        TTF_Font* timer_font = TTF_OpenFont("arial.ttf", 24);
        if(!timer_font) timer_font = TTF_OpenFont("DejaVuSans.ttf", 24);
        
        if(timer_font) {
            SDL_Surface* text = TTF_RenderText_Blended(timer_font, time_text, (SDL_Color){255, 255, 255, 0});
            if(text) {
                SDL_Rect pos = {
                    timer_bar.bg_rect.x + (timer_bar.bg_rect.w - text->w)/2,
                    timer_bar.bg_rect.y + (timer_bar.bg_rect.h - text->h)/2,
                    text->w,
                    text->h
                };
                SDL_BlitSurface(text, NULL, screen, &pos);
                SDL_FreeSurface(text);
            }
            TTF_CloseFont(timer_font);
        }
    }
}

static void draw_button(Button* button) {
    if(button->image) {
        SDL_BlitSurface(button->image, NULL, screen, &button->rect);
    } else {
        SDL_Color base_color = (button == &restart_button) ? 
            (SDL_Color){180, 80, 80, 0} : (SDL_Color){80, 80, 180, 0};
        
        SDL_FillRect(screen, &button->rect, SDL_MapRGB(screen->format, base_color.r, base_color.g, base_color.b));
        
        if(font) {
            const char* text = (button == &start_button) ? "PLAY" : "REPLAY";
            SDL_Surface* text_surf = TTF_RenderText_Blended(font, text, (SDL_Color){255, 255, 255, 0});
            if(text_surf) {
                SDL_Rect text_pos = {
                    button->rect.x + (button->rect.w - text_surf->w)/2,
                    button->rect.y + (button->rect.h - text_surf->h)/2,
                    text_surf->w,
                    text_surf->h
                };
                SDL_BlitSurface(text_surf, NULL, screen, &text_pos);
                SDL_FreeSurface(text_surf);
            }
        }
    }
}

static int is_click_on_button(int x, int y, Button* button) {
    return (x >= button->rect.x && 
            x <= button->rect.x + button->rect.w && 
            y >= button->rect.y && 
            y <= button->rect.y + button->rect.h);
}

static void update_puzzle() {
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    if(background) SDL_BlitSurface(background, NULL, screen, NULL);

    if(game_state == 0) {
        render_timer();
    }

    switch(game_state) {
        case -1: {
            if(font) {
                SDL_Surface* title = TTF_RenderText_Blended(font, "Puzzle Challenge", (SDL_Color){255, 255, 255, 0});
                if(title) {
                    SDL_Rect pos = {(screen->w - title->w)/2, 100, title->w, title->h};
                    SDL_BlitSurface(title, NULL, screen, &pos);
                    SDL_FreeSurface(title);
                }
                
                // Display instruction text
                TTF_Font* instr_font = TTF_OpenFont("arial.ttf", 24);
                if(!instr_font) instr_font = TTF_OpenFont("DejaVuSans.ttf", 24);
                
                if(instr_font) {
                    SDL_Surface* instr = TTF_RenderText_Blended(instr_font, 
                        "Complete the puzzle to continue to level 2!", 
                        (SDL_Color){200, 200, 255, 0});
                    if(instr) {
                        SDL_Rect instr_pos = {(screen->w - instr->w)/2, 200, instr->w, instr->h};
                        SDL_BlitSurface(instr, NULL, screen, &instr_pos);
                        SDL_FreeSurface(instr);
                    }
                    TTF_CloseFont(instr_font);
                }
            }
            draw_button(&start_button);
            break;
        }
            
        case 0: {
            SDL_Rect frame = {
                (SCREEN_WIDTH - PUZZLE_IMAGE_SIZE)/2 - 10,
                (SCREEN_HEIGHT - PUZZLE_IMAGE_SIZE)/2 - 10,
                PUZZLE_IMAGE_SIZE + 20,
                PUZZLE_IMAGE_SIZE + 20
            };
            SDL_FillRect(screen, &frame, SDL_MapRGB(screen->format, 100, 100, 100));
            
            for(int i = 0; i < PUZZLE_SIZE*PUZZLE_SIZE; i++) {
                if(!pieces[i].is_missing) {
                    SDL_BlitSurface(pieces[i].piece, NULL, screen, &pieces[i].position);
                } else {
                    SDL_FillRect(screen, &pieces[i].correct_position, SDL_MapRGB(screen->format, 30, 30, 30));
                }
            }
            
            for(int i = 0; i < MISSING_PIECES; i++) {
                if(missing_pieces[i].piece) {
                    SDL_BlitSurface(missing_pieces[i].piece, NULL, screen, &missing_pieces[i].position);
                    // Draw border around the piece
                    SDL_Rect border = missing_pieces[i].position;
                    border.x--; border.y--; border.w += 2; border.h += 2;
                    SDL_FillRect(screen, &border, SDL_MapRGB(screen->format, 255, 255, 255));
                }
            }
            
            if(timer_bar.is_active && SDL_GetTicks() - timer_bar.start_time >= GAME_DURATION) {
                game_state = 2;
                timer_bar.is_active = false;
                show_message("", (SDL_Color){255, 0, 0, 0}, 2000);
                if(timer_sound) Mix_PlayChannel(-1, timer_sound, 0);
            }
            break;
        }
            
        case 1: { // Victory with rotozoom
            Uint32 current_time = SDL_GetTicks();
            if(current_time - last_transform_time > 30) {
                if(transformed_victory_msg) SDL_FreeSurface(transformed_victory_msg);
                
                float progress = (current_time % 2000) / 2000.0f;
                float angle = progress * 360.0f;
                float zoom = 1.0f + sin(progress * M_PI * 2) * 0.2f;
                
                transformed_victory_msg = rotate_zoom_surface(victory_msg_surface, angle, zoom);
                last_transform_time = current_time;
            }
            
            if(transformed_victory_msg) {
                SDL_Rect msg_rect = {
                    (screen->w - transformed_victory_msg->w)/2,
                    SCREEN_HEIGHT/3,
                    transformed_victory_msg->w,
                    transformed_victory_msg->h
                };
                SDL_BlitSurface(transformed_victory_msg, NULL, screen, &msg_rect);
            }
            
            // Instruction to continue
            if(font) {
                TTF_Font* cont_font = TTF_OpenFont("arial.ttf", 24);
                if(!cont_font) cont_font = TTF_OpenFont("DejaVuSans.ttf", 24);
                
                if(cont_font) {
                    SDL_Surface* continue_text = TTF_RenderText_Blended(cont_font, 
                        "Click anywhere to continue to Level 2", 
                        (SDL_Color){255, 255, 255, 0});
                    if(continue_text) {
                        SDL_Rect text_pos = {
                            (screen->w - continue_text->w)/2,
                            SCREEN_HEIGHT/2 + 50,
                            continue_text->w,
                            continue_text->h
                        };
                        SDL_BlitSurface(continue_text, NULL, screen, &text_pos);
                        SDL_FreeSurface(continue_text);
                    }
                    TTF_CloseFont(cont_font);
                }
            }
            break;
        }
            
        case 2: { // Defeat with rotozoom
            Uint32 current_time = SDL_GetTicks();
            if(current_time - last_transform_time > 30) {
                if(transformed_defeat_msg) SDL_FreeSurface(transformed_defeat_msg);
                
                float progress = (current_time % 2000) / 2000.0f;
                float angle = progress * -360.0f;
                float zoom = 1.0f + sin(progress * M_PI * 2) * 0.3f;
                
                transformed_defeat_msg = rotate_zoom_surface(defeat_msg_surface, angle, zoom);
                last_transform_time = current_time;
            }
            
            if(transformed_defeat_msg) {
                SDL_Rect msg_rect = {
                    (screen->w - transformed_defeat_msg->w)/2,
                    SCREEN_HEIGHT/3,
                    transformed_defeat_msg->w,
                    transformed_defeat_msg->h
                };
                SDL_BlitSurface(transformed_defeat_msg, NULL, screen, &msg_rect);
            }
            draw_button(&restart_button);
            break;
        }
    }

    if(SDL_GetTicks() < last_message_time && font) {
        SDL_Surface* msg = TTF_RenderText_Blended(font, current_message, current_message_color);
        if(msg) {
            SDL_Rect pos = {(screen->w - msg->w)/2, screen->h - 150, msg->w, msg->h};
            SDL_BlitSurface(msg, NULL, screen, &pos);
            SDL_FreeSurface(msg);
        }
    }
}

static GameState handle_puzzle_events() {
    SDL_Event event;
    GameState nextState = STATE_PUZZLE; // Default state is to stay in puzzle game
    
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT: 
                return STATE_EXIT;
                
            case SDL_KEYDOWN: 
                if(event.key.keysym.sym == SDLK_ESCAPE) {
                    // Allow exit to menu with ESC key
                    return STATE_MENU;
                }
                break;
                
            case SDL_MOUSEBUTTONDOWN:
                if(event.button.button == SDL_BUTTON_LEFT) {
                    int x = event.button.x;
                    int y = event.button.y;
                    
                    // Handle different game states
                    switch(game_state) {
                        case -1: // Start screen
                            if(is_click_on_button(x, y, &start_button)) {
                                if(click_sound) Mix_PlayChannel(-1, click_sound, 0);
                                game_state = 0;
                                timer_bar.start_time = SDL_GetTicks();
                                timer_bar.is_active = true;
                            }
                            break;
                            
                        case 0: // Gameplay
                            for(int i = 0; i < MISSING_PIECES; i++) {
                                if(missing_pieces[i].piece && 
                                   x >= missing_pieces[i].position.x && 
                                   x <= missing_pieces[i].position.x + missing_pieces[i].position.w &&
                                   y >= missing_pieces[i].position.y && 
                                   y <= missing_pieces[i].position.y + missing_pieces[i].position.h) {
                                   
                                    missing_pieces[i].is_dragging = true;
                                    missing_pieces[i].offset_x = x - missing_pieces[i].position.x;
                                    missing_pieces[i].offset_y = y - missing_pieces[i].position.y;
                                    if(click_sound) Mix_PlayChannel(-1, click_sound, 0);
                                    break;
                                }
                            }
                            break;
                            
                        case 1: // Victory screen - click anywhere to continue to Level 2
                            return STATE_LEVEL2;
                            
                        case 2: // Defeat screen - must click restart button
                            if(is_click_on_button(x, y, &restart_button)) {
                                if(click_sound) Mix_PlayChannel(-1, click_sound, 0);
                                game_state = -1;
                                // Reset the puzzle
                                clean_puzzle();
                                init_puzzle();
                            }
                            break;
                    }
                }
                break;
                
            case SDL_MOUSEMOTION:
                if(game_state == 0) {
                    for(int i = 0; i < MISSING_PIECES; i++) {
                        if(missing_pieces[i].is_dragging) {
                            missing_pieces[i].position.x = event.motion.x - missing_pieces[i].offset_x;
                            missing_pieces[i].position.y = event.motion.y - missing_pieces[i].offset_y;
                        }
                    }
                }
                break;
                
            case SDL_MOUSEBUTTONUP:
                if(event.button.button == SDL_BUTTON_LEFT && game_state == 0) {
                    for(int i = 0; i < MISSING_PIECES; i++) {
                        if(missing_pieces[i].is_dragging) {
                            missing_pieces[i].is_dragging = false;
                            bool placed = false;
                            
                            for(int j = 0; j < PUZZLE_SIZE * PUZZLE_SIZE; j++) {
                                if(pieces[j].is_missing) {
                                    int center_x = missing_pieces[i].position.x + missing_pieces[i].position.w/2;
                                    int center_y = missing_pieces[i].position.y + missing_pieces[i].position.h/2;
                                    
                                    if(center_x >= pieces[j].correct_position.x && 
                                       center_x <= pieces[j].correct_position.x + pieces[j].correct_position.w &&
                                       center_y >= pieces[j].correct_position.y && 
                                       center_y <= pieces[j].correct_position.y + pieces[j].correct_position.h) {
                                       
                                        pieces[j].position = pieces[j].correct_position;
                                        pieces[j].is_missing = false;
                                        SDL_FreeSurface(missing_pieces[i].piece);
                                        missing_pieces[i].piece = NULL;
                                        placed = true;
                                        if(correct_sound) Mix_PlayChannel(-1, correct_sound, 0);
                                        if(check_puzzle_completion()) {
                                            // Will transition to Level 2 on next click
                                        }
                                        break;
                                    }
                                }
                            }
                            
                            if(!placed) {
                                int piece_size = PUZZLE_IMAGE_SIZE / PUZZLE_SIZE;
                                missing_pieces[i].position.x = SCREEN_WIDTH - 250;
                                missing_pieces[i].position.y = 100 + i * (piece_size + 20);
                            }
                        }
                    }
                }
                break;
        }
    }
    
    return nextState;
}

GameState run_puzzle() {
    // Initialize puzzle game
    init_puzzle();
    
    GameState nextState = STATE_PUZZLE;
    bool running = true;
    
    while(running) {
        // Handle events
        nextState = handle_puzzle_events();
        if(nextState != STATE_PUZZLE) {
            running = false;
        }
        
        // Update game state
        update_puzzle();
        
        // Render
        SDL_Flip(screen);
        SDL_Delay(16);
    }
    
    // Clean up resources
    clean_puzzle();
    
    return nextState;
}