#ifndef PERSO_H
#define PERSO_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 480

#define SPRITE_WIDTH  97
#define SPRITE_HEIGHT 150

#define MAX_OBJETS 2

typedef struct {
    int x, y;
    int row;
    int currentFrame;
    int vely;
    int direction;
    int flip;
    int vie;
    int collisionActive;
    SDL_Rect spriteClips[18]; // 3 lignes * 6 colonnes
    SDL_Surface* spriteSheet;
} Personne;

typedef struct {
    SDL_Rect pos;
    int floatOffset;
    int floatDirection;
} Objet;

void initPerso(Personne* p, SDL_Surface* spriteSheet);
void animerPerso(Personne* p, Uint32 now, Uint32* lastUpdate);
void movePersoAuto(Personne* p, int level);
void afficherPerso(Personne* p, SDL_Surface* screen);
void afficherVie(Personne* p, SDL_Surface* screen, SDL_Surface* coeur);

void initObjets(Objet objets[], int count);
void updateObjets(Objet objets[], int count);
void afficherObjets(Objet objets[], SDL_Surface* img, SDL_Surface* screen);
int checkCollision(SDL_Rect a, SDL_Rect b);

void afficherTexteMort(SDL_Surface* screen);

#endif
