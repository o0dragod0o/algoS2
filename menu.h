#ifndef MENU_H
#define MENU_H

#include "constantes.h"

typedef struct {
    int x;
    int y;
    int largeur;
    int hauteur;
    char* texte;
    EtatJeu etat_cible;
} Bouton;

void initialiser_boutons_menu(int ecran_w, int ecran_h);
int est_clic_sur_bouton(Bouton b, int mx, int my);

Bouton* get_boutons_menu_principal();
int get_nb_boutons_menu_principal();

#endif
