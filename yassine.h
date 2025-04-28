#ifndef HEADER_H
#define HEADER_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>


typedef struct {
    SDL_Surface *image;       
    SDL_Rect position;        
    int is_dragging;          
    int drag_offset_x;        
    int drag_offset_y;        
    int visible;              
} PuzzlePiece;

typedef struct {
    SDL_Surface *surface;     
    SDL_Rect position;        
} GameBackground;

typedef struct {
    SDL_Surface *surface;     
    SDL_Rect position;        
    int visible;              
} GameMessage;

#endif
