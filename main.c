#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "enigme.h"

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Surface *screen = SDL_SetVideoMode(1000, 800, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    SDL_Surface *background = IMG_Load("background.png");
    SDL_BlitSurface(background, NULL, screen, NULL);

    TTF_Font *font = TTF_OpenFont("Trajan Pro.ttf", 30);
    SDL_Color textColor = {255, 255, 255}; // blanc

    // Charger les questions
    charger_questions("questions.txt");
    Enigme e = generer_enigme();

    // -------- Affichage question centrée en haut --------
    SDL_Surface *question_txt = TTF_RenderText_Blended(font, e.question, textColor);
    SDL_Rect pos_question;
    pos_question.x = (screen->w - question_txt->w) / 2;
    pos_question.y = 80; // Entre les deux lignes de ta bordure supérieure
    SDL_BlitSurface(question_txt, NULL, screen, &pos_question);

    // -------- Affichage des 3 choix en bas et espacés --------
    SDL_Surface *choice[3];
    SDL_Rect pos_choice[3];
    int spacing = 80; // espace entre chaque choix
    int total_width = 0;

    // Créer les surfaces et calculer la largeur totale
    for (int i = 0; i < 3; i++) {
        choice[i] = TTF_RenderText_Blended(font, e.choices[i], textColor);
        total_width += choice[i]->w;
    }
    total_width += spacing * 2; // 2 espaces entre 3 éléments

    // Positionner horizontalement au centre
    int start_x = (screen->w - total_width) / 2;
    for (int i = 0; i < 3; i++) {
        pos_choice[i].x = start_x;
        pos_choice[i].y = 600; // zone du bas de ton image
        SDL_BlitSurface(choice[i], NULL, screen, &pos_choice[i]);
        start_x += choice[i]->w + spacing;
    }

    SDL_Flip(screen);

    // Boucle d'attente
    SDL_Event event;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = 0;
        }
    }

    // Libérations
    for (int i = 0; i < 3; i++) SDL_FreeSurface(choice[i]);
    SDL_FreeSurface(question_txt);
    SDL_FreeSurface(background);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 0;
}

