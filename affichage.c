#include <allegro.h>
#include "affichage.h"
#include "constantes.h"
#include "menu.h"
#include "carte.h"
#include "ressources.h"
#include "options.h"
#include "logique.h"
#include "sauvegarde.h"

static BITMAP *buffer = NULL;
static int position_fond_x = 0;

void initialiser_affichage(int ecran_w, int ecran_h) { buffer = create_bitmap(ecran_w, ecran_h); }

void synchroniser_affichage() {
    if (mouse_sprite != NULL) draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
    blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

void detruire_affichage() { if (buffer != NULL) destroy_bitmap(buffer); }

void animer_fond_menu() {
    if (img_fond_menu != NULL) { position_fond_x -= 1; if (position_fond_x <= -img_fond_menu->w) position_fond_x = 0; }
}

void dessiner_menu_principal() {
    clear_to_color(buffer, makecol(0, 0, 0));
    if (img_fond_menu != NULL) { draw_sprite(buffer, img_fond_menu, position_fond_x, 0); draw_sprite(buffer, img_fond_menu, position_fond_x + img_fond_menu->w, 0); }
    if (img_logo != NULL) draw_sprite(buffer, img_logo, (SCREEN_W - img_logo->w) / 2, 40);

    // --- SCOREBOARD DIRECTEMENT SUR LE MENU (À GAUCHE) ---
    int bx = 20; int by = 150;
    set_trans_blender(0, 0, 0, 150);
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
    rectfill(buffer, bx, by, bx + 220, by + 250, makecol(0,0,0));
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
    rect(buffer, bx, by, bx + 220, by + 250, makecol(255, 255, 255));

    textout_centre_ex(buffer, font, "MEILLEURS SCORES", bx + 110, by + 20, makecol(255, 255, 0), -1);
    int* scores = get_scores();
    for (int i = 0; i < 5; i++) {
        textprintf_centre_ex(buffer, font, bx + 110, by + 70 + (i * 35), makecol(255, 255, 255), -1, "%d.  %05d PTS", i+1, scores[i]);
    }

    // --- DESSIN DES BOUTONS ---
    Bouton* boutons = get_boutons_menu_principal(); int nb = get_nb_boutons_menu_principal();
    for (int i = 0; i < nb; i++) {
        textout_centre_ex(buffer, font, boutons[i].texte, boutons[i].x + (boutons[i].largeur / 2) + 2, boutons[i].y + (boutons[i].hauteur / 2) + 2, makecol(0, 0, 0), -1);
        textout_centre_ex(buffer, font, boutons[i].texte, boutons[i].x + (boutons[i].largeur / 2), boutons[i].y + (boutons[i].hauteur / 2), makecol(255, 255, 255), -1);
        if (est_clic_sur_bouton(boutons[i], mouse_x, mouse_y) && img_curseur != NULL) draw_sprite(buffer, img_curseur, boutons[i].x - 40, boutons[i].y + (boutons[i].hauteur/2) - 16);
    }
}

void dessiner_carte_monde() {
    clear_to_color(buffer, makecol(0, 0, 0));
    GestionnaireCarte* etat = get_etat_carte(); Ville* villes = get_villes(); int nb = get_nb_villes();

    if (img_fond_map != NULL) stretch_sprite(buffer, img_fond_map, 0, 0, SCREEN_W, SCREEN_H);
    textout_centre_ex(buffer, font, "SELECTION DU NIVEAU", SCREEN_W/2, 20, makecol(255, 255, 255), makecol(0,0,0));

    int flag_w = 40, flag_h = 40;
    for (int i = 0; i < nb; i++) {
        if (villes[i].numero_niveau > get_niveau_debloque()) {
            circlefill(buffer, villes[i].x, villes[i].y, 10, makecol(100, 100, 100));
            textout_centre_ex(buffer, font, "BLOQUE", villes[i].x, villes[i].y + 15, makecol(150, 150, 150), makecol(0,0,0));
        } else {
            BITMAP* img_a_dessiner = img_drapeau_rouge;
            if (etat->mode_actuel == MODE_CARTE_PARCOURIR && villes[i].id == etat->id_ville_survolee) {
                img_a_dessiner = img_drapeau_jaune;
                textout_centre_ex(buffer, font, villes[i].nom, villes[i].x, villes[i].y - (flag_h/2) - 10, makecol(255, 255, 0), makecol(0,0,0));
            }
            if (img_a_dessiner != NULL) stretch_sprite(buffer, img_a_dessiner, villes[i].x - flag_w/2, villes[i].y - flag_h/2, flag_w, flag_h);
        }
    }

    if (etat->mode_actuel == MODE_CARTE_INFO && etat->id_ville_selectionnee != -1) {
        Ville v = villes[etat->id_ville_selectionnee];
        int panel_w = 260; int panel_h = 160;
        int px = v.x + 30; if (px + panel_w > SCREEN_W) px = v.x - panel_w - 30;
        int py = v.y - 50; if (py < 0) py = 10; if (py + panel_h > SCREEN_H) py = SCREEN_H - panel_h - 10;
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

// --- NOUVEAU MENU DE CHARGEMENT ---
void dessiner_charger_partie() {
    clear_to_color(buffer, makecol(30, 30, 80));
    textout_centre_ex(buffer, font, "REPRENDRE UNE PARTIE", SCREEN_W/2, 50, makecol(255, 255, 0), -1);

    int cx = SCREEN_W / 2;
    for(int i=0; i<4; i++) {
        int by = 130 + i * 90; // Espacement vertical
        rectfill(buffer, cx - 180, by, cx + 180, by + 70, makecol(50, 50, 100));
        rect(buffer, cx - 180, by, cx + 180, by + 70, makecol(255, 255, 255));

        if(get_info_save_existe(i+1)) {
            textprintf_centre_ex(buffer, font, cx, by + 15, makecol(255, 255, 0), -1, "- EMPLACEMENT %d -", i+1);
            textprintf_centre_ex(buffer, font, cx, by + 40, makecol(255, 255, 255), -1, "Niveau: %d   |   Score Total: %d", get_info_save_niveau(i+1), get_info_save_score(i+1));
        } else {
            textprintf_centre_ex(buffer, font, cx, by + 30, makecol(150, 150, 150), -1, "--- EMPLACEMENT VIDE ---");
        }
    }

    textout_centre_ex(buffer, font, "Appuyez sur 'R' ou 'ESC' pour revenir", cx, SCREEN_H - 40, makecol(255, 255, 0), -1);
}

void dessiner_options() {
    clear_to_color(buffer, makecol(30, 30, 80));
    textout_centre_ex(buffer, font, "OPTIONS", SCREEN_W/2, 50, makecol(255, 255, 0), -1);

    int cx = SCREEN_W / 2; int bar_w = 200; int start_x = cx - (bar_w / 2);

    textprintf_centre_ex(buffer, font, cx, 150, makecol(255, 255, 255), -1, "Volume Musique : %d%%", (get_vol_musique() * 100) / 255);
    rectfill(buffer, start_x, 180, start_x + bar_w, 200, makecol(100, 100, 100));
    int fill_mus = (get_vol_musique() * bar_w) / 255;
    rectfill(buffer, start_x, 180, start_x + fill_mus, 200, makecol(50, 200, 50));
    rectfill(buffer, start_x + fill_mus - 5, 175, start_x + fill_mus + 5, 205, makecol(255, 255, 255));

    textprintf_centre_ex(buffer, font, cx, 250, makecol(255, 255, 255), -1, "Volume Effets : %d%%", (get_vol_sfx() * 100) / 255);
    rectfill(buffer, start_x, 280, start_x + bar_w, 300, makecol(100, 100, 100));
    int fill_sfx = (get_vol_sfx() * bar_w) / 255;
    rectfill(buffer, start_x, 280, start_x + fill_sfx, 300, makecol(50, 200, 50));
    rectfill(buffer, start_x + fill_sfx - 5, 275, start_x + fill_sfx + 5, 305, makecol(255, 255, 255));

    textprintf_centre_ex(buffer, font, cx, 370, makecol(255, 255, 255), -1, "Affichage : %s", is_plein_ecran() ? "PLEIN ECRAN" : "FENETRE");
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

void dessiner_jeu_en_cours() {
    // 0. FOND DE L'ECRAN
    clear_to_color(buffer, makecol(135, 206, 235));

    Joueur* p = get_joueur();
    Tir* t = get_tirs();
    Ballon* b = get_ballons();
    RayonDanger* r = get_rayons();

    // 1. DESSINER LES RAYONS LASERS EN ARRIERE PLAN (SANS TRANSPARENCE)
    for (int i = 0; i < MAX_RAYONS; i++) {
        if (r[i].actif) {
            if (r[i].timer_avertissement > 0) {
                // AVERTISSEMENT : Fond rose pâle
                rectfill(buffer, r[i].x, 0, r[i].x + r[i].w, SCREEN_H, makecol(255, 200, 200));

                // Dessin de hachures diagonales pour marquer la zone de danger
                for(int y = -r[i].w; y < SCREEN_H; y += 30) {
                    line(buffer, r[i].x, y, r[i].x + r[i].w, y + r[i].w, makecol(255, 100, 100));
                    line(buffer, r[i].x, y+1, r[i].x + r[i].w, y + r[i].w + 1, makecol(255, 100, 100));
                }
            } else if (r[i].timer_actif > 0) {
                // RAYON MORTEL : Rouge vif intense
                rectfill(buffer, r[i].x, 0, r[i].x + r[i].w, SCREEN_H, makecol(255, 0, 0));

                // Bordures Jaunes épaisses
                rect(buffer, r[i].x, 0, r[i].x + r[i].w, SCREEN_H, makecol(255, 255, 0));
                rect(buffer, r[i].x + 1, 0, r[i].x + r[i].w - 1, SCREEN_H, makecol(255, 255, 0));
                rect(buffer, r[i].x + 2, 0, r[i].x + r[i].w - 2, SCREEN_H, makecol(255, 255, 0));
            }
        }
    }

    // 2. DESSINER LE JOUEUR ET LES TIRS
    rectfill(buffer, p->x, p->y, p->x + p->w, p->y + p->h, makecol(0, 0, 200));

    for (int i = 0; i < MAX_TIRS; i++) {
        if (t[i].actif) rectfill(buffer, t[i].x, t[i].y, t[i].x + t[i].w, t[i].y + t[i].h, makecol(255, 255, 0));
    }

    // 3. DESSINER LES BULLES AVEC LEURS COULEURS SPECIFIQUES
    for (int i = 0; i < MAX_BALLONS; i++) {
        if (b[i].actif) {
            int col_interieur, col_contour;

            switch(b[i].type) {
                case BULLE_CLASSIQUE:
                    col_interieur = makecol(200, 30, 30); col_contour = makecol(0, 0, 0);
                    break;
                case BULLE_MULTI:
                    col_interieur = makecol(150, 30, 200); col_contour = makecol(255, 255, 255);
                    break;
                case BULLE_EXPLOSIVE:
                    col_interieur = makecol(100, 100, 100); col_contour = makecol(255, 100, 0); // Gris foncé
                    break;
                default:
                    col_interieur = makecol(200, 30, 30); col_contour = makecol(0, 0, 0);
            }

            circlefill(buffer, b[i].x, b[i].y, b[i].rayon, col_interieur);
            circle(buffer, b[i].x, b[i].y, b[i].rayon, col_contour);
            circle(buffer, b[i].x, b[i].y, b[i].rayon - 1, col_contour); // Contour plus épais
        }
    }

    // 4. HUD ET INTERFACE
    textprintf_ex(buffer, font, 20, 20, makecol(255, 255, 255), -1, "SCORE TOTAL : %d", get_score_global_partie() + get_score_actuel());
    textprintf_ex(buffer, font, 20, 40, makecol(255, 255, 0), -1, "SCORE MANCHE: %d", get_score_actuel());
    textprintf_ex(buffer, font, 20, 60, makecol(255, 255, 0), -1, "OBJECTIF : %d / %d", get_objectif_actuel(), get_objectif_max());

    // DECOMPTE DU DEBUT
    int timer = get_timer_debut();
    if (timer > 0) {
        int secondes = (timer / 40) + 1;

        // On évite la transparence ici aussi au cas où pour le texte central
        textprintf_centre_ex(buffer, font, SCREEN_W/2, SCREEN_H/2, makecol(255, 0, 0), makecol(0,0,0), " - %d - ", secondes);
    }
}

void dessiner_ecran_defaite() {
    dessiner_jeu_en_cours();

    set_trans_blender(0, 0, 0, 200);
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
    rectfill(buffer, 0, 0, SCREEN_W, SCREEN_H, makecol(0,0,0));
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

    int cx = SCREEN_W / 2; int cy = SCREEN_H / 2;

    textout_centre_ex(buffer, font, "GAME OVER", cx, cy - 60, makecol(255, 0, 0), -1);

    rectfill(buffer, cx - 100, cy - 10, cx + 100, cy + 20, makecol(50, 150, 50));
    rect(buffer, cx - 100, cy - 10, cx + 100, cy + 20, makecol(255, 255, 255));
    textout_centre_ex(buffer, font, "REJOUER LE NIVEAU", cx, cy + 2, makecol(255, 255, 255), -1);

    rectfill(buffer, cx - 100, cy + 40, cx + 100, cy + 70, makecol(150, 50, 50));
    rect(buffer, cx - 100, cy + 40, cx + 100, cy + 70, makecol(255, 255, 255));
    textout_centre_ex(buffer, font, "RETOUR A LA CARTE", cx, cy + 52, makecol(255, 255, 255), -1);
}
