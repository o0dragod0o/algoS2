#include <allegro.h>
#include "affichage.h"
#include "constantes.h"
#include "menu.h"
#include "carte.h"
#include "ressources.h"
#include "options.h" // NOUVEAU

static BITMAP *buffer = NULL;
static int position_fond_x = 0;

void initialiser_affichage(int ecran_w, int ecran_h) {
    buffer = create_bitmap(ecran_w, ecran_h);
}

void synchroniser_affichage() { blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H); }
void detruire_affichage() { if (buffer != NULL) destroy_bitmap(buffer); }

void animer_fond_menu() {
    if (img_fond_menu != NULL) {
        position_fond_x -= 1;
        if (position_fond_x <= -img_fond_menu->w) position_fond_x = 0;
    }
}

void dessiner_menu_principal() {
    clear_to_color(buffer, makecol(0, 0, 0));

    if (img_fond_menu != NULL) {
        draw_sprite(buffer, img_fond_menu, position_fond_x, 0);
        draw_sprite(buffer, img_fond_menu, position_fond_x + img_fond_menu->w, 0);
    }
    if (img_logo != NULL) draw_sprite(buffer, img_logo, (SCREEN_W - img_logo->w) / 2, 40);

    Bouton* boutons = get_boutons_menu_principal();
    int nb = get_nb_boutons_menu_principal();

    for (int i = 0; i < nb; i++) {
        textout_centre_ex(buffer, font, boutons[i].texte, boutons[i].x + (boutons[i].largeur / 2) + 2, boutons[i].y + (boutons[i].hauteur / 2) + 2, makecol(0, 0, 0), -1);
        textout_centre_ex(buffer, font, boutons[i].texte, boutons[i].x + (boutons[i].largeur / 2), boutons[i].y + (boutons[i].hauteur / 2), makecol(255, 255, 255), -1);

        if (est_clic_sur_bouton(boutons[i], mouse_x, mouse_y) && img_curseur != NULL) {
            draw_sprite(buffer, img_curseur, boutons[i].x - 40, boutons[i].y + (boutons[i].hauteur/2) - 16);
        }
    }
}

void dessiner_carte_monde() {
    clear_to_color(buffer, makecol(0, 0, 0));
    GestionnaireCarte* etat = get_etat_carte();
    Ville* villes = get_villes();
    int nb = get_nb_villes();

    if (img_fond_map != NULL) stretch_sprite(buffer, img_fond_map, 0, 0, SCREEN_W, SCREEN_H);
    textout_centre_ex(buffer, font, "SELECTION DU NIVEAU", SCREEN_W/2, 20, makecol(255, 255, 255), makecol(0,0,0));

    int flag_w = 40, flag_h = 40;
    for (int i = 0; i < nb; i++) {
        BITMAP* img_a_dessiner = img_drapeau_rouge;
        if (etat->mode_actuel == MODE_CARTE_PARCOURIR && villes[i].id == etat->id_ville_survolee) {
            img_a_dessiner = img_drapeau_jaune;
            textout_centre_ex(buffer, font, villes[i].nom, villes[i].x, villes[i].y - (flag_h/2) - 10, makecol(255, 255, 0), makecol(0,0,0));
        }
        if (img_a_dessiner != NULL) {
            stretch_sprite(buffer, img_a_dessiner, villes[i].x - flag_w/2, villes[i].y - flag_h/2, flag_w, flag_h);
        }
    }

    if (etat->mode_actuel == MODE_CARTE_INFO && etat->id_ville_selectionnee != -1) {
        Ville v = villes[etat->id_ville_selectionnee];
        int panel_w = 260; int panel_h = 160;
        int px = v.x + 30;
        if (px + panel_w > SCREEN_W) px = v.x - panel_w - 30;
        int py = v.y - 50;
        if (py < 0) py = 10;
        if (py + panel_h > SCREEN_H) py = SCREEN_H - panel_h - 10;
        etat->panel_x = px; etat->panel_y = py;

        if (img_panneau_info != NULL) {
            stretch_sprite(buffer, img_panneau_info, px, py, panel_w, panel_h);
            textout_ex(buffer, font, v.nom, px + 20, py + 20, makecol(255, 255, 255), -1);
            hline(buffer, px+20, py+35, px+panel_w-20, makecol(200, 200, 200));
            textout_ex(buffer, font, v.desc_ligne1, px + 20, py + 50, makecol(255, 255, 0), -1);
            textout_ex(buffer, font, v.desc_ligne2, px + 20, py + 70, makecol(200, 200, 200), -1);

            rectfill(buffer, px + 20, py + 110, px + 110, py + 140, makecol(50, 150, 50));
            rect(buffer, px + 20, py + 110, px + 110, py + 140, makecol(255, 255, 255));
            textout_centre_ex(buffer, font, "JOUER", px + 65, py + 120, makecol(255, 255, 255), -1);

            rectfill(buffer, px + 130, py + 110, px + 220, py + 140, makecol(150, 50, 50));
            rect(buffer, px + 130, py + 110, px + 220, py + 140, makecol(255, 255, 255));
            textout_centre_ex(buffer, font, "FERMER", px + 175, py + 120, makecol(255, 255, 255), -1);
        }
    }
}

