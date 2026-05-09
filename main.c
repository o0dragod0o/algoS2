#include <allegro.h>
#include "constantes.h"
#include "init.h"
#include "menu.h"
#include "carte.h"
#include "ihm.h"
#include "affichage.h"
#include "ressources.h"
#include "options.h"
#include "logique.h"
#include "sauvegarde.h"

int main() {
    initialiser_allegro();
    initialiser_affichage(SCREEN_W, SCREEN_H);
    initialiser_ressources();
    initialiser_boutons_menu(SCREEN_W, SCREEN_H);

    // CORRECTION ICI : le bon nom de la fonction avec ses paramètres
    initialiser_donnees_carte(SCREEN_W, SCREEN_H);

    charger_scores();
    lire_infos_sauvegardes();

    EtatJeu etat_courant = ETAT_MENU_PRINCIPAL;

    if (mus_menu != NULL) {
        play_sample(mus_menu, get_vol_musique(), 128, 1000, 1);
    }

    while (etat_courant != ETAT_QUITTER) {

        // 1. LOGIQUE & IHM
        switch (etat_courant) {
            case ETAT_MENU_PRINCIPAL: etat_courant = gerer_ihm_menu_principal(etat_courant); break;
            case ETAT_CARTE_MONDE: etat_courant = gerer_ihm_carte_monde(etat_courant); break;
            case ETAT_OPTIONS: etat_courant = gerer_ihm_options(etat_courant); break;

            case ETAT_REGLES:
                etat_courant = gerer_ihm_generique_retour(etat_courant);
                break;
            case ETAT_CHARGER_PARTIE:
                etat_courant = gerer_ihm_charger_partie(etat_courant);
                break;

            case ETAT_JEU_EN_COURS: etat_courant = mettre_a_jour_jeu(etat_courant, SCREEN_W, SCREEN_H); break;
            case ETAT_ECRAN_DEFAITE: etat_courant = gerer_ihm_ecran_defaite(etat_courant); break;

            // NOUVEAUX ETATS
            case ETAT_PAUSE: etat_courant = gerer_ihm_pause(etat_courant); break;
            case ETAT_ECRAN_VICTOIRE: etat_courant = gerer_ihm_ecran_victoire(etat_courant); break;
            default: break;
        }

        if (flag_musique_changee) {
            if (mus_menu) adjust_sample(mus_menu, get_vol_musique(), 128, 1000, 1);
            flag_musique_changee = 0;
        }

        if (flag_resolution_changee) {
            detruire_affichage();

            if (is_plein_ecran()) {
                set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, SCREEN_W, SCREEN_H, 0, 0);
            } else {
                set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_W, SCREEN_H, 0, 0);
            }

            initialiser_affichage(SCREEN_W, SCREEN_H);
            initialiser_boutons_menu(SCREEN_W, SCREEN_H);
            flag_resolution_changee = 0;
        }

        if (etat_courant == ETAT_MENU_PRINCIPAL) animer_fond_menu();

        // 3. AFFICHAGE
        switch (etat_courant) {
            case ETAT_MENU_PRINCIPAL: dessiner_menu_principal(); break;
            case ETAT_CARTE_MONDE: dessiner_carte_monde(); break;
            case ETAT_OPTIONS: dessiner_options(); break;
            case ETAT_REGLES: dessiner_regles(); break;
            case ETAT_CHARGER_PARTIE: dessiner_charger_partie(); break;
            case ETAT_JEU_EN_COURS: dessiner_jeu_en_cours(); break;
            case ETAT_ECRAN_DEFAITE: dessiner_ecran_defaite(); break;

            // NOUVEAUX ETATS
            case ETAT_PAUSE: dessiner_pause(); break;
            case ETAT_ECRAN_VICTOIRE: dessiner_ecran_victoire(); break;
            default: break;
        }

        if (etat_courant != ETAT_QUITTER) {
            synchroniser_affichage();
        }

        // OPTIMISATION DU LAG (~60 FPS)
        rest(16);
    }

    if (mus_menu) stop_sample(mus_menu);
    detruire_ressources();
    detruire_affichage();
    detruire_donnees_carte();

    return 0;
}
END_OF_MAIN()
