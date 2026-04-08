#include "sauvegarde.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_SCORES 5
static int top_scores[MAX_SCORES] = {0};

static int niveau_debloque = 1;
static int score_global_partie = 0;
static int slot_actuel = 1;

typedef struct { int existe; int niveau; int score; } InfoSave;
static InfoSave saves[4]; // 4 slots de sauvegarde

// -- SCORES GLOBAUX --
void charger_scores() {
    FILE* f = fopen("scores.txt", "r");
    if (f != NULL) {
        for(int i = 0; i < MAX_SCORES; i++) fscanf(f, "%d\n", &top_scores[i]);
        fclose(f);
    }
}

void sauvegarder_scores() {
    FILE* f = fopen("scores.txt", "w");
    if (f != NULL) {
        for(int i = 0; i < MAX_SCORES; i++) fprintf(f, "%d\n", top_scores[i]);
        fclose(f);
    }
}

void ajouter_score(int nouveau_score) {
    if (nouveau_score <= 0) return;
    for (int i = 0; i < MAX_SCORES; i++) {
        if (nouveau_score > top_scores[i]) {
            for (int j = MAX_SCORES - 1; j > i; j--) top_scores[j] = top_scores[j-1];
            top_scores[i] = nouveau_score;
            break;
        }
    }
    sauvegarder_scores();
}

int* get_scores() { return top_scores; }

// -- PROGRESSION MULTI-SLOTS --
void lire_infos_sauvegardes() {
    for(int i = 0; i < 4; i++) {
        char filename[20];
        sprintf(filename, "save_%d.txt", i+1);
        FILE* f = fopen(filename, "r");
        if (f) {
            saves[i].existe = 1;
            fscanf(f, "%d %d", &saves[i].niveau, &saves[i].score);
            fclose(f);
        } else {
            saves[i].existe = 0;
            saves[i].niveau = 1;
            saves[i].score = 0;
        }
    }
}

int get_info_save_existe(int slot) { return saves[slot-1].existe; }
int get_info_save_niveau(int slot) { return saves[slot-1].niveau; }
int get_info_save_score(int slot) { return saves[slot-1].score; }

void creer_nouvelle_partie() {
    // Cherche le premier emplacement vide
    slot_actuel = 1;
    for(int i = 0; i < 4; i++) {
        if(!saves[i].existe) {
            slot_actuel = i+1;
            break;
        }
    }
    // Si tout est plein, ça écrase le slot 1 par défaut.
    niveau_debloque = 1;
    score_global_partie = 0;
    sauvegarder_progression();
}

void charger_partie(int slot) {
    slot_actuel = slot;
    char filename[20];
    sprintf(filename, "save_%d.txt", slot);
    FILE* f = fopen(filename, "r");
    if (f) {
        fscanf(f, "%d %d", &niveau_debloque, &score_global_partie);
        fclose(f);
    }
}

void sauvegarder_progression() {
    char filename[20];
    sprintf(filename, "save_%d.txt", slot_actuel);
    FILE* f = fopen(filename, "w");
    if (f != NULL) {
        fprintf(f, "%d %d\n", niveau_debloque, score_global_partie);
        fclose(f);
    }
    lire_infos_sauvegardes(); // MAJ visuelle pour le menu
}

int get_niveau_debloque() { return niveau_debloque; }
void set_niveau_debloque(int niv) {
    if (niv > niveau_debloque) {
        niveau_debloque = niv;
        sauvegarder_progression();
    }
}

int get_score_global_partie() { return score_global_partie; }
void ajouter_score_partie(int points) {
    score_global_partie += points;
    sauvegarder_progression();
}
