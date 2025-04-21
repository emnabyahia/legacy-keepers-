#include "perso.h"
#include <stdio.h>
#include <math.h>

#define ANIM_SPEED 150
#define GROUND_Y (SCREEN_HEIGHT - SPRITE_HEIGHT - 10)



void initPerso(Personne* p, SDL_Surface* spriteSheet) {
    p->spriteSheet = spriteSheet;
    p->x = 0;
    p->y = GROUND_Y;
    p->row = 0;
    p->currentFrame = 0;
    p->vely = 0;
    p->direction = 1;
    p->flip = 0;
    p->vie = 3;
    p->collisionActive = 0;

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 6; ++j) {
            SDL_Rect* clip = &p->spriteClips[i * 6 + j];
            clip->x = j * SPRITE_WIDTH;
            clip->y = i * SPRITE_HEIGHT;
            clip->w = SPRITE_WIDTH;
            clip->h = SPRITE_HEIGHT;
        }
    }
}

void animerPerso(Personne* p, Uint32 now, Uint32* lastUpdate) {
    if (now - *lastUpdate > ANIM_SPEED) {
        p->currentFrame = (p->currentFrame + 1) % 6;
        *lastUpdate = now;
    }
}

void movePersoAuto(Personne* p, int level) {
    if (p->vie <= 0) return;

    p->x += 2 * p->direction;

    if (level == 2) {
        p->y = GROUND_Y + (int)(10 * sin(SDL_GetTicks() / 200.0));
    } else {
        p->y = GROUND_Y;
    }

    if (p->x <= 0 || p->x >= SCREEN_WIDTH - SPRITE_WIDTH) {
        p->direction *= -1;
        p->flip = (p->direction == -1) ? 1 : 0;
    }
}

void afficherPerso(Personne* p, SDL_Surface* screen) {
    SDL_Rect src = p->spriteClips[p->row * 6 + p->currentFrame];
    SDL_Rect dest = { p->x, p->y, SPRITE_WIDTH, SPRITE_HEIGHT };

    if (!p->flip) {
        SDL_BlitSurface(p->spriteSheet, &src, screen, &dest);
    } else {
        SDL_Surface* flipped = SDL_CreateRGBSurface(
            SDL_SWSURFACE,
            SPRITE_WIDTH,
            SPRITE_HEIGHT,
            p->spriteSheet->format->BitsPerPixel,
            p->spriteSheet->format->Rmask,
            p->spriteSheet->format->Gmask,
            p->spriteSheet->format->Bmask,
            p->spriteSheet->format->Amask
        );

        SDL_LockSurface(p->spriteSheet);
        SDL_LockSurface(flipped);
        Uint32* srcPixels = (Uint32*)p->spriteSheet->pixels;
        Uint32* dstPixels = (Uint32*)flipped->pixels;
        int pitch = p->spriteSheet->pitch / 4;
        for (int y = 0; y < SPRITE_HEIGHT; ++y) {
            for (int x = 0; x < SPRITE_WIDTH; ++x) {
                int sx = src.x + x;
                int sy = src.y + y;
                Uint32 pixel = srcPixels[sy * pitch + sx];
                int fx = SPRITE_WIDTH - 1 - x;
                dstPixels[y * SPRITE_WIDTH + fx] = pixel;
            }
        }
        SDL_UnlockSurface(flipped);
        SDL_UnlockSurface(p->spriteSheet);
        SDL_SetColorKey(flipped, SDL_SRCCOLORKEY | SDL_RLEACCEL,
                        SDL_MapRGB(flipped->format, 255, 255, 255));
        SDL_BlitSurface(flipped, NULL, screen, &dest);
        SDL_FreeSurface(flipped);
    }
}

void initObjets(Objet objets[], int count) {
    objets[0].pos.x = 150;
    objets[1].pos.x = 500; 

    for (int i = 0; i < count; i++) {
        objets[i].pos.y = GROUND_Y + 90;
        objets[i].pos.w = 58;
        objets[i].pos.h = 52;
        objets[i].floatOffset = 0;
        objets[i].floatDirection = 1;
    }
}

void updateObjets(Objet objets[], int count) {
    for (int i = 0; i < count; i++) {
        objets[i].floatOffset += objets[i].floatDirection;
        if (objets[i].floatOffset > 5 || objets[i].floatOffset < -5)
            objets[i].floatDirection *= -1;
        objets[i].pos.y = GROUND_Y + 90 + objets[i].floatOffset;
    }
}

void afficherObjets(Objet objets[], SDL_Surface* img, SDL_Surface* screen) {
    for (int i = 0; i < MAX_OBJETS; i++) {
        SDL_BlitSurface(img, NULL, screen, &objets[i].pos);
    }
}

int checkCollision(SDL_Rect a, SDL_Rect b) {
    if (a.x + a.w < b.x) return 0;
    if (a.x > b.x + b.w) return 0;
    if (a.y + a.h < b.y) return 0;
    if (a.y > b.y + b.h) return 0;
    return 1;
}

void afficherTexteMort(SDL_Surface* screen) {
    TTF_Font* font = TTF_OpenFont("greek_freak.ttf", 36); // Assure-toi que greek_freak.ttf est présent
    if (!font) {
        printf("Erreur chargement police : %s\n", TTF_GetError());
        return;
    }

    SDL_Color rouge = {255, 0, 0};
    SDL_Surface* texte = TTF_RenderText_Solid(font, "ENEMY DEAD", rouge);

    SDL_Rect pos;
    pos.x = SCREEN_WIDTH / 2 - texte->w / 2;
    pos.y = SCREEN_HEIGHT / 2 - texte->h / 2;

    SDL_BlitSurface(texte, NULL, screen, &pos);
    SDL_FreeSurface(texte);
    TTF_CloseFont(font);
}

void afficherVie(Personne* p, SDL_Surface* screen, SDL_Surface* coeur) {
    SDL_Rect src, dest;
    src.y = 0;
    src.w = 60;  // Largeur du cœur
    src.h = 48;  // Hauteur du cœur

    for (int i = 0; i < p->vie; i++) {  // Affiche les cœurs restants
        src.x = i * 60; // Position de chaque cœur dans l'image
        dest.x = 10 + i * 60;  // Espacement entre les cœurs
        dest.y = 10;
        SDL_BlitSurface(coeur, &src, screen, &dest);
    }
}

