#include "integrated.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Structure for high score entry
typedef struct {
    char name[20];
    int score;
} ScoreEntry;

// Structure for button
typedef struct {
    SDL_Surface *image;
    SDL_Rect position;
} Button;

// Structure for background
typedef struct {
    SDL_Surface *image;
    SDL_Rect position;
} Background;

// Function to load image
static SDL_Surface *load_image(const char *filename) {
    SDL_Surface *image = IMG_Load(filename);
    if (!image) {
        printf("Error loading image %s: %s\n", filename, IMG_GetError());
    }
    return image;
}

// Function to display text
static void display_text(SDL_Surface *screen, TTF_Font *font, const char *text, int x, int y, SDL_Color color) {
    SDL_Surface *text_surface = TTF_RenderText_Blended(font, text, color);
    if (text_surface) {
        SDL_Rect position = {x, y, 0, 0};
        SDL_BlitSurface(text_surface, NULL, screen, &position);
        SDL_FreeSurface(text_surface);
    }
}

// Function to display button
static void display_button(SDL_Surface *screen, Button button) {
    SDL_BlitSurface(button.image, NULL, screen, &button.position);
}

// Function to check if a point is inside a button
static bool is_point_in_button(int x, int y, Button button) {
    return (x >= button.position.x && x <= button.position.x + button.image->w &&
            y >= button.position.y && y <= button.position.y + button.image->h);
}

// Function to save scores to file
static void save_scores(ScoreEntry scores[], int num_scores) {
    FILE *file = fopen("highscores.dat", "wb");
    if (file) {
        fwrite(scores, sizeof(ScoreEntry), num_scores, file);
        fclose(file);
    }
}

// Function to load scores from file
static int load_scores(ScoreEntry scores[], int max_scores) {
    FILE *file = fopen("highscores.dat", "rb");
    if (!file) {
        // Initialize with default scores if file doesn't exist
        strcpy(scores[0].name, "Champion");
        scores[0].score = 1000;
        strcpy(scores[1].name, "Runner-up");
        scores[1].score = 750;
        strcpy(scores[2].name, "Newcomer");
        scores[2].score = 500;
        return 3;
    }
    
    int num_scores = fread(scores, sizeof(ScoreEntry), max_scores, file);
    fclose(file);
    return num_scores;
}

// Function to add a new score and keep the list sorted
static void add_score(ScoreEntry scores[], int *num_scores, const char *name, int score, int max_scores) {
    // Skip if score is too low to be added
    if (*num_scores >= max_scores && score <= scores[max_scores - 1].score) {
        return;
    }
    
    // Find position to insert
    int pos = 0;
    while (pos < *num_scores && pos < max_scores && scores[pos].score > score) {
        pos++;
    }
    
    // Shift elements down
    if (pos < max_scores) {
for (int i = ((*num_scores < max_scores - 1) ? *num_scores : max_scores - 1) - 1; i >= pos; i--) {

            scores[i + 1] = scores[i];
        }
        
        // Insert new score
        strcpy(scores[pos].name, name);
        scores[pos].score = score;
        
        // Increment count if not at max
        if (*num_scores < max_scores) {
            (*num_scores)++;
        }
    }
}

