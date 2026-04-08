#ifndef MENU_H
#define MENU_H

#include "constantes.h"

typedef struct {
    int x, y;
    int largeur, hauteur;
    char* texte;
    EtatJeu etat_cible;
} Bouton;

void initialiser_boutons_menu(int ecran_w, int ecran_h);
Bouton* get_boutons_menu_principal();
int get_nb_boutons_menu_principal();
int est_clic_sur_bouton(Bouton b, int mx, int my);

#endif
