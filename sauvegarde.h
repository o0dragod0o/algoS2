#ifndef SAUVEGARDE_H
#define SAUVEGARDE_H

// --- Scores Globaux (Affichés sur le Menu Principal) ---
void charger_scores();
void sauvegarder_scores();
void ajouter_score(int nouveau_score);
int* get_scores();

// --- Progression Multi-Sauvegardes (Slots 1 à 4) ---
void lire_infos_sauvegardes();
void creer_nouvelle_partie();
void charger_partie(int slot);
void sauvegarder_progression();

// Accesseurs pour le menu Charger Partie
int get_info_save_existe(int slot);
int get_info_save_niveau(int slot);
int get_info_save_score(int slot);

// Accesseurs en cours de jeu
int get_niveau_debloque();
void set_niveau_debloque(int niv);
int get_score_global_partie();
void ajouter_score_partie(int points);

#endif
