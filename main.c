#include <allegro.h>
#include "constantes.h"
#include "init.h"
#include "menu.h"
#include "carte.h"
#include "ihm.h"
#include "affichage.h"
#include "ressources.h"
#include "options.h"

int main() {
    initialiser_allegro();
    initialiser_affichage(SCREEN_W, SCREEN_H);
    initialiser_ressources();
    initialiser_boutons_menu(SCREEN_W, SCREEN_H);
    initialiser_donnees_carte(SCREEN_W, SCREEN_H);

    EtatJeu etat_courant = ETAT_MENU_PRINCIPAL;

    if (mus_menu != NULL) {
        play_sample(mus_menu, get_vol_musique(), 128, 1000, 1);
    }

    while (etat_courant != ETAT_QUITTER) {

        // 1. LOGIQUE & IHM
        switch (etat_courant) {
            case ETAT_MENU_PRINCIPAL:
                etat_courant = gerer_ihm_menu_principal(etat_courant);
                break;
            case ETAT_CARTE_MONDE:
                etat_courant = gerer_ihm_carte_monde(etat_courant);
                break;
            case ETAT_OPTIONS:
                etat_courant = gerer_ihm_options(etat_courant);
                break;
            case ETAT_REGLES:
            case ETAT_SAUVEGARDE:
                etat_courant = gerer_ihm_generique_retour(etat_courant);
                break;
            case ETAT_JEU_EN_COURS:
                allegro_message("Lancement du niveau ! (Bientot dispo)\nRetour a la carte.");
                etat_courant = ETAT_CARTE_MONDE;
                break;
            default:
                break;
        }

        // --- APPLICATION DES OPTIONS EN TEMPS REEL ---
        if (flag_musique_changee) {
            if (mus_menu) {
                // NOUVEAU : adjust_sample modifie le son SANS le faire recommencer du début !
                adjust_sample(mus_menu, get_vol_musique(), 128, 1000, 1);
            }
            flag_musique_changee = 0;
        }

        if (flag_resolution_changee) {
            // FIX CRASH : On doit détruire le buffer AVANT de reset la carte graphique !
            detruire_affichage();

            set_gfx_mode(GFX_AUTODETECT_WINDOWED, get_res_w(), get_res_h(), 0, 0);

            initialiser_affichage(get_res_w(), get_res_h()); // On recrée le buffer à la nouvelle taille
            initialiser_boutons_menu(get_res_w(), get_res_h());
            flag_resolution_changee = 0;
        }

        // 2. ANIMATIONS
        if (etat_courant == ETAT_MENU_PRINCIPAL) animer_fond_menu();

        // 3. AFFICHAGE
        switch (etat_courant) {
            case ETAT_MENU_PRINCIPAL:
                dessiner_menu_principal();
                break;
            case ETAT_CARTE_MONDE:
                dessiner_carte_monde();
                break;
            case ETAT_OPTIONS:
                dessiner_options();
                break;
            case ETAT_REGLES:
                dessiner_regles();
                break;
            case ETAT_SAUVEGARDE:
                dessiner_sauvegarde();
                break;
            default:
                break;
        }

        // 4. SYNCHRONISATION ECRAN
        if (etat_courant != ETAT_QUITTER) {
            synchroniser_affichage();
        }

        rest(10);
    }

    // NETTOYAGE
    if (mus_menu) stop_sample(mus_menu);
    detruire_ressources();
    detruire_affichage();
    detruire_donnees_carte();

    return 0;
}
END_OF_MAIN()
