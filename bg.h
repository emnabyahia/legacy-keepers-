#ifndef BG_H
#define BG_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

// Constants
#define SCREEN_WIDTH 1151
#define SCREEN_HEIGHT 1080
#define ANIM_STEP_X 10  // Horizontal step size for scrolling

// Background structure
typedef struct {
    SDL_Rect pos;
    SDL_Surface *img;
    int anim;
    SDL_Surface *anima[50];
    SDL_Rect pos1;
} background;

// Animation structure
typedef struct {
    SDL_Surface *img1;
    SDL_Rect pos2;
    int comp;
} animate;

// Function prototypes
int init_bg(background *bg); // Initialize background
void display_bg(background bg, SDL_Surface *screen); // Display background
void scrolling(background *bg, int dest, int pas); // Scrolling logic
SDL_Surface *render_Txt(const char *txt, SDL_Color txtC, TTF_Font *font); // Render text
void display_score(SDL_Surface *screen, TTF_Font *font, int min, int sec); // Display timer
void timer(int min, int sec, SDL_Surface *screen, TTF_Font *font); // Timer display
int init_animation(animate *a, const char *image_path); // Initialize animation with custom image path
void display_animation(animate *a, SDL_Surface *screen); // Display animation

#endif
