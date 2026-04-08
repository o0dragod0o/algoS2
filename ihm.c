#include <allegro.h>
#include <math.h>
#include "ihm.h"
#include "menu.h"
#include "carte.h"
#include "ressources.h"
#include "options.h"
#include "logique.h"
#include "sauvegarde.h"

int souris_sur_ville(Ville v, int mx, int my) {
    long dx = mx - v.x; long dy = my - v.y;
    return (dx*dx + dy*dy) < (v.rayon_clic * v.rayon_clic);
}
int souris_dans_rect(int x, int y, int w, int h, int mx, int my) { return (mx >= x && mx <= x+w && my >= y && my <= y+h); }

EtatJeu gerer_ihm_menu_principal(EtatJeu etat_actuel) {
    if (mouse_b & 1) {
        Bouton* boutons = get_boutons_menu_principal(); int nb = get_nb_boutons_menu_principal();
        for (int i = 0; i < nb; i++) {
            if (est_clic_sur_bouton(boutons[i], mouse_x, mouse_y)) {
                if (son_beep) play_sample(son_beep, get_vol_sfx(), 128, 1000, 0);
                while(mouse_b & 1) rest(10);

                // NOUVEAU : Création de la sauvegarde en cliquant sur NOUVEAU JEU
                if (boutons[i].etat_cible == ETAT_CARTE_MONDE) {
                    creer_nouvelle_partie();
                }

                return boutons[i].etat_cible;
            }
        }
    }
    if (key[KEY_ESC]) return ETAT_QUITTER;
    return etat_actuel;
}

// NOUVEAU MENU : Sélection de la sauvegarde
EtatJeu gerer_ihm_charger_partie(EtatJeu etat_actuel) {
    if (key[KEY_R] || key[KEY_ESC]) {
        if (son_beep != NULL) play_sample(son_beep, get_vol_sfx(), 128, 1000, 0);
        rest(200); return ETAT_MENU_PRINCIPAL;
    }

    if (mouse_b & 1) {
        int cx = SCREEN_W / 2;
        for(int i=0; i<4; i++) {
            int by = 130 + i * 90;
            if(souris_dans_rect(cx - 180, by, 360, 70, mouse_x, mouse_y)) {
                if(get_info_save_existe(i+1)) {
                    if (son_beep) play_sample(son_beep, get_vol_sfx(), 128, 1100, 0);
                    while(mouse_b & 1) rest(10);
                    charger_partie(i+1); // Charge et sélectionne le slot
                    return ETAT_CARTE_MONDE;
                }
            }
        }
    }
    return etat_actuel;
}

EtatJeu gerer_ihm_carte_monde(EtatJeu etat_actuel) {
    GestionnaireCarte* etat = get_etat_carte(); Ville* villes = get_villes(); int nb = get_nb_villes();

    if (key[KEY_R] || key[KEY_ESC]) {
        while(key[KEY_R] || key[KEY_ESC]) rest(10);
        if (etat->mode_actuel == MODE_CARTE_INFO) { etat->mode_actuel = MODE_CARTE_PARCOURIR; etat->id_ville_selectionnee = -1; return etat_actuel; }
        else return ETAT_MENU_PRINCIPAL;
    }
    if (mouse_b & 2) {
        while(mouse_b & 2) rest(10);
        if (etat->mode_actuel == MODE_CARTE_INFO) { etat->mode_actuel = MODE_CARTE_PARCOURIR; etat->id_ville_selectionnee = -1; return etat_actuel; }
        else return ETAT_MENU_PRINCIPAL;
    }

    if (etat->mode_actuel == MODE_CARTE_PARCOURIR) {
        etat->id_ville_survolee = -1;
        for (int i = 0; i < nb; i++) {
            if (souris_sur_ville(villes[i], mouse_x, mouse_y)) {
                if (villes[i].numero_niveau <= get_niveau_debloque()) {
                    etat->id_ville_survolee = villes[i].id;
                    break;
                }
            }
        }

        if ((mouse_b & 1) && etat->id_ville_survolee != -1) {
            if (son_beep) play_sample(son_beep, get_vol_sfx(), 128, 1000, 0);
            etat->id_ville_selectionnee = etat->id_ville_survolee; etat->mode_actuel = MODE_CARTE_INFO;
            while(mouse_b & 1) rest(10);
        }
    }
    else if (etat->mode_actuel == MODE_CARTE_INFO) {
        int px = etat->panel_x; int py = etat->panel_y;
        if (mouse_b & 1) {
            if (souris_dans_rect(px + 20, py + 110, 90, 30, mouse_x, mouse_y)) {
                if (son_beep) play_sample(son_beep, get_vol_sfx(), 128, 1100, 0);
                while(mouse_b & 1) rest(10);
                Ville v = villes[etat->id_ville_selectionnee];
                initialiser_niveau(v.numero_niveau, SCREEN_W, SCREEN_H);
                return ETAT_JEU_EN_COURS;
            }
            if (souris_dans_rect(px + 130, py + 110, 90, 30, mouse_x, mouse_y)) {
                etat->mode_actuel = MODE_CARTE_PARCOURIR; etat->id_ville_selectionnee = -1;
                while(mouse_b & 1) rest(10);
            }
        }
    }
    return etat_actuel;
}

