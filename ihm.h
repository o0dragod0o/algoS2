#ifndef IHM_H
#define IHM_H

#include "constantes.h"

EtatJeu gerer_ihm_menu_principal(EtatJeu etat_actuel);
EtatJeu gerer_ihm_carte_monde(EtatJeu etat_actuel);
EtatJeu gerer_ihm_generique_retour(EtatJeu etat_actuel);
EtatJeu gerer_ihm_options(EtatJeu etat_actuel);
EtatJeu gerer_ihm_ecran_defaite(EtatJeu etat_actuel);
EtatJeu gerer_ihm_charger_partie(EtatJeu etat_actuel); // NOUVEAU

int souris_dans_rect(int x, int y, int w, int h, int mx, int my);

#endif
