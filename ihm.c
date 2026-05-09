#include <allegro.h>
#include <math.h>
#include <string.h>
#include "ihm.h"
#include "menu.h"
#include "carte.h"
#include "ressources.h"
#include "options.h"
#include "logique.h"
#include "sauvegarde.h"

int mode_action_saisie = 0; // 0=1 Joueur, 1=2 Joueurs, 2=Reprendre

int souris_sur_ville(Ville v, int mx, int my) {
    long dx = mx - v.x; long dy = my - v.y;
    return (dx*dx + dy*dy) < (v.rayon_clic * v.rayon_clic);
}
int souris_dans_rect(int x, int y, int w, int h, int mx, int my) {
    return (mx >= x && mx <= x+w && my >= y && my <= y+h);
}

EtatJeu gerer_ihm_menu_principal(EtatJeu etat_actuel) {
    static int clic_precedent = 0;
    int clic_actuel = mouse_b & 1;

    if (clic_actuel && !clic_precedent) {
        Bouton* boutons = get_boutons_menu_principal();
        int nb = get_nb_boutons_menu_principal();

        for (int i = 0; i < nb; i++) {
            if (est_clic_sur_bouton(boutons[i], mouse_x, mouse_y)) {
                if (son_beep) play_sample(son_beep, get_vol_sfx(), 128, 1000, 0);

                // Si on clique sur 1 JOUEUR, 2 JOUEURS ou REPRENDRE
                if (i == 0 || i == 1 || i == 2) {
                    mode_action_saisie = (i == 0) ? 0 : (i == 1) ? 1 : 2;
                    clear_keybuf(); // Vide le cache clavier Allegro
                    pseudo_actuel[0] = '\0';
                    message_erreur_pseudo = 0;
                    return ETAT_CHARGER_PARTIE; // Écran de saisie
                } else {
                    return boutons[i].etat_cible;
                }
            }
        }
    }
    clic_precedent = clic_actuel;

    if (key[KEY_ESC]) return ETAT_QUITTER;
    return etat_actuel;
}

// CETTE FONCTION GERE MAINTENANT LA SAISIE DU PSEUDO AU CLAVIER
EtatJeu gerer_ihm_charger_partie(EtatJeu etat_actuel) {
    if (key[KEY_ESC]) {
        if (son_beep != NULL) play_sample(son_beep, get_vol_sfx(), 128, 1000, 0);
        return ETAT_MENU_PRINCIPAL;
    }

    if (keypressed()) {
        int val = readkey();
        int scancode = val >> 8;
        char ascii = val & 0xff;

        // Si on appuie sur ENTRER
        if (scancode == KEY_ENTER) {
            if (strlen(pseudo_actuel) > 0) {
                if (son_beep) play_sample(son_beep, get_vol_sfx(), 128, 1100, 0);

                if (mode_action_saisie == 2) {
                    // CHARGER UNE PARTIE
                    if (charger_partie_pseudo(pseudo_actuel)) {
                        set_nombre_joueurs_mode(1);
                        return ETAT_CARTE_MONDE;
                    } else {
                        message_erreur_pseudo = 1; // Pseudo non trouvé
                    }
                } else {
                    // NOUVELLE PARTIE (1 ou 2 Joueurs)
                    set_niveau_debloque(1);
                    set_score_global_partie(0);
                    set_nombre_joueurs_mode((mode_action_saisie == 0) ? 1 : 2);
                    sauvegarder_partie(pseudo_actuel, 1, 0); // Crée la ligne dans le fichier
                    return ETAT_CARTE_MONDE;
                }
            }
        }
        // Si on appuie sur RETOUR ARRIERE (Effacer)
        else if (scancode == KEY_BACKSPACE) {
            int len = strlen(pseudo_actuel);
            if (len > 0) pseudo_actuel[len-1] = '\0';
            message_erreur_pseudo = 0;
        }
        // Lettres et Chiffres classiques
        else if (ascii >= 32 && ascii <= 126) {
            int len = strlen(pseudo_actuel);
            if (len < 15) { // Limite de 15 caractères
                pseudo_actuel[len] = ascii;
                pseudo_actuel[len+1] = '\0';
            }
            message_erreur_pseudo = 0;
        }
    }

    return etat_actuel;
}

EtatJeu gerer_ihm_carte_monde(EtatJeu etat_actuel) {
    static int clic_gauche_prec = 0;
    int clic_gauche_actuel = mouse_b & 1;
    GestionnaireCarte* etat = get_etat_carte(); Ville* villes = get_villes(); int nb = get_nb_villes();

    if (key[KEY_R] || key[KEY_ESC] || (mouse_b & 2)) {
        if (etat->mode_actuel == MODE_CARTE_INFO) {
            etat->mode_actuel = MODE_CARTE_PARCOURIR; etat->id_ville_selectionnee = -1;
        } else {
            return ETAT_MENU_PRINCIPAL;
        }
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
        if (clic_gauche_actuel && !clic_gauche_prec && etat->id_ville_survolee != -1) {
            if (son_beep) play_sample(son_beep, get_vol_sfx(), 128, 1000, 0);
            etat->id_ville_selectionnee = etat->id_ville_survolee;
            etat->mode_actuel = MODE_CARTE_INFO;
        }
    }
    else if (etat->mode_actuel == MODE_CARTE_INFO) {
        int px = etat->panel_x; int py = etat->panel_y;
        if (clic_gauche_actuel && !clic_gauche_prec) {
            if (souris_dans_rect(px + 20, py + 110, 90, 30, mouse_x, mouse_y)) {
                if (son_beep) play_sample(son_beep, get_vol_sfx(), 128, 1100, 0);
                Ville v = villes[etat->id_ville_selectionnee];
                initialiser_niveau(v.numero_niveau, SCREEN_W, SCREEN_H);
                return ETAT_JEU_EN_COURS;
            }
            if (souris_dans_rect(px + 130, py + 110, 90, 30, mouse_x, mouse_y)) {
                etat->mode_actuel = MODE_CARTE_PARCOURIR;
                etat->id_ville_selectionnee = -1;
            }
        }
    }
    clic_gauche_prec = clic_gauche_actuel;
    return etat_actuel;
}