EtatJeu gerer_ihm_generique_retour(EtatJeu etat_actuel) {
    if (key[KEY_R] || key[KEY_ESC]) {
        if (son_beep != NULL) play_sample(son_beep, get_vol_sfx(), 128, 1000, 0);
        rest(200); return ETAT_MENU_PRINCIPAL;
    }
    return etat_actuel;
}

EtatJeu gerer_ihm_options(EtatJeu etat_actuel) {
    if (key[KEY_R] || key[KEY_ESC]) {
        if (son_beep) play_sample(son_beep, get_vol_sfx(), 128, 1000, 0);
        rest(200); return ETAT_MENU_PRINCIPAL;
    }

    if (mouse_b & 1) {
        int cx = SCREEN_W / 2; int bar_w = 200; int start_x = cx - (bar_w / 2);

        if (souris_dans_rect(start_x - 10, 170, bar_w + 20, 40, mouse_x, mouse_y)) {
            int new_vol = ((mouse_x - start_x) * 255) / bar_w; set_vol_musique(new_vol);
        }
        if (souris_dans_rect(start_x - 10, 270, bar_w + 20, 40, mouse_x, mouse_y)) {
            int new_vol = ((mouse_x - start_x) * 255) / bar_w; set_vol_sfx(new_vol);
        }
        if (souris_dans_rect(cx - 75, 400, 150, 30, mouse_x, mouse_y)) {
            toggle_plein_ecran();
            if (son_beep) play_sample(son_beep, get_vol_sfx(), 128, 1000, 0);
            while(mouse_b & 1) rest(10);
        }
    }
    return etat_actuel;
}

EtatJeu gerer_ihm_ecran_defaite(EtatJeu etat_actuel) {
    if (mouse_b & 1) {
        int cx = SCREEN_W / 2; int cy = SCREEN_H / 2;
        if (souris_dans_rect(cx - 100, cy - 10, 200, 30, mouse_x, mouse_y)) {
            if (son_beep) play_sample(son_beep, get_vol_sfx(), 128, 1000, 0);
            while(mouse_b & 1) rest(10);
            initialiser_niveau(get_niveau_en_cours(), SCREEN_W, SCREEN_H);
            return ETAT_JEU_EN_COURS;
        }
        if (souris_dans_rect(cx - 100, cy + 40, 200, 30, mouse_x, mouse_y)) {
            if (son_beep) play_sample(son_beep, get_vol_sfx(), 128, 1000, 0);
            while(mouse_b & 1) rest(10);
            return ETAT_CARTE_MONDE;
        }
    }
    return etat_actuel;
}
