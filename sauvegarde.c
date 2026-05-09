#include "sauvegarde.h"
#include <stdio.h>
#include <string.h>

char pseudo_actuel[20] = "";
int message_erreur_pseudo = 0;

static int niveau_debloque = 1;
static int score_global = 0;
static int meilleurs_scores[5] = {0,0,0,0,0};

#define MAX_SAVES 100
typedef struct {
    char pseudo[20];
    int niveau;
    int score;
} SaveData;

SaveData saves[MAX_SAVES];
int nb_saves = 0;

void set_niveau_debloque(int niveau) { niveau_debloque = niveau; }
int get_niveau_debloque() { return niveau_debloque; }

void set_score_global_partie(int score) { score_global = score; }
int get_score_global_partie() { return score_global; }

void lire_toutes_sauvegardes() {
    FILE* f = fopen("sauvegardes.txt", "r");
    nb_saves = 0;
    if (!f) return;
    // Format : Pseudo Niveau Score
    while(fscanf(f, "%19s %d %d", saves[nb_saves].pseudo, &saves[nb_saves].niveau, &saves[nb_saves].score) == 3) {
        nb_saves++;
        if (nb_saves >= MAX_SAVES) break;
    }
    fclose(f);
}

void ecrire_toutes_sauvegardes() {
    FILE* f = fopen("sauvegardes.txt", "w");
    if (!f) return;
    for(int i=0; i<nb_saves; i++) {
        fprintf(f, "%s %d %d\n", saves[i].pseudo, saves[i].niveau, saves[i].score);
    }
    fclose(f);
}

void sauvegarder_partie(const char* pseudo, int niveau, int score) {
    if (strlen(pseudo) == 0) return;
    lire_toutes_sauvegardes();

    int index = -1;
    for(int i=0; i<nb_saves; i++) {
        if (strcmp(saves[i].pseudo, pseudo) == 0) {
            index = i; break;
        }
    }

    if (index != -1) {
        // Mise à jour si le pseudo existe déjà
        saves[index].niveau = niveau;
        if (score > saves[index].score) saves[index].score = score;
    } else {
        // Ajout d'une nouvelle sauvegarde
        if (nb_saves < MAX_SAVES) {
            strcpy(saves[nb_saves].pseudo, pseudo);
            saves[nb_saves].niveau = niveau;
            saves[nb_saves].score = score;
            nb_saves++;
        }
    }
    ecrire_toutes_sauvegardes();
}

int charger_partie_pseudo(const char* pseudo) {
    lire_toutes_sauvegardes();
    for(int i=0; i<nb_saves; i++) {
        if (strcmp(saves[i].pseudo, pseudo) == 0) {
            set_niveau_debloque(saves[i].niveau);
            set_score_global_partie(saves[i].score);
            return 1; // Pseudo trouvé !
        }
    }
    return 0; // Pseudo introuvable
}

// ---- SCORES ET COMPATIBILITE ----
void charger_scores() {
    FILE* f = fopen("scores.txt", "r");
    if (f) {
        for(int i=0; i<5; i++) fscanf(f, "%d", &meilleurs_scores[i]);
        fclose(f);
    }
}

void sauvegarder_scores() {
    FILE* f = fopen("scores.txt", "w");
    if (f) {
        for(int i=0; i<5; i++) fprintf(f, "%d\n", meilleurs_scores[i]);
        fclose(f);
    }
}

int* get_scores() { return meilleurs_scores; }

void ajouter_score(int score) {
    for (int i = 0; i < 5; i++) {
        if (score > meilleurs_scores[i]) {
            for (int j = 4; j > i; j--) meilleurs_scores[j] = meilleurs_scores[j - 1];
            meilleurs_scores[i] = score;
            break;
        }
    }
}

void ajouter_score_partie(int score) { score_global += score; }

void lire_infos_sauvegardes() {}
int get_info_save_existe(int slot) { return 0; }
