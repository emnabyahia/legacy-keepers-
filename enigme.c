#include "enigme.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Variables globales internes
Enigme questions[MAX_QUESTIONS];
int used_indices[MAX_QUESTIONS];
int total_questions = 0;
int used_count = 0;

void charger_questions(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erreur ouverture fichier");
        exit(1);
    }

    char line[MAX_LINE_LEN];
    while (fgets(line, sizeof(line), file) && total_questions < MAX_QUESTIONS) {
        char *token = strtok(line, ";");
        if (!token) continue;

        strncpy(questions[total_questions].question, token, sizeof(questions[total_questions].question));

        for (int i = 0; i < 3; i++) {
            token = strtok(NULL, ";");
            if (!token) break;
            strncpy(questions[total_questions].choices[i], token, sizeof(questions[total_questions].choices[i]));
        }

        token = strtok(NULL, ";");
        if (token)
            questions[total_questions].correct_choice = atoi(token);

        total_questions++;
    }

    fclose(file);
    srand(time(NULL));
}

int est_utilise(int index) {
    for (int i = 0; i < used_count; i++) {
        if (used_indices[i] == index) return 1;
    }
    return 0;
}

Enigme generer_enigme() {
    if (used_count >= total_questions) {
        printf("Toutes les questions ont été utilisées.\n");
        exit(0);
    }

    int index;
    do {
        index = rand() % total_questions;
    } while (est_utilise(index));

    used_indices[used_count++] = index;
    return questions[index];
}

