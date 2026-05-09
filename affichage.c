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

void dessiner_charger_partie() {
    clear_to_color(buffer, makecol(30, 30, 80));

    // Déclaration pour récupérer le mode (0=Nouveau, 2=Reprendre)
    extern int mode_action_saisie;

    if (mode_action_saisie == 2) {
        textout_centre_ex(buffer, font, "REPRENDRE UNE PARTIE", SCREEN_W/2, 50, makecol(255, 255, 0), -1);
    } else {
        textout_centre_ex(buffer, font, "NOUVELLE PARTIE", SCREEN_W/2, 50, makecol(255, 255, 0), -1);
    }

    int cx = SCREEN_W / 2;
    int cy = SCREEN_H / 2;

    textout_centre_ex(buffer, font, "ENTREZ VOTRE PSEUDO :", cx, cy - 40, makecol(255, 255, 255), -1);

    // Rectangle pour la saisie
    rectfill(buffer, cx - 150, cy - 15, cx + 150, cy + 25, makecol(0, 0, 0));
    rect(buffer, cx - 150, cy - 15, cx + 150, cy + 25, makecol(255, 255, 255));

    // Affichage du pseudo avec un petit underscore
    char affichage_pseudo[30];
    sprintf(affichage_pseudo, "%s_", pseudo_actuel);
    textout_centre_ex(buffer, font, affichage_pseudo, cx, cy, makecol(0, 255, 0), -1);

    if (message_erreur_pseudo) {
        textout_centre_ex(buffer, font, "ERREUR: PSEUDO INTROUVABLE !", cx, cy + 50, makecol(255, 0, 0), -1);
    }

    textout_centre_ex(buffer, font, "Appuyez sur ENTRER pour valider", cx, SCREEN_H - 80, makecol(200, 200, 200), -1);
    textout_centre_ex(buffer, font, "Appuyez sur 'ESC' pour revenir", cx, SCREEN_H - 50, makecol(255, 255, 0), -1);
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
    int niveau = get_niveau_en_cours();

    if (niveau >= 1 && niveau <= 4 && img_fonds_niveaux[niveau - 1] != NULL) {
        stretch_blit(img_fonds_niveaux[niveau - 1], buffer, 0, 0, img_fonds_niveaux[niveau - 1]->w, img_fonds_niveaux[niveau - 1]->h, 0, 0, SCREEN_W, SCREEN_H);
    } else {
        clear_to_color(buffer, makecol(135, 206, 235));
    }

    Joueur* jrs = get_joueurs();
    Tir* t = get_tirs();
    Ballon* b = get_ballons();
    RayonDanger* r = get_rayons();
    Explosion* e = get_explosions();
    Boss* boss = get_boss();

    for (int i = 0; i < MAX_RAYONS; i++) {
        if (r[i].actif) {
            if (r[i].timer_avertissement > 0) {
                rectfill(buffer, r[i].x, 0, r[i].x + r[i].w, SCREEN_H, makecol(255, 200, 200));
                for(int y = -r[i].w; y < SCREEN_H; y += 30) line(buffer, r[i].x, y, r[i].x + r[i].w, y + r[i].w, makecol(255, 100, 100));
            } else if (r[i].timer_actif > 0) {
                rectfill(buffer, r[i].x, 0, r[i].x + r[i].w, SCREEN_H, makecol(255, 0, 0));
                rect(buffer, r[i].x, 0, r[i].x + r[i].w, SCREEN_H, makecol(255, 255, 0));
            }
        }
    }

    if (boss->actif) {
        if (img_boss != NULL) stretch_sprite(buffer, img_boss, boss->x, boss->y, boss->w, boss->h);
        else rectfill(buffer, boss->x, boss->y, boss->x + boss->w, boss->y + boss->h, makecol(50, 0, 0));

        int barre_w = 400; int hp_w = (boss->pv_actuel * barre_w) / boss->pv_max; if (hp_w < 0) hp_w = 0;
        int bx = (SCREEN_W - barre_w) / 2;
        rectfill(buffer, bx, 10, bx + barre_w, 25, makecol(100, 0, 0));
        rectfill(buffer, bx, 10, bx + hp_w, 25, makecol(255, 0, 0));
        rect(buffer, bx, 10, bx + barre_w, 25, makecol(255, 255, 255));
        if (boss->phase == 3) textout_centre_ex(buffer, font, "!!! ENRAGE !!!", SCREEN_W/2, 14, makecol(255, 255, 0), -1);
        else textout_centre_ex(buffer, font, "LE ROI DES BULLES", SCREEN_W/2, 14, makecol(255, 255, 255), -1);
    }

    // --- DESSIN DES JOUEURS (Correction : Retrait de la transparence et taille respectée) ---
    for (int p = 0; p < 2; p++) {
        if (jrs[p].actif && !jrs[p].est_mort) {
            if (jrs[p].timer_invincibilite == 0 || (jrs[p].timer_invincibilite % 10 < 5)) {
                if (img_player != NULL) {
                    // On utilise les dimensions w et h définies dans la logique (20x26)
                    stretch_sprite(buffer, img_player, jrs[p].x, jrs[p].y, jrs[p].w, jrs[p].h);
                } else {
                    int col = (p == 0) ? makecol(0, 0, 200) : makecol(200, 0, 0);
                    rectfill(buffer, jrs[p].x, jrs[p].y, jrs[p].x + jrs[p].w, jrs[p].y + jrs[p].h, col);
                }
                textout_centre_ex(buffer, font, (p == 0) ? "P1" : "P2", jrs[p].x + jrs[p].w / 2, jrs[p].y - 15, makecol(255, 255, 255), -1);
            }
        }
    }

    for (int i = 0; i < MAX_TIRS; i++) {
        if (t[i].actif) {
            int col_tir = (t[i].id_joueur == 0) ? makecol(255, 255, 0) : makecol(255, 150, 0);
            rectfill(buffer, t[i].x, t[i].y, t[i].x + t[i].w, t[i].y + t[i].h, col_tir);
        }
    }

    for (int i = 0; i < MAX_BALLONS; i++) {
        if (b[i].actif) {
            BITMAP* sprite_bulle = NULL;
            int col_secours = makecol(200, 30, 30);

            if (b[i].type == BULLE_CLASSIQUE) { sprite_bulle = img_bulle_rouge; col_secours = makecol(200, 30, 30); }
            else if (b[i].type == BULLE_MULTI) { sprite_bulle = img_bulle_violette; col_secours = makecol(150, 30, 200); }
            else if (b[i].type == BULLE_EXPLOSIVE) { sprite_bulle = img_bulle_jaune; col_secours = makecol(200, 200, 30); }

            if (sprite_bulle != NULL) {
                stretch_sprite(buffer, sprite_bulle, b[i].x - b[i].rayon, b[i].y - b[i].rayon, b[i].rayon * 2, b[i].rayon * 2);
            } else {
                circlefill(buffer, b[i].x, b[i].y, b[i].rayon, col_secours);
                circle(buffer, b[i].x, b[i].y, b[i].rayon, makecol(0,0,0));
            }
        }
    }

    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (e[i].actif && img_explosion[e[i].frame_actuelle] != NULL) {
            stretch_sprite(buffer, img_explosion[e[i].frame_actuelle], e[i].x - (e[i].taille / 2), e[i].y - (e[i].taille / 2), e[i].taille, e[i].taille);
        }
    }

    textprintf_ex(buffer, font, 20, 20, makecol(255, 255, 255), -1, "SCORE TOTAL : %d", get_score_global_partie() + get_score_actuel());

    if (niveau == 4) {
        for(int p=0; p < get_nombre_joueurs_mode(); p++) {
            int x_barre = (p == 0) ? 50 : SCREEN_W - 250;
            int y_barre = SCREEN_H - 30;

            textprintf_ex(buffer, font, x_barre, y_barre - 15, makecol(255,255,255), -1, "VIE P%d", p+1);
            rectfill(buffer, x_barre, y_barre, x_barre + 200, y_barre + 15, makecol(50, 50, 50));

            int largeur_vie = (jrs[p].vies * 200) / 3;
            int col_vie = (jrs[p].vies > 1) ? makecol(0, 255, 0) : makecol(255, 0, 0);
            if (jrs[p].est_mort) largeur_vie = 0;

            rectfill(buffer, x_barre, y_barre, x_barre + largeur_vie, y_barre + 15, col_vie);
            rect(buffer, x_barre, y_barre, x_barre + 200, y_barre + 15, makecol(255, 255, 255));
        }
    } else {
        textprintf_ex(buffer, font, 20, 40, makecol(255, 255, 0), -1, "OBJECTIF : %d / %d", get_objectif_actuel(), get_objectif_max());
    }

    int timer = get_timer_debut();
    if (timer > 0) textprintf_centre_ex(buffer, font, SCREEN_W/2, SCREEN_H/2, makecol(255, 0, 0), makecol(0,0,0), " - %d - ", (timer / 40) + 1);
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

