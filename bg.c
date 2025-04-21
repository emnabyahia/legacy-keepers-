#include "bg.h"
#include <stdlib.h> // For rand()

// Initialize background
int init_bg(background *bg) {
    bg->img = IMG_Load("bg2.jpeg");
    if (bg->img == NULL) {
        printf("Error: Failed to load background image 'bg2.jpeg'. Reason: %s\n", IMG_GetError());
        return -1; // Indicate failure
    }
    printf("Successfully loaded background image 'bg2.jpeg'\n");

    bg->pos.x = 0;
    bg->pos.y = 0;
    bg->pos.w = SCREEN_WIDTH;
    bg->pos.h = SCREEN_HEIGHT;
    return 0; // Indicate success
}

// Display background
void display_bg(background bg, SDL_Surface *screen) {
    if (bg.img == NULL) {
        printf("Error: Background surface is NULL.\n");
        return;
    }

    SDL_BlitSurface(bg.img, NULL, screen, &bg.pos);
}

// Scrolling logic
void scrolling(background *bg, int dest, int pas) {
    switch (dest) {
        case 0: // Up
            if (bg->pos.y > 0) bg->pos.y -= pas;
            break;
        case 1: // Down
            if (bg->pos.y + bg->pos.h < SCREEN_HEIGHT) bg->pos.y += pas;
            break;
        case 2: // Left
            if (bg->pos.x > 0) bg->pos.x -= pas;
            break;
        case 3: // Right
            if (bg->pos.x + bg->pos.w < SCREEN_WIDTH) bg->pos.x += pas;
            break;
    }
}

// Render text
SDL_Surface *render_Txt(const char *txt, SDL_Color txtC, TTF_Font *font) {
    return TTF_RenderText_Solid(font, txt, txtC);
}

// Display timer (replaces score)
void display_score(SDL_Surface *screen, TTF_Font *font, int min, int sec) {
    char time_txt[20];
    sprintf(time_txt, "Time: %02d:%02d", min, sec);

    SDL_Color txt_c = {255, 255, 255}; // White text
    SDL_Surface *time_sur = render_Txt(time_txt, txt_c, font);
    if (time_sur == NULL) {
        printf("Failed to render timer: %s\n", TTF_GetError());
        return;
    }

    SDL_Rect time_rect = {10, 10, time_sur->w, time_sur->h};
    SDL_BlitSurface(time_sur, NULL, screen, &time_rect);
    SDL_FreeSurface(time_sur);
}

// Timer display
void timer(int min, int sec, SDL_Surface *screen, TTF_Font *font) {
    char timer_txt[20];
    sprintf(timer_txt, "%02d:%02d", min, sec);

    SDL_Color txt_c = {255, 255, 255}; // White text
    SDL_Surface *timer_sur = render_Txt(timer_txt, txt_c, font);
    if (timer_sur == NULL) {
        printf("Failed to render timer: %s\n", TTF_GetError());
        return;
    }

    SDL_Rect timer_rect = {SCREEN_WIDTH - 10 - timer_sur->w, 10, timer_sur->w, timer_sur->h};
    SDL_BlitSurface(timer_sur, NULL, screen, &timer_rect);
    SDL_FreeSurface(timer_sur);
}

// Initialize animation with custom image path
int init_animation(animate *a, const char *image_path) {
    a->img1 = IMG_Load(image_path);
    if (a->img1 == NULL) {
        printf("Error: Failed to load '%s'. Reason: %s\n", image_path, IMG_GetError());
        return -1; // Indicate failure
    }
    printf("Successfully loaded '%s'\n", image_path);

    a->pos2.x = SCREEN_WIDTH / 2 - 50;  // Center horizontally (adjusted for scaled size)
    a->pos2.y = SCREEN_HEIGHT / 2 - 50; // Center vertically (adjusted for scaled size)
    a->comp = 0;
    return 0; // Indicate success
}

// Display animation
void display_animation(animate *a, SDL_Surface *screen) {
    if (a->img1 == NULL) {
        printf("Error: Animation surface is NULL.\n");
        return;
    }

    printf("Rendering player at (%d, %d) on surface with dimensions %dx%d\n",
           a->pos2.x, a->pos2.y, screen->w, screen->h);

    // Define the destination rectangle for rendering
    SDL_Rect dest_rect;
    dest_rect.x = a->pos2.x; // Default position
    dest_rect.y = a->pos2.y;

    // Adjust x position for the right surface
    if (screen->w == SCREEN_WIDTH / 2 && screen->h == SCREEN_HEIGHT) {
        dest_rect.x -= SCREEN_WIDTH / 2; // Shift x position for the right half
    }

    // Render the surface
    SDL_BlitSurface(a->img1, NULL, screen, &dest_rect);
}
