#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "perso.h"


void convertirEnNoirEtBlanc(SDL_Surface* screen) {
    SDL_LockSurface(screen);
    Uint32* pixels = (Uint32*)screen->pixels;
    int width = screen->w;
    int height = screen->h;

    for (int i = 0; i < width * height; i++) {
        Uint32 pixel = pixels[i];
        Uint8 r, g, b;
        SDL_GetRGB(pixel, screen->format, &r, &g, &b);
        Uint8 gray = (Uint8)(0.299 * r + 0.587 * g + 0.114 * b); // Calcul de la luminosité
        pixels[i] = SDL_MapRGB(screen->format, gray, gray, gray); // Applique la couleur grise
    }

    SDL_UnlockSurface(screen);
}

int main(int argc, char* argv[]) {
    SDL_Surface *screen = NULL, *bg1 = NULL, *bg2 = NULL, *coeur = NULL, *objetImg = NULL;
    SDL_Event event;
    Personne perso;
    Objet objets[MAX_OBJETS];
    int level = 1;
    Uint32 lastUpdate = 0;
    int running = 1;

    // Initialisation SDL
    SDL_Init(SDL_INIT_VIDEO);
	if (TTF_Init() == -1) {
	    printf("Erreur d'initialisation de SDL_ttf : %s\n", TTF_GetError());
	    return 1;
	}

    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE);

    // Chargement des ressources
    bg1 = IMG_Load("backg.png");  // Fond du niveau 1
    bg2 = IMG_Load("backg2.png"); // Fond du niveau 2
    coeur = IMG_Load("h1.png");   // Image des cœurs de santé
    objetImg = IMG_Load("objet.png"); // Objet flottant
    SDL_Surface* sheet = IMG_Load("spritesheet.png");

    if (!sheet) {
        printf("Erreur chargement spritesheet : %s\n", IMG_GetError());
        return 1;
    } else {
        Uint32 key = SDL_MapRGB(sheet->format, 255, 255, 255); // Color key pour transparence
        SDL_SetColorKey(sheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, key);
    }

    // Initialisation des objets et de la momie
    initPerso(&perso, sheet);
    initObjets(objets, MAX_OBJETS);

    SDL_Rect pos_bg = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}; // Position de l’arrière-plan

    while (running) {
        Uint32 now = SDL_GetTicks();

        // Gestion des événements
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_l)  // Touche 'L' pour changer de niveau
                    level = (level == 1) ? 2 : 1;
            }
        }

        // Déplacement et animation automatique de la momie
        movePersoAuto(&perso, level);
        animerPerso(&perso, now, &lastUpdate);
        updateObjets(objets, MAX_OBJETS);

        // Collision avec les objets flottants
        perso.row = 0;  // Par défaut la momie marche
        SDL_Rect ennemiBox = {perso.x, perso.y, SPRITE_WIDTH, SPRITE_HEIGHT};

        for (int i = 0; i < MAX_OBJETS; i++) {
            if (checkCollision(ennemiBox, objets[i].pos)) {
                perso.row = 2;  // La momie entre en crouch
                perso.currentFrame = 0;
            }
        }

        // Remplissage de l'écran et affichage des éléments
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0)); // Fond noir
        SDL_BlitSurface((level == 1) ? bg1 : bg2, NULL, screen, &pos_bg); // Affichage du fond selon le niveau
        afficherObjets(objets, objetImg, screen); // Affichage des objets flottants
        afficherPerso(&perso, screen); // Affichage de la momie

        // Affichage de la santé de la momie
        afficherVie(&perso, screen, coeur);


	if (perso.x >= SCREEN_WIDTH - SPRITE_WIDTH) {
	    if (!perso.collisionActive && perso.vie > 0) {
		perso.vie--; // Diminue un cœur
		perso.collisionActive = 1; // Empêche de diminuer plusieurs fois
	    }
	} else {
	    perso.collisionActive = 0; // Réinitialise la détection de collision une fois que la momie n'est plus sur le bord
	}

        // Affichage du texte "ENEMY DEAD" quand la vie de la momie est à 0
	if (perso.vie == 0) {
   	 convertirEnNoirEtBlanc(screen); // Applique l'effet noir et blanc
    	afficherTexteMort(screen); // Affiche le texte "ENEMY DEAD"
}

        SDL_Flip(screen); // Mise à jour de l'écran
        SDL_Delay(16); // Limite à environ 60 FPS
    }

    // Libération des ressources
    SDL_FreeSurface(bg1);
    SDL_FreeSurface(bg2);
    SDL_FreeSurface(coeur);
    SDL_FreeSurface(objetImg);
    SDL_FreeSurface(sheet);
    TTF_Quit();
    SDL_Quit();

    return 0;
}



