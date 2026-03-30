#include "carte.h"
#include <stdlib.h>
#include <allegro.h>

// NOUVEAU : Pointeur pour l'allocation dynamique (malloc)
static Ville* liste_villes = NULL;
static GestionnaireCarte etat_carte;

void initialiser_donnees_carte(int ecran_w, int ecran_h) {
    etat_carte.mode_actuel = MODE_CARTE_PARCOURIR;
    etat_carte.id_ville_survolee = -1;
    etat_carte.id_ville_selectionnee = -1;
    etat_carte.panel_x = 0;
    etat_carte.panel_y = 0;

    // NOUVEAU : Allocation dynamique pour 4 villes
    liste_villes = (Ville*)malloc(4 * sizeof(Ville));
    if (liste_villes == NULL) {
        allegro_message("Erreur d'allocation dynamique pour les villes !");
        exit(EXIT_FAILURE);
    }

    // Niveau 1
    liste_villes[0].id = 0;
    liste_villes[0].x = 150;
    liste_villes[0].y = 200;
    liste_villes[0].rayon_clic = 25;
    liste_villes[0].nom = "Paris - France";
    liste_villes[0].desc_ligne1 = "Niveau 1 : Debut de l'aventure.";
    liste_villes[0].desc_ligne2 = "Difficulte : Facile";
    liste_villes[0].numero_niveau = 1;

    // Niveau 2
    liste_villes[1].id = 1;
    liste_villes[1].x = 300;
    liste_villes[1].y = 150;
    liste_villes[1].rayon_clic = 25;
    liste_villes[1].nom = "Londres - UK";
    liste_villes[1].desc_ligne1 = "Niveau 2 : Pluie de bulles.";
    liste_villes[1].desc_ligne2 = "Difficulte : Moyenne";
    liste_villes[1].numero_niveau = 2;

    // Niveau 3
    liste_villes[2].id = 2;
    liste_villes[2].x = 650;
    liste_villes[2].y = 250;
    liste_villes[2].rayon_clic = 25;
    liste_villes[2].nom = "Tokyo - Japon";
    liste_villes[2].desc_ligne1 = "Niveau 3 : Le defi ultime.";
    liste_villes[2].desc_ligne2 = "Difficulte : Difficile";
    liste_villes[2].numero_niveau = 3;

    // Niveau 4
    liste_villes[3].id = 3;
    liste_villes[3].x = 550;
    liste_villes[3].y = 450;
    liste_villes[3].rayon_clic = 25;
    liste_villes[3].nom = "New York (BOSS)";
    liste_villes[3].desc_ligne1 = "Niveau 4 FINAL.";
    liste_villes[3].desc_ligne2 = "Attention au Boss des bulles !";
    liste_villes[3].numero_niveau = 4;
}

// NOUVEAU : Nettoyage de la mémoire dynamique
void detruire_donnees_carte() {
    if (liste_villes != NULL) {
        free(liste_villes);
        liste_villes = NULL;
    }
}

Ville* get_villes() { return liste_villes; }
int get_nb_villes() { return 4; }
GestionnaireCarte* get_etat_carte() { return &etat_carte; }
