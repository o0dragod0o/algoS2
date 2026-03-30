#include "menu.h"

static Bouton boutons_menu[5];

void initialiser_boutons_menu(int ecran_w, int ecran_h) {
    int largeur_bouton = 300;
    int hauteur_bouton = 40;

    // NOUVEAU : Moins d'espace et on descend le bloc pour ne pas toucher le Logo
    int espacement = 10;
    int pos_x_centree = (ecran_w / 2) - (largeur_bouton / 2);
    int pos_y_depart = 280;

    boutons_menu[0].x = pos_x_centree;
    boutons_menu[0].y = pos_y_depart;
    boutons_menu[0].largeur = largeur_bouton;
    boutons_menu[0].hauteur = hauteur_bouton;
    boutons_menu[0].texte = "Nouvelle Partie (Carte)";
    boutons_menu[0].etat_cible = ETAT_CARTE_MONDE;

    boutons_menu[1].x = pos_x_centree;
    boutons_menu[1].y = pos_y_depart + (hauteur_bouton + espacement);
    boutons_menu[1].largeur = largeur_bouton;
    boutons_menu[1].hauteur = hauteur_bouton;
    boutons_menu[1].texte = "Reprendre une partie";
    boutons_menu[1].etat_cible = ETAT_SAUVEGARDE;

    boutons_menu[2].x = pos_x_centree;
    boutons_menu[2].y = pos_y_depart + 2 * (hauteur_bouton + espacement);
    boutons_menu[2].largeur = largeur_bouton;
    boutons_menu[2].hauteur = hauteur_bouton;
    boutons_menu[2].texte = "Regles du jeu";
    boutons_menu[2].etat_cible = ETAT_REGLES;

    boutons_menu[3].x = pos_x_centree;
    boutons_menu[3].y = pos_y_depart + 3 * (hauteur_bouton + espacement);
    boutons_menu[3].largeur = largeur_bouton;
    boutons_menu[3].hauteur = hauteur_bouton;
    boutons_menu[3].texte = "Options";
    boutons_menu[3].etat_cible = ETAT_OPTIONS;

    boutons_menu[4].x = pos_x_centree;
    boutons_menu[4].y = pos_y_depart + 4 * (hauteur_bouton + espacement);
    boutons_menu[4].largeur = largeur_bouton;
    boutons_menu[4].hauteur = hauteur_bouton;
    boutons_menu[4].texte = "Quitter";
    boutons_menu[4].etat_cible = ETAT_QUITTER;
}

int est_clic_sur_bouton(Bouton b, int mx, int my) {
    if (mx >= b.x && mx <= b.x + b.largeur && my >= b.y && my <= b.y + b.hauteur) return 1;
    return 0;
}

Bouton* get_boutons_menu_principal() { return boutons_menu; }
int get_nb_boutons_menu_principal() { return 5; }
