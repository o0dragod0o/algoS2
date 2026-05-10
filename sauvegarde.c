#include "sauvegarde.h"
#include <stdio.h>
#include <string.h>

// ==========================================
// VARIABLES EN MÉMOIRE (RAM)
// ==========================================

char pseudo_actuel[20] = "";
int message_erreur_pseudo = 0;

int niveau_debloque = 1;
int score_global = 0;

// Structure représentant une ligne dans le fichier texte
#define MAX_SAVES 100
typedef struct {
    char pseudo[20];
    int niveau;
    int score;
} SaveData;

// Tableau conservant toutes les lignes du fichier texte
SaveData saves[MAX_SAVES];
int nb_saves = 0;

// ==========================================
// GESTION DU PROFIL ACTUEL
// ==========================================

void set_niveau_debloque(int niveau) { niveau_debloque = niveau; }
int get_niveau_debloque() { return niveau_debloque; }

void set_score_global_partie(int score) { score_global = score; }
int get_score_global_partie() { return score_global; }
void ajouter_score_partie(int score) { score_global += score; }

// ==========================================
// LECTURE ET ÉCRITURE DANS LE FICHIER TEXTE
// ==========================================

// Ouvre le fichier en mode "r" (Read) et stocke tout dans le tableau 'saves'
void lire_toutes_sauvegardes() {
    FILE* f = fopen("sauvegardes.txt", "r");
    nb_saves = 0; // Remise à zéro

    if (!f) return; // Si le fichier n'existe pas encore, on arrête

    // Lit chaque ligne tant qu'elle contient 3 éléments : "Texte Entier Entier"
    while(fscanf(f, "%19s %d %d", saves[nb_saves].pseudo, &saves[nb_saves].niveau, &saves[nb_saves].score) == 3) {
        nb_saves++;
        if (nb_saves >= MAX_SAVES) break; // Sécurité pour ne pas faire déborder le tableau
    }
    fclose(f); // Ne jamais oublier de fermer le fichier !
}

// Ouvre le fichier en mode "w" (Write) et écrase l'ancien contenu
void ecrire_toutes_sauvegardes() {
    FILE* f = fopen("sauvegardes.txt", "w");
    if (!f) return;

    // Réécrit le tableau 'saves' ligne par ligne dans le fichier
    for(int i=0; i<nb_saves; i++) {
        fprintf(f, "%s %d %d\n", saves[i].pseudo, saves[i].niveau, saves[i].score);
    }
    fclose(f);
}

// ==========================================
// ACTIONS SUR LES SAUVEGARDES
// ==========================================

// Met à jour la progression ou crée un nouveau profil
void sauvegarder_partie(const char* pseudo, int niveau, int score) {
    if (strlen(pseudo) == 0) return; // Pas de pseudo = pas de sauvegarde

    lire_toutes_sauvegardes(); // 1. Met à jour la RAM depuis le fichier

    // 2. Cherche si le joueur existe déjà
    int index = -1;
    for(int i=0; i<nb_saves; i++) {
        if (strcmp(saves[i].pseudo, pseudo) == 0) {
            index = i; break;
        }
    }

    // 3. Mise à jour ou Création
    if (index != -1) {
        // Le joueur existe : on met à jour son niveau
        saves[index].niveau = niveau;
        // On ne met à jour son score global que s'il a battu son propre record
        if (score > saves[index].score) saves[index].score = score;
    } else {
        // Nouveau joueur : on l'ajoute à la fin du tableau
        if (nb_saves < MAX_SAVES) {
            strcpy(saves[nb_saves].pseudo, pseudo);
            saves[nb_saves].niveau = niveau;
            saves[nb_saves].score = score;
            nb_saves++;
        }
    }

    ecrire_toutes_sauvegardes(); // 4. Enregistre le tableau sur le disque dur
}

// Cherche le profil dans le fichier et charge ses stats en mémoire
int charger_partie_pseudo(const char* pseudo) {
    lire_toutes_sauvegardes();

    for(int i=0; i<nb_saves; i++) {
        if (strcmp(saves[i].pseudo, pseudo) == 0) {
            set_niveau_debloque(saves[i].niveau);
            set_score_global_partie(saves[i].score);
            return 1; // Succès
        }
    }
    return 0; // Échec (Pseudo introuvable)
}

// ==========================================
// GESTION DU SCOREBOARD (Top 5)
// ==========================================

void get_top_5_scores(char pseudos[5][20], int scores[5]) {
    lire_toutes_sauvegardes();

    // 1. Initialisation d'un classement vide par défaut
    for(int i=0; i<5; i++) {
        strcpy(pseudos[i], "---");
        scores[i] = 0;
    }

    // 2. Copie temporaire du tableau pour pouvoir le trier sans casser l'original
    SaveData temp[MAX_SAVES];
    for(int i=0; i<nb_saves; i++) temp[i] = saves[i];

    // 3. Algorithme du Tri à Bulles (Décroissant)
    // Compare les scores deux à deux et les inverse si le plus petit est en haut
    for(int i=0; i<nb_saves-1; i++) {
        for(int j=0; j<nb_saves-i-1; j++) {
            if (temp[j].score < temp[j+1].score) {
                SaveData t = temp[j];
                temp[j] = temp[j+1];
                temp[j+1] = t;
            }
        }
    }

    // 4. On extrait seulement les 5 premiers du tableau trié
    for(int i=0; i<5 && i<nb_saves; i++) {
        strcpy(pseudos[i], temp[i].pseudo);
        scores[i] = temp[i].score;
    }
}

// ==========================================
// COMPATIBILITÉ (Anciennes fonctions vides)
// ==========================================
void charger_scores() {}
void sauvegarder_scores() {}
void lire_infos_sauvegardes() {}
int get_info_save_existe(int slot) { return 0; }
