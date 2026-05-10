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
    // ==========================================
    // INITIALISATION DU JEU
    // ==========================================
    initialiser_allegro();                       // Allume le moteur, clavier, souris
    initialiser_affichage(SCREEN_W, SCREEN_H);   // Crée le double buffer
    initialiser_ressources();                    // Charge les images et les sons
    initialiser_boutons_menu(SCREEN_W, SCREEN_H);// Place les boutons de l'accueil

    // CORRECTION : Plus besoin de passer SCREEN_W et SCREEN_H ici
    initialiser_donnees_carte();                 // Prépare les villes et niveaux

    charger_scores();                            // Lit le fichier du Top 5
    lire_infos_sauvegardes();                    // Lit la progression des joueurs

    // Définition de l'état de départ du jeu
    EtatJeu etat_courant = ETAT_MENU_PRINCIPAL;

    // Lancement de la musique de fond en boucle (paramètre '1' à la fin)
    if (mus_menu != NULL) {
        play_sample(mus_menu, get_vol_musique(), 128, 1000, 1);
    }

    // ==========================================
    // LA BOUCLE DE JEU
    // ==========================================
    while (etat_courant != ETAT_QUITTER) {

        // --- PHASE A : LOGIQUE & IHM (Cerveau) ---
        // On aiguille vers la bonne fonction de calcul selon l'écran actuel
        switch (etat_courant) {
            case ETAT_MENU_PRINCIPAL: etat_courant = gerer_ihm_menu_principal(etat_courant); break;
            case ETAT_CARTE_MONDE:    etat_courant = gerer_ihm_carte_monde(etat_courant); break;
            case ETAT_OPTIONS:        etat_courant = gerer_ihm_options(etat_courant); break;
            case ETAT_REGLES:         etat_courant = gerer_ihm_generique_retour(etat_courant); break;
            case ETAT_CHARGER_PARTIE: etat_courant = gerer_ihm_charger_partie(etat_courant); break;
            case ETAT_JEU_EN_COURS:   etat_courant = mettre_a_jour_jeu(etat_courant, SCREEN_W, SCREEN_H); break;
            case ETAT_ECRAN_DEFAITE:  etat_courant = gerer_ihm_ecran_defaite(etat_courant); break;
            case ETAT_PAUSE:          etat_courant = gerer_ihm_pause(etat_courant); break;
            case ETAT_ECRAN_VICTOIRE: etat_courant = gerer_ihm_ecran_victoire(etat_courant); break;
            default: break;
        }

        // --- PHASE B : ÉVÉNEMENTS GLOBAUX ---

        // Si le joueur a bougé la jauge de musique dans les options
        if (flag_musique_changee) {
            if (mus_menu) adjust_sample(mus_menu, get_vol_musique(), 128, 1000, 1);
            flag_musique_changee = 0; // Acquittement
        }

        // Si le joueur a cliqué sur "Plein Écran"
        if (flag_resolution_changee) {
            detruire_affichage(); // Détruit l'ancien buffer

            if (is_plein_ecran()) {
                set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, SCREEN_W, SCREEN_H, 0, 0);
            } else {
                set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_W, SCREEN_H, 0, 0);
            }

            initialiser_affichage(SCREEN_W, SCREEN_H); // Recrée un buffer propre
            initialiser_boutons_menu(SCREEN_W, SCREEN_H);
            flag_resolution_changee = 0; // Acquittement
        }

        // Animation constante du fond étoilé dans le menu
        if (etat_courant == ETAT_MENU_PRINCIPAL) animer_fond_menu();


        // --- PHASE C : AFFICHAGE (Dessin) ---
        // On aiguille vers la bonne fonction de dessin
        switch (etat_courant) {
            case ETAT_MENU_PRINCIPAL: dessiner_menu_principal(); break;
            case ETAT_CARTE_MONDE:    dessiner_carte_monde(); break;
            case ETAT_OPTIONS:        dessiner_options(); break;
            case ETAT_REGLES:         dessiner_regles(); break;
            case ETAT_CHARGER_PARTIE: dessiner_charger_partie(); break;
            case ETAT_JEU_EN_COURS:   dessiner_jeu_en_cours(); break;
            case ETAT_ECRAN_DEFAITE:  dessiner_ecran_defaite(); break;
            case ETAT_PAUSE:          dessiner_pause(); break;
            case ETAT_ECRAN_VICTOIRE: dessiner_ecran_victoire(); break;
            default: break;
        }


        // --- PHASE D : SYNCHRONISATION ET TEMPS ---

        // Envoie l'image terminée à l'écran (Double Buffering)
        if (etat_courant != ETAT_QUITTER) {
            synchroniser_affichage();
        }

        // Pause de 16 millisecondes : Force le jeu à tourner à ~60 FPS
        // Évite que le jeu tourne à 2000 FPS et brûle le processeur !
        rest(16);
    }

    // ==========================================
    // FERMETURE DU JEU (Nettoyage mémoire)
    // ==========================================
    if (mus_menu) stop_sample(mus_menu);
    detruire_ressources();
    detruire_affichage();
    detruire_donnees_carte();

    return 0;
}
END_OF_MAIN()
