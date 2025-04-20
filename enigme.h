#ifndef ENIGME_H
#define ENIGME_H

#include <stdio.h>

#define MAX_QUESTIONS 100
#define MAX_LINE_LEN 256

typedef struct {
    char question[128];
    char choices[3][64];
    int correct_choice;
} Enigme;

// Fonctions accessibles depuis d'autres fichiers
void charger_questions(const char *filename);
Enigme generer_enigme(void);

#endif

