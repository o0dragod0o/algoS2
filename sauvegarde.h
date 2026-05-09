#ifndef SAUVEGARDE_H
#define SAUVEGARDE_H

// Variables partagées pour l'interface de saisie
extern char pseudo_actuel[20];
extern int message_erreur_pseudo;

// Fonctions principales de sauvegarde sur fichier unique
void sauvegarder_partie(const char* pseudo, int niveau, int score);
int charger_partie_pseudo(const char* pseudo);

// Accesseurs de l'état global
void set_niveau_debloque(int niveau);
int get_niveau_debloque();
void set_score_global_partie(int score);
int get_score_global_partie();

// Tableau des meilleurs scores (scores.txt)
void charger_scores();
void sauvegarder_scores();
void ajouter_score(int points);
void ajouter_score_partie(int score);
int* get_scores();

// Anciennes fonctions vides pour éviter des erreurs de compilation dans main.c
void lire_infos_sauvegardes();
int get_info_save_existe(int slot);

#endif
