#include "menu.h"
#include <string.h>

static Bouton boutons[5];

void initialiser_boutons_menu(int ecran_w, int ecran_h) {
    int cx = ecran_w / 2;
    int start_y = 200;
    int espace = 60;
    int bw = 240; int bh = 40;
    int bx = cx - (bw / 2);

    // NOUVEAUX NOMS POUR LES BOUTONS
    boutons[0] = (Bouton){bx, start_y, bw, bh, "NOUVEAU JEU", ETAT_CARTE_MONDE};
    boutons[1] = (Bouton){bx, start_y + espace, bw, bh, "REPRENDRE PARTIE", ETAT_CHARGER_PARTIE};
    boutons[2] = (Bouton){bx, start_y + espace*2, bw, bh, "REGLES DU JEU", ETAT_REGLES};
    boutons[3] = (Bouton){bx, start_y + espace*3, bw, bh, "OPTIONS", ETAT_OPTIONS};
    boutons[4] = (Bouton){bx, start_y + espace*4, bw, bh, "QUITTER", ETAT_QUITTER};
}

Bouton* get_boutons_menu_principal() { return boutons; }
int get_nb_boutons_menu_principal() { return 5; }
int est_clic_sur_bouton(Bouton b, int mx, int my) {
    return (mx >= b.x && mx <= b.x + b.largeur && my >= b.y && my <= b.y + b.hauteur);
}
