#ifndef SAUVEGARDE_H
#define SAUVEGARDE_H

// ==========================================
// VARIABLES GLOBALES
// ==========================================

// Le pseudo tapé au clavier par le joueur
extern char pseudo_actuel[20];

// 1 si le joueur essaie de charger un pseudo qui n'existe pas
extern int message_erreur_pseudo;

// ==========================================
// FONCTIONS DE LECTURE / ÉCRITURE
// ==========================================

// Ajoute ou met à jour un profil dans le fichier "sauvegardes.txt"
void sauvegarder_partie(const char* pseudo, int niveau, int score);

// Cherche un profil dans le fichier. Retourne 1 si trouvé, 0 sinon.
int charger_partie_pseudo(const char* pseudo);

// Trie le fichier texte pour extraire les 5 meilleurs joueurs
void get_top_5_scores(char pseudos[5][20], int scores[5]);

// ==========================================
// ACCESSEURS DE PROGRESSION EN MÉMOIRE
// ==========================================
void set_niveau_debloque(int niveau);
int get_niveau_debloque();

void set_score_global_partie(int score);
int get_score_global_partie();
void ajouter_score_partie(int score);

// --- Fonctions de compatibilité (vides, pour éviter les erreurs) ---
void charger_scores();
void sauvegarder_scores();
void lire_infos_sauvegardes();
int get_info_save_existe(int slot);

#endif