EtatJeu gerer_ihm_generique_retour(EtatJeu etat_actuel) {
    if (key[KEY_R] || key[KEY_ESC]) {
        if (son_beep != NULL) play_sample(son_beep, get_vol_sfx(), 128, 1000, 0);
        return ETAT_MENU_PRINCIPAL;
    }
    return etat_actuel;
}

EtatJeu gerer_ihm_options(EtatJeu etat_actuel) {
    static int clic_precedent = 0;
    int clic_actuel = mouse_b & 1;

    if (key[KEY_R] || key[KEY_ESC]) {
        if (son_beep) play_sample(son_beep, get_vol_sfx(), 128, 1000, 0);
        return ETAT_MENU_PRINCIPAL;
    }

    if (mouse_b & 1) {
        int cx = SCREEN_W / 2; int bar_w = 200; int start_x = cx - (bar_w / 2);
        if (souris_dans_rect(start_x - 10, 170, bar_w + 20, 40, mouse_x, mouse_y)) {
            int new_vol = ((mouse_x - start_x) * 255) / bar_w;
            if(new_vol < 0) new_vol = 0; if(new_vol > 255) new_vol = 255;
            set_vol_musique(new_vol);
        }
        if (souris_dans_rect(start_x - 10, 270, bar_w + 20, 40, mouse_x, mouse_y)) {
            int new_vol = ((mouse_x - start_x) * 255) / bar_w;
            if(new_vol < 0) new_vol = 0; if(new_vol > 255) new_vol = 255;
            set_vol_sfx(new_vol);
        }
    }

    if (clic_actuel && !clic_precedent) {
        int cx = SCREEN_W / 2;
        if (souris_dans_rect(cx - 75, 400, 150, 30, mouse_x, mouse_y)) {
            toggle_plein_ecran();
            if (son_beep) play_sample(son_beep, get_vol_sfx(), 128, 1000, 0);
        }
    }
    clic_precedent = clic_actuel;
    return etat_actuel;
}

EtatJeu gerer_ihm_ecran_defaite(EtatJeu etat_actuel) {
    static int clic_precedent = 0;
    int clic_actuel = mouse_b & 1;

    if (clic_actuel && !clic_precedent) {
        int cx = SCREEN_W / 2; int cy = SCREEN_H / 2;
        if (souris_dans_rect(cx - 100, cy - 10, 200, 30, mouse_x, mouse_y)) {
            if (son_beep) play_sample(son_beep, get_vol_sfx(), 128, 1000, 0);
            initialiser_niveau(get_niveau_en_cours(), SCREEN_W, SCREEN_H);
            return ETAT_JEU_EN_COURS;
        }
        if (souris_dans_rect(cx - 100, cy + 40, 200, 30, mouse_x, mouse_y)) {
            if (son_beep) play_sample(son_beep, get_vol_sfx(), 128, 1000, 0);
            return ETAT_CARTE_MONDE;
        }
    }
    clic_precedent = clic_actuel;
    return etat_actuel;
}

EtatJeu gerer_ihm_pause(EtatJeu etat_courant) {
    static int clic_precedent = 0;
    int clic_actuel = mouse_b & 1;
    int cx = SCREEN_W / 2;
    int cy = SCREEN_H / 2;

    if (clic_actuel && !clic_precedent) {
        if (souris_dans_rect(cx - 100, cy - 30, 200, 30, mouse_x, mouse_y)) return ETAT_JEU_EN_COURS;
        if (souris_dans_rect(cx - 100, cy + 20, 200, 30, mouse_x, mouse_y)) {
            initialiser_niveau(get_niveau_en_cours(), SCREEN_W, SCREEN_H);
            return ETAT_JEU_EN_COURS;
        }
        if (souris_dans_rect(cx - 100, cy + 70, 200, 30, mouse_x, mouse_y)) return ETAT_CARTE_MONDE;
    }
    clic_precedent = clic_actuel;

    static int esc_presse = 1;
    if (key[KEY_ESC]) {
        if (!esc_presse) { esc_presse = 1; return ETAT_JEU_EN_COURS; }
    } else {
        esc_presse = 0;
    }
    return etat_courant;
}

EtatJeu gerer_ihm_ecran_victoire(EtatJeu etat_courant) {
    static int clic_precedent = 0;
    int clic_actuel = mouse_b & 1;
    int cx = SCREEN_W / 2; int cy = SCREEN_H / 2;

    if (clic_actuel && !clic_precedent) {
        if (souris_dans_rect(cx - 100, cy + 10, 200, 30, mouse_x, mouse_y)) return ETAT_CARTE_MONDE;
    }
    clic_precedent = clic_actuel;
    return etat_courant;
}
