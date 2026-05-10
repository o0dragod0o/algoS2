#ifndef IHM_H
#define IHM_H

#include "constantes.h"
#include "carte.h"

// 0 = 1 Joueur, 1 = 2 Joueurs, 2 = Reprendre une partie
extern int mode_action_saisie;

// ==========================================
// SOURIS
// ==========================================
int souris_sur_ville(Ville v, int mx, int my); // Vérifie le survol d'un cercle (ville)
int souris_dans_rect(int x, int y, int w, int h, int mx, int my); // Vérifie le survol d'un rectangle

// ==========================================
// GESTIONNAIRES D'ÉCRANS
// ==========================================
EtatJeu gerer_ihm_menu_principal(EtatJeu etat_actuel);
EtatJeu gerer_ihm_charger_partie(EtatJeu etat_actuel); // Saisie clavier du pseudo
EtatJeu gerer_ihm_carte_monde(EtatJeu etat_actuel);    // Clics sur les niveaux
EtatJeu gerer_ihm_generique_retour(EtatJeu etat_actuel); // Retour simple (Menu Règles)
EtatJeu gerer_ihm_options(EtatJeu etat_actuel);        // Gestion des jauges de son

// ==========================================
// GESTIONNAIRES D'OVERLAYS
// ==========================================
EtatJeu gerer_ihm_ecran_defaite(EtatJeu etat_actuel);
EtatJeu gerer_ihm_pause(EtatJeu etat_courant);
EtatJeu gerer_ihm_ecran_victoire(EtatJeu etat_courant);

#endif
