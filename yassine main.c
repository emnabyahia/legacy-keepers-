#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <time.h>
#include "header.h"

// Game constants
#define TARGET_THRESHOLD 30    
#define NUM_PIECES 4           
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800
#define LEFT_SIDE_X 100        
#define RIGHT_SIDE_X 800       


typedef struct {
    int x;
    int y;
} TargetPosition;


void initGame(GameBackground *bg, PuzzlePiece pieces[], int puzzle_set, GameMessage *msg, TTF_Font *font);
void cleanup(GameBackground *bg, PuzzlePiece pieces[], GameMessage *msg, TTF_Font *font);
int handleEvents(PuzzlePiece pieces[], int puzzle_set, GameMessage *msg, int *game_active);
void renderGame(SDL_Surface *screen, GameBackground *bg, PuzzlePiece pieces[], GameMessage *msg);
int checkPuzzleComplete(PuzzlePiece *piece, int puzzle_set, GameMessage *msg, PuzzlePiece pieces[]);
TargetPosition getTargetPosition(int puzzle_set);
int isMouseOverPiece(PuzzlePiece *piece, int mouse_x, int mouse_y);
int isInTargetPosition(PuzzlePiece *piece, int puzzle_set);
void snapToTarget(PuzzlePiece *piece, int puzzle_set);
void initMessage(GameMessage *msg, TTF_Font *font, SDL_Color color, const char *text);

int main(int argc, char** argv) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "SDL_image initialization failed: %s\n", IMG_GetError());
        return EXIT_FAILURE;
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "SDL_ttf initialization failed: %s\n", TTF_GetError());
        return EXIT_FAILURE;
    }


    SDL_Surface *screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE);
    if (!screen) {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    TTF_Font *font = TTF_OpenFont("arial.ttf", 48);
    if (!font) {
        fprintf(stderr, "Font loading failed: %s\n", TTF_GetError());
        return EXIT_FAILURE;
    }


    GameBackground background;
    PuzzlePiece pieces[NUM_PIECES];
    GameMessage victory_msg;
    int game_active = 1;
    

    srand(time(NULL));
    int puzzle_set = rand() % 5 + 1;
    printf("Using puzzle set: %d\n", puzzle_set);


    initGame(&background, pieces, puzzle_set, &victory_msg, font);


    while (game_active) {
        game_active = handleEvents(pieces, puzzle_set, &victory_msg, &game_active);
        renderGame(screen, &background, pieces, &victory_msg);
        SDL_Delay(10); // Small delay to reduce CPU usage
    }


    cleanup(&background, pieces, &victory_msg, font);
    return EXIT_SUCCESS;
}

void initGame(GameBackground *bg, PuzzlePiece pieces[], int puzzle_set, GameMessage *msg, TTF_Font *font) {

    bg->surface = IMG_Load("background.jpg");
    if (!bg->surface) {
        fprintf(stderr, "Background loading failed: %s\n", IMG_GetError());
        exit(EXIT_FAILURE);
    }
    bg->position = (SDL_Rect){0, 0, bg->surface->w, bg->surface->h};


    char path[100];
    for (int i = 0; i < NUM_PIECES; i++) {
        sprintf(path, "puzzlerand/%d/puzzle%d.png", puzzle_set, i+1);
        pieces[i].image = IMG_Load(path);
        if (!pieces[i].image) {
            fprintf(stderr, "Puzzle piece %d loading failed: %s\n", i+1, IMG_GetError());
            exit(EXIT_FAILURE);
        }
        pieces[i].position.w = pieces[i].image->w;
        pieces[i].position.h = pieces[i].image->h;
        pieces[i].is_dragging = 0;
        pieces[i].drag_offset_x = 0;
        pieces[i].drag_offset_y = 0;
        pieces[i].visible = 1;
    }


    pieces[0].position = (SDL_Rect){LEFT_SIDE_X, 200, 0, 0};  
    pieces[1].position = (SDL_Rect){LEFT_SIDE_X, 350, 0, 0};  
    pieces[2].position = (SDL_Rect){LEFT_SIDE_X, 500, 0, 0};  
    
    
    TargetPosition target = getTargetPosition(puzzle_set);
    pieces[3].position = (SDL_Rect){target.x, target.y, 0, 0};
    pieces[3].visible = 0; 

    initMessage(msg, font, (SDL_Color){255, 215, 0, 255}, "BRAVO! Puzzle Complete!");
}

