#include <allegro.h>
#include <math.h>
#include "ihm.h"
#include "menu.h"
#include "carte.h"
#include "ressources.h"
#include "options.h" // NOUVEAU

int souris_sur_ville(Ville v, int mx, int my) {
    long dx = mx - v.x;
    long dy = my - v.y;
    return (dx*dx + dy*dy) < (v.rayon_clic * v.rayon_clic);
}

int souris_dans_rect(int x, int y, int w, int h, int mx, int my) {
    return (mx >= x && mx <= x+w && my >= y && my <= y+h);
}

EtatJeu gerer_ihm_menu_principal(EtatJeu etat_actuel) {
    if (mouse_b & 1) {
        Bouton* boutons = get_boutons_menu_principal();
        int nb = get_nb_boutons_menu_principal();
        for (int i = 0; i < nb; i++) {
            if (est_clic_sur_bouton(boutons[i], mouse_x, mouse_y)) {
                // Utilisation de get_vol_sfx()
                if (son_beep) play_sample(son_beep, get_vol_sfx(), 128, 1000, 0);
                while(mouse_b & 1) rest(10);
                return boutons[i].etat_cible;
            }
        }
    }
    if (key[KEY_ESC]) return ETAT_QUITTER;
    return etat_actuel;
}

EtatJeu gerer_ihm_carte_monde(EtatJeu etat_actuel) {
    GestionnaireCarte* etat = get_etat_carte();
    Ville* villes = get_villes();
    int nb = get_nb_villes();

    if (key[KEY_R] || key[KEY_ESC]) {
        while(key[KEY_R] || key[KEY_ESC]) rest(10);
        if (etat->mode_actuel == MODE_CARTE_INFO) {
            etat->mode_actuel = MODE_CARTE_PARCOURIR;
            etat->id_ville_selectionnee = -1;
            return etat_actuel;
        } else return ETAT_MENU_PRINCIPAL;
    }

    if (mouse_b & 2) {
        while(mouse_b & 2) rest(10);
        if (etat->mode_actuel == MODE_CARTE_INFO) {
            etat->mode_actuel = MODE_CARTE_PARCOURIR;
            etat->id_ville_selectionnee = -1;
            return etat_actuel;
        } else return ETAT_MENU_PRINCIPAL;
    }

    if (etat->mode_actuel == MODE_CARTE_PARCOURIR) {
        etat->id_ville_survolee = -1;
        for (int i = 0; i < nb; i++) {
            if (souris_sur_ville(villes[i], mouse_x, mouse_y)) {
                etat->id_ville_survolee = villes[i].id;
                break;
            }
        }

        if ((mouse_b & 1) && etat->id_ville_survolee != -1) {
            if (son_beep) play_sample(son_beep, get_vol_sfx(), 128, 1000, 0);
            etat->id_ville_selectionnee = etat->id_ville_survolee;
            etat->mode_actuel = MODE_CARTE_INFO;
            while(mouse_b & 1) rest(10);
        }
    }
    else if (etat->mode_actuel == MODE_CARTE_INFO) {
        int px = etat->panel_x; int py = etat->panel_y;
        if (mouse_b & 1) {
            if (souris_dans_rect(px + 20, py + 110, 90, 30, mouse_x, mouse_y)) {
                if (son_beep) play_sample(son_beep, get_vol_sfx(), 128, 1100, 0);
                while(mouse_b & 1) rest(10);
                return ETAT_JEU_EN_COURS;
            }
            if (souris_dans_rect(px + 130, py + 110, 90, 30, mouse_x, mouse_y)) {
                etat->mode_actuel = MODE_CARTE_PARCOURIR;
                etat->id_ville_selectionnee = -1;
                while(mouse_b & 1) rest(10);
            }
        }
    }
    return etat_actuel;
}

EtatJeu gerer_ihm_generique_retour(EtatJeu etat_actuel) {
    if (key[KEY_R] || key[KEY_ESC]) {
        if (son_beep != NULL) play_sample(son_beep, get_vol_sfx(), 128, 1000, 0);
        rest(200);
        return ETAT_MENU_PRINCIPAL;
    }
    return etat_actuel;
}

// NOUVEAU : Logique des clics dans le menu Options
EtatJeu gerer_ihm_options(EtatJeu etat_actuel) {
    if (key[KEY_R] || key[KEY_ESC]) {
        if (son_beep) play_sample(son_beep, get_vol_sfx(), 128, 1000, 0);
        rest(200);
        return ETAT_MENU_PRINCIPAL;
    }

    if (mouse_b & 1) {
        int cx = SCREEN_W / 2;
        int bar_w = 200;
        int start_x = cx - (bar_w / 2);

        // Détection de survol (Drag) sur le Slider Musique
        if (souris_dans_rect(start_x - 10, 170, bar_w + 20, 40, mouse_x, mouse_y)) {
            // Produit en croix : on déduit la valeur (0-255) selon la position X de la souris
            int new_vol = ((mouse_x - start_x) * 255) / bar_w;
            set_vol_musique(new_vol);
            // PAS DE rest(10) ICI ! Ça permet un glissement parfaitement fluide !
        }

        // Détection de survol (Drag) sur le Slider Effets
        if (souris_dans_rect(start_x - 10, 270, bar_w + 20, 40, mouse_x, mouse_y)) {
            int new_vol = ((mouse_x - start_x) * 255) / bar_w;
            set_vol_sfx(new_vol);
        }

        // Clic Résolution
        if (souris_dans_rect(cx - 75, 400, 150, 30, mouse_x, mouse_y)) {
            toggle_resolution();
            if (son_beep) play_sample(son_beep, get_vol_sfx(), 128, 1000, 0);
            while(mouse_b & 1) rest(10); // Anti-rebond indispensable ici
        }
    }
    return etat_actuel;
}