GameState run_highscore(int current_score) {
    // Initialize SDL components
    SDL_Surface *screen = SDL_GetVideoSurface();
    
    // Set caption
    SDL_WM_SetCaption("High Scores", NULL);
    
    // Load font
    TTF_Font *font = TTF_OpenFont("arial.ttf", 30);
    SDL_Color white = {255, 255, 255};
    SDL_Color black = {0, 0, 0};
    
    // Load background
    Background bg = {load_image("bg3.png"), {0, 0}};
    
    // Load buttons
    Button validate_button = {load_image("Valider.png"), {790, 600}};
    Button back_button = {load_image("Retour.png"), {550, 615}};
    Button quit_button = {load_image("Quitter.png"), {1130, 600}};
    
    // Load star images
    SDL_Surface *star1 = load_image("etoile1.png");
    SDL_Surface *star2 = load_image("etoile2.png");
    SDL_Surface *star3 = load_image("etoile3.png");
    
    // Set star positions
    SDL_Rect star_pos1 = {660, 400, 0, 0};
    SDL_Rect star_pos2 = {660, 480, 0, 0};
    SDL_Rect star_pos3 = {660, 560, 0, 0};
    
    // Load high scores
    const int MAX_SCORES = 3;
    ScoreEntry scores[MAX_SCORES];
    int num_scores = load_scores(scores, MAX_SCORES);
    
    // Check if resources loaded correctly
    if (!font || !bg.image || !validate_button.image || !back_button.image || 
        !quit_button.image || !star1 || !star2 || !star3) {
        printf("Error loading resources\n");
        return STATE_MENU;
    }
    
    // Set up sound
    Mix_Chunk *click_sound = Mix_LoadWAV("click.wav");
    
    // State variables
    bool running = true;
    bool entering_name = false; // Start with showing scores by default
    bool new_high_score = false;
    GameState next_state = STATE_MENU;
    
    // Only check for high score if current_score > 0 (meaning came from gameplay)
    if (current_score > 0) {
        for (int i = 0; i < num_scores; i++) {
            if (current_score > scores[i].score) {
                new_high_score = true;
                entering_name = true; // Only enter name if it's a high score
                break;
            }
        }
        
        if (!new_high_score && num_scores < MAX_SCORES) {
            new_high_score = true;
            entering_name = true; // Only enter name if it's a high score
        }
    }
    
    // Text input variables
    char player_name[20] = "";
    SDL_Rect text_box = {750, 400, 400, 50};
    
    SDL_EnableUNICODE(1);  // Enable Unicode keyboard input
    
    // Main loop
    while (running) {
        SDL_Event event;
        
        // Draw background
        SDL_BlitSurface(bg.image, NULL, screen, &bg.position);
        
        if (entering_name && new_high_score) {
            // Name entry screen
            display_text(screen, font, "Enter Your Name", 750, 340, white);
            
            // Draw text box
            SDL_FillRect(screen, &text_box, SDL_MapRGB(screen->format, 255, 255, 255));
            display_text(screen, font, player_name, text_box.x + 10, text_box.y + 10, black);
            
            // Draw validate button
            display_button(screen, validate_button);
            
        } else {
            // High score display screen
            display_text(screen, font, "High Scores", 750, 340, white);
            
            // Display stars and scores
            SDL_BlitSurface(star1, NULL, screen, &star_pos1);
            SDL_BlitSurface(star2, NULL, screen, &star_pos2);
            SDL_BlitSurface(star3, NULL, screen, &star_pos3);
            
            char score_text[50];
            for (int i = 0; i < num_scores && i < MAX_SCORES; i++) {
                sprintf(score_text, "%s - %d", scores[i].name, scores[i].score);
                display_text(screen, font, score_text, star_pos1.x + 120, 
                           (i == 0 ? star_pos1.y : (i == 1 ? star_pos2.y : star_pos3.y)) + 23, white);
            }
            
            // Display buttons
            display_button(screen, back_button);
            display_button(screen, quit_button);
            
            // Display current score only if it's greater than 0
            if (current_score > 0) {
                sprintf(score_text, "Your Score: %d", current_score);
                display_text(screen, font, score_text, 750, 280, white);
            }
        }
        
        // Update display
        SDL_Flip(screen);
        
        // Handle events
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    next_state = STATE_EXIT;
                    break;
                    
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        int mouse_x = event.button.x;
                        int mouse_y = event.button.y;
                        
                        if (entering_name && is_point_in_button(mouse_x, mouse_y, validate_button)) {
                            // Add score and switch to display mode
                            if (strlen(player_name) > 0 && new_high_score) {
                                add_score(scores, &num_scores, player_name, current_score, MAX_SCORES);
                                save_scores(scores, num_scores);
                            }
                            entering_name = false;
                            if (click_sound) Mix_PlayChannel(-1, click_sound, 0);
                        } else if (!entering_name) {
                            if (is_point_in_button(mouse_x, mouse_y, back_button)) {
                                // Return to menu
                                running = false;
                                next_state = STATE_MENU;
                                if (click_sound) Mix_PlayChannel(-1, click_sound, 0);
                            } else if (is_point_in_button(mouse_x, mouse_y, quit_button)) {
                                // Exit game
                                running = false;
                                next_state = STATE_EXIT;
                                if (click_sound) Mix_PlayChannel(-1, click_sound, 0);
                            }
                        }
                    }
                    break;
                    
                case SDL_KEYDOWN:
                    if (entering_name) {
                        if (event.key.keysym.sym == SDLK_RETURN) {
                            // Submit name
                            if (strlen(player_name) > 0 && new_high_score) {
                                add_score(scores, &num_scores, player_name, current_score, MAX_SCORES);
                                save_scores(scores, num_scores);
                            }
                            entering_name = false;
                        } else if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(player_name) > 0) {
                            // Delete last character
                            player_name[strlen(player_name) - 1] = '\0';
                        } else if (strlen(player_name) < 19) {
                            // Add character if valid
                            char c = (char)event.key.keysym.unicode;
                            if (c >= ' ' && c <= '~') {  // Printable ASCII
                                player_name[strlen(player_name)] = c;
                                player_name[strlen(player_name) + 1] = '\0';
                            }
                        }
                    } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                        running = false;
                        next_state = STATE_MENU;
                    }
                    break;
            }
        }
    }
    
    // Clean up
    SDL_EnableUNICODE(0);  // 0 disables Unicode
    if (bg.image) SDL_FreeSurface(bg.image);
    if (validate_button.image) SDL_FreeSurface(validate_button.image);
    if (back_button.image) SDL_FreeSurface(back_button.image);
    if (quit_button.image) SDL_FreeSurface(quit_button.image);
    if (star1) SDL_FreeSurface(star1);
    if (star2) SDL_FreeSurface(star2);
    if (star3) SDL_FreeSurface(star3);
    if (click_sound) Mix_FreeChunk(click_sound);
    if (font) TTF_CloseFont(font);
    
    return next_state;
}