void cleanup(GameBackground *bg, PuzzlePiece pieces[], GameMessage *msg, TTF_Font *font) {
    // Free surfaces
    SDL_FreeSurface(bg->surface);
    for (int i = 0; i < NUM_PIECES; i++) {
        if (pieces[i].image) {
            SDL_FreeSurface(pieces[i].image);
        }
    }
    if (msg->surface) {
        SDL_FreeSurface(msg->surface);
    }

    TTF_CloseFont(font);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

int handleEvents(PuzzlePiece pieces[], int puzzle_set, GameMessage *msg, int *game_active) {
    SDL_Event event;
    static PuzzlePiece *dragged_piece = NULL;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                *game_active = 0;
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT && !msg->visible) {
                    for (int i = 0; i < NUM_PIECES - 1; i++) { 
                        if (isMouseOverPiece(&pieces[i], event.button.x, event.button.y)) {
                            dragged_piece = &pieces[i];
                            dragged_piece->drag_offset_x = event.button.x - dragged_piece->position.x;
                            dragged_piece->drag_offset_y = event.button.y - dragged_piece->position.y;
                            break;
                        }
                    }
                }
                break;

            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT && dragged_piece) {
                    if (checkPuzzleComplete(dragged_piece, puzzle_set, msg, pieces)) {
                        // Puzzle completed - wait for 2 seconds before exiting
                        SDL_Delay(2000);
                        *game_active = 0;
                    }
                    dragged_piece = NULL;
                }
                break;

            case SDL_MOUSEMOTION:
                if (dragged_piece && !msg->visible) {
                    dragged_piece->position.x = event.motion.x - dragged_piece->drag_offset_x;
                    dragged_piece->position.y = event.motion.y - dragged_piece->drag_offset_y;
                }
                break;
        }
    }

    return *game_active;
}

void renderGame(SDL_Surface *screen, GameBackground *bg, PuzzlePiece pieces[], GameMessage *msg) {

    SDL_BlitSurface(bg->surface, NULL, screen, &bg->position);


    for (int i = 0; i < NUM_PIECES; i++) {
        if (pieces[i].visible) {
            SDL_BlitSurface(pieces[i].image, NULL, screen, &pieces[i].position);
        }
    }

    // Draw victory message if visible
    if (msg->visible) {
        SDL_BlitSurface(msg->surface, NULL, screen, &msg->position);
    }

    // Update screen
    SDL_Flip(screen);
}

int checkPuzzleComplete(PuzzlePiece *piece, int puzzle_set, GameMessage *msg, PuzzlePiece pieces[]) {
    if (isInTargetPosition(piece, puzzle_set)) {
        snapToTarget(piece, puzzle_set);
        msg->visible = 1;
        
        // Hide other pieces
        for (int i = 0; i < NUM_PIECES - 1; i++) {
            if (&pieces[i] != piece) {
                pieces[i].visible = 0;
            }
        }
        
        // Show target piece
        pieces[NUM_PIECES - 1].visible = 1;
        return 1; // Puzzle completed
    }
    return 0; // Puzzle not completed
}


TargetPosition getTargetPosition(int puzzle_set) {
    TargetPosition pos;
    switch(puzzle_set) {
        case 1: pos.x = RIGHT_SIDE_X; pos.y = 200; break;
        case 2: pos.x = RIGHT_SIDE_X; pos.y = 300; break;
        case 3: pos.x = RIGHT_SIDE_X; pos.y = 400; break;
        case 4: pos.x = RIGHT_SIDE_X; pos.y = 500; break;
        case 5: pos.x = RIGHT_SIDE_X; pos.y = 600; break;
        default: pos.x = RIGHT_SIDE_X; pos.y = 350; break;
    }
    return pos;
}

// Helper function to check if mouse is over a piece
int isMouseOverPiece(PuzzlePiece *piece, int mouse_x, int mouse_y) {
    return piece->visible && 
           mouse_x >= piece->position.x && 
           mouse_x <= piece->position.x + piece->position.w &&
           mouse_y >= piece->position.y && 
           mouse_y <= piece->position.y + piece->position.h;
}

int isInTargetPosition(PuzzlePiece *piece, int puzzle_set) {
    TargetPosition target = getTargetPosition(puzzle_set);
    return abs(piece->position.x - target.x) < TARGET_THRESHOLD &&
           abs(piece->position.y - target.y) < TARGET_THRESHOLD;
}

void snapToTarget(PuzzlePiece *piece, int puzzle_set) {
    TargetPosition target = getTargetPosition(puzzle_set);
    piece->position.x = target.x;
    piece->position.y = target.y;
}


void initMessage(GameMessage *msg, TTF_Font *font, SDL_Color color, const char *text) {
    msg->surface = TTF_RenderText_Solid(font, text, color);
    if (!msg->surface) {
        fprintf(stderr, "Text rendering failed: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
    msg->position = (SDL_Rect){400, 550, msg->surface->w, msg->surface->h};
    msg->visible = 0;
}
