#include "menu.h"
#include "constantes.h"
#include <string.h>

// ==========================================
// VARIABLES GLOBALES
// ==========================================

// Tableau mémoire contenant nos 6 boutons du menu d'accueil
Bouton boutons[6];

// ==========================================
// INITIALISATION
// ==========================================

// Prépare et aligne les boutons parfaitement au centre de l'écran
void initialiser_boutons_menu(int ecran_w, int ecran_h) {
    int cx = ecran_w / 2;              // Milieu de l'écran (Axe X)
    int start_y = BOUTON_MENU_START_Y; // Hauteur de départ (Axe Y)
    int espace = BOUTON_MENU_ESPACE;   // Écart entre chaque bouton
    int bw = BOUTON_MENU_LARGEUR;      // Largeur du bouton
    int bh = BOUTON_MENU_HAUTEUR;      // Hauteur du bouton
    int bx = cx - (bw / 2);            // Calcul pour centrer le point X

    // Utilisation de la syntaxe C99 pour remplir la structure (Bouton){...} en une ligne
    //                    X   Y                     W   H   TEXTE                 CIBLE
    boutons[0] = (Bouton){bx, start_y,              bw, bh, "1 JOUEUR",         ETAT_CARTE_MONDE};
    boutons[1] = (Bouton){bx, start_y + espace,     bw, bh, "2 JOUEURS",        ETAT_CARTE_MONDE};
    boutons[2] = (Bouton){bx, start_y + espace * 2, bw, bh, "REPRENDRE PARTIE", ETAT_CHARGER_PARTIE};
    boutons[3] = (Bouton){bx, start_y + espace * 3, bw, bh, "REGLES DU JEU",    ETAT_REGLES};
    boutons[4] = (Bouton){bx, start_y + espace * 4, bw, bh, "OPTIONS",          ETAT_OPTIONS};
    boutons[5] = (Bouton){bx, start_y + espace * 5, bw, bh, "QUITTER",          ETAT_QUITTER};
}

// ==========================================
// MÉCANIQUE ET COLLISION
// ==========================================

// Renvoie 1 (Vrai) si la souris est à l'intérieur du rectangle du bouton
int est_clic_sur_bouton(Bouton b, int mx, int my) {
    return (mx >= b.x && mx <= b.x + b.largeur && my >= b.y && my <= b.y + b.hauteur);
}

// ==========================================
// ACCESSEURS
// ==========================================

// Permet au fichier ihm.c de lire les boutons pour les cliquer
// Permet au fichier affichage.c de lire les boutons pour les dessiner
Bouton* get_boutons_menu_principal() {
    return boutons;
}

// Indique aux autres fichiers combien de boutons lire
int get_nb_boutons_menu_principal() {
    return 6;
}
