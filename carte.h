#ifndef CARTE_H
#define CARTE_H

#include "constantes.h"

typedef struct Ville {
    int id;
    int x, y;
    int rayon_clic;
    char* nom;
    // NOUVEAU : On sépare la description en deux lignes pour éviter les débordements
    char* desc_ligne1;
    char* desc_ligne2;
    int numero_niveau;
} Ville;

typedef struct {
    ModeCarte mode_actuel;
    int id_ville_survolee;
    int id_ville_selectionnee;
    // NOUVEAU : On sauvegarde la position du panneau pour que l'IHM trouve les boutons
    int panel_x;
    int panel_y;
} GestionnaireCarte;

void initialiser_donnees_carte(int ecran_w, int ecran_h);
void detruire_donnees_carte(); // NOUVEAU : Libère la mémoire (free)

Ville* get_villes();
int get_nb_villes();
GestionnaireCarte* get_etat_carte();

#endif