void dessiner_pause() {
    dessiner_jeu_en_cours(); // On dessine le jeu derrière

    // Fond sombre transparent
    set_trans_blender(0, 0, 0, 150);
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
    rectfill(buffer, 0, 0, SCREEN_W, SCREEN_H, makecol(0,0,0));
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

    int cx = SCREEN_W / 2; int cy = SCREEN_H / 2;

    textout_centre_ex(buffer, font, "PAUSE", cx, cy - 80, makecol(255, 255, 0), -1);

    // Bouton Reprendre
    rectfill(buffer, cx - 100, cy - 30, cx + 100, cy, makecol(50, 150, 50));
    rect(buffer, cx - 100, cy - 30, cx + 100, cy, makecol(255, 255, 255));
    textout_centre_ex(buffer, font, "REPRENDRE", cx, cy - 18, makecol(255, 255, 255), -1);

    // Bouton Recommencer
    rectfill(buffer, cx - 100, cy + 20, cx + 100, cy + 50, makecol(200, 100, 0));
    rect(buffer, cx - 100, cy + 20, cx + 100, cy + 50, makecol(255, 255, 255));
    textout_centre_ex(buffer, font, "RECOMMENCER", cx, cy + 32, makecol(255, 255, 255), -1);

    // Bouton Quitter
    rectfill(buffer, cx - 100, cy + 70, cx + 100, cy + 100, makecol(150, 50, 50));
    rect(buffer, cx - 100, cy + 70, cx + 100, cy + 100, makecol(255, 255, 255));
    textout_centre_ex(buffer, font, "QUITTER (CARTE)", cx, cy + 82, makecol(255, 255, 255), -1);
}

void dessiner_ecran_victoire() {
    dessiner_jeu_en_cours(); // On dessine le jeu derrière

    set_trans_blender(0, 0, 0, 200);
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
    rectfill(buffer, 0, 0, SCREEN_W, SCREEN_H, makecol(0,0,0));
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

    int cx = SCREEN_W / 2; int cy = SCREEN_H / 2;

    textout_centre_ex(buffer, font, "NIVEAU TERMINE !", cx, cy - 60, makecol(0, 255, 0), -1);
    textprintf_centre_ex(buffer, font, cx, cy - 30, makecol(255, 255, 255), -1, "Score de la manche : %d", get_score_actuel());

    rectfill(buffer, cx - 100, cy + 10, cx + 100, cy + 40, makecol(50, 150, 50));
    rect(buffer, cx - 100, cy + 10, cx + 100, cy + 40, makecol(255, 255, 255));
    textout_centre_ex(buffer, font, "CONTINUER", cx, cy + 22, makecol(255, 255, 255), -1);
}