// Dessin des options
void dessiner_options() {
    clear_to_color(buffer, makecol(30, 30, 80));
    textout_centre_ex(buffer, font, "OPTIONS", SCREEN_W/2, 50, makecol(255, 255, 0), -1);

    int cx = SCREEN_W / 2;
    int bar_w = 200; // Largeur de la barre
    int start_x = cx - (bar_w / 2);

    // --- 1. SLIDER MUSIQUE ---
    textprintf_centre_ex(buffer, font, cx, 150, makecol(255, 255, 255), -1, "Volume Musique : %d%%", (get_vol_musique() * 100) / 255);
    // Fond de la barre (Gris)
    rectfill(buffer, start_x, 180, start_x + bar_w, 200, makecol(100, 100, 100));
    // Remplissage (Vert) calculé proportionnellement
    int fill_mus = (get_vol_musique() * bar_w) / 255;
    rectfill(buffer, start_x, 180, start_x + fill_mus, 200, makecol(50, 200, 50));
    // Curseur (Carré blanc)
    rectfill(buffer, start_x + fill_mus - 5, 175, start_x + fill_mus + 5, 205, makecol(255, 255, 255));

    // --- 2. SLIDER EFFETS ---
    textprintf_centre_ex(buffer, font, cx, 250, makecol(255, 255, 255), -1, "Volume Effets : %d%%", (get_vol_sfx() * 100) / 255);
    // Fond de la barre (Gris)
    rectfill(buffer, start_x, 280, start_x + bar_w, 300, makecol(100, 100, 100));
    // Remplissage (Vert)
    int fill_sfx = (get_vol_sfx() * bar_w) / 255;
    rectfill(buffer, start_x, 280, start_x + fill_sfx, 300, makecol(50, 200, 50));
    // Curseur (Carré blanc)
    rectfill(buffer, start_x + fill_sfx - 5, 275, start_x + fill_sfx + 5, 305, makecol(255, 255, 255));

    // --- 3. BOUTON RESOLUTION ---
    textprintf_centre_ex(buffer, font, cx, 370, makecol(255, 255, 255), -1, "Resolution : %dx%d", get_res_w(), get_res_h());
    rectfill(buffer, cx - 75, 400, cx + 75, 430, makecol(100, 100, 200));
    textout_centre_ex(buffer, font, "CHANGER", cx, 410, makecol(255, 255, 255), -1);

    textout_centre_ex(buffer, font, "Appuyez sur 'R' ou 'ESC' pour revenir", cx, SCREEN_H - 50, makecol(255, 255, 0), -1);
}
void dessiner_regles() {
    clear_to_color(buffer, makecol(80, 30, 30));
    textout_centre_ex(buffer, font, "REGLES DU JEU", SCREEN_W/2, 50, makecol(255, 255, 255), -1);
    textout_centre_ex(buffer, font, "- Eclatez toutes les bulles pour gagner.", SCREEN_W/2, 150, makecol(200, 200, 200), -1);
    textout_centre_ex(buffer, font, "- Evitez le contact avec les bulles.", SCREEN_W/2, 180, makecol(200, 200, 200), -1);
    textout_centre_ex(buffer, font, "Appuyez sur 'R' ou 'ESC' pour revenir", SCREEN_W/2, SCREEN_H - 50, makecol(255, 255, 0), -1);
}

void dessiner_sauvegarde() {
    clear_to_color(buffer, makecol(30, 80, 30));
    textout_centre_ex(buffer, font, "MENU DES SAUVEGARDES (Bientot dispo)", SCREEN_W/2, 50, makecol(255, 255, 255), -1);
    textout_centre_ex(buffer, font, "Appuyez sur 'R' ou 'ESC' pour revenir", SCREEN_W/2, SCREEN_H - 50, makecol(255, 255, 0), -1);
}
