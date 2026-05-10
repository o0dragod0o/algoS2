#include <allegro.h>
#include <stdio.h>
#include "affichage.h"
#include "constantes.h"
#include "menu.h"
#include "carte.h"
#include "ressources.h"
#include "options.h"
#include "logique.h"
#include "sauvegarde.h"

// Variables globales (sans static)
BITMAP *buffer = NULL;  // Image invisible sur laquelle on dessine tout
int position_fond_x = 0; // Gère le défilement du fond d'écran

// Allocation de la mémoire pour l'image buffer
void initialiser_affichage(int ecran_w, int ecran_h) {
    buffer = create_bitmap(ecran_w, ecran_h);
}

// Affiche le buffer sur l'écran physique proprement
void synchroniser_affichage() {
    show_mouse(buffer); // Affiche la souris sur le buffer
    blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H); // Envoi à l'écran
    show_mouse(NULL); // Cache la souris pour éviter les traces
}

// Nettoyage de la mémoire à la fermeture du jeu
void detruire_affichage() {
    if (buffer != NULL) destroy_bitmap(buffer);
}

// Défilement horizontal infini de l'image de fond
void animer_fond_menu() {
    if (img_fond_menu != NULL) {
        position_fond_x -= 1; // Déplace vers la gauche
        if (position_fond_x <= -img_fond_menu->w) position_fond_x = 0; // Boucle
    }
}

// ==========================================
// SOUS-FONCTIONS (MENUS)
// ==========================================

// Dessine le fond d'écran avec un filtre noir semi-transparent par-dessus
void dessiner_fond_menu_assombri() {
    clear_to_color(buffer, makecol(0, 0, 0)); // Fond noir de sécurité

    // Dessin du fond défilant (en double pour ne pas voir de coupure)
    if (img_fond_menu != NULL) {
        draw_sprite(buffer, img_fond_menu, position_fond_x, 0);
        draw_sprite(buffer, img_fond_menu, position_fond_x + img_fond_menu->w, 0);
    }

    // Filtre transparent central
    set_trans_blender(0, 0, 0, 180);
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
    rectfill(buffer, 100, 100, SCREEN_W - 100, SCREEN_H - 100, makecol(0,0,0));
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

    // Cadre blanc autour du filtre
    rect(buffer, 100, 100, SCREEN_W - 100, SCREEN_H - 100, makecol(255, 255, 255));
}

// ==========================================
// AFFICHAGE DES MENUS
// ==========================================

void dessiner_menu_principal() {
    clear_to_color(buffer, makecol(0, 0, 0));

    // Fond défilant
    if (img_fond_menu != NULL) {
        draw_sprite(buffer, img_fond_menu, position_fond_x, 0);
        draw_sprite(buffer, img_fond_menu, position_fond_x + img_fond_menu->w, 0);
    }

    // Affichage du Logo en haut au centre
    if (img_logo != NULL) draw_sprite(buffer, img_logo, (SCREEN_W - img_logo->w) / 2, 40);

    // --- Dessin du Tableau des scores à gauche ---
    int bx = 20; int by = 270;
    set_trans_blender(0, 0, 0, 150);
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
    rectfill(buffer, bx, by, bx + 220, by + 250, makecol(0,0,0)); // Fond transparent
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
    rect(buffer, bx, by, bx + 220, by + 250, makecol(255, 255, 255)); // Cadre

    textout_centre_ex(buffer, font, "MEILLEURS SCORES", bx + 110, by + 20, makecol(255, 255, 0), -1);

    // Récupération et affichage du Top 5
    char top_pseudos[5][20]; int top_scores[5];
    get_top_5_scores(top_pseudos, top_scores);

    for (int i = 0; i < 5; i++) {
        textprintf_ex(buffer, font, bx + 10, by + 60 + (i * 35), makecol(255, 255, 255), -1, "%d. %s", i+1, top_pseudos[i]);
        textprintf_ex(buffer, font, bx + 10, by + 75 + (i * 35), makecol(200, 200, 200), -1, "   %05d PTS", top_scores[i]);
    }

    // --- Dessin des boutons du menu ---
    Bouton* boutons = get_boutons_menu_principal();
    int nb = get_nb_boutons_menu_principal();

    for (int i = 0; i < nb; i++) {
        // Ombre portée du texte
        textout_centre_ex(buffer, font, boutons[i].texte, boutons[i].x + (boutons[i].largeur / 2) + 2, boutons[i].y + (boutons[i].hauteur / 2) + 2, makecol(0, 0, 0), -1);
        // Texte principal
        textout_centre_ex(buffer, font, boutons[i].texte, boutons[i].x + (boutons[i].largeur / 2), boutons[i].y + (boutons[i].hauteur / 2), makecol(255, 255, 255), -1);

        // Affichage du curseur étoile si la souris survole le bouton
        if (est_clic_sur_bouton(boutons[i], mouse_x, mouse_y) && img_curseur != NULL) {
            draw_sprite(buffer, img_curseur, boutons[i].x - 40, boutons[i].y + (boutons[i].hauteur/2) - 16);
        }
    }
}

// Menu de saisie du pseudo
void dessiner_charger_partie() {
    dessiner_fond_menu_assombri();

    extern int mode_action_saisie; // Savoir si c'est "Nouvelle" ou "Reprendre"
    if (mode_action_saisie == 2) textout_centre_ex(buffer, font, "REPRENDRE UNE PARTIE", SCREEN_W/2, 130, makecol(255, 255, 0), -1);
    else textout_centre_ex(buffer, font, "NOUVELLE PARTIE", SCREEN_W/2, 130, makecol(255, 255, 0), -1);

    int cx = SCREEN_W / 2; int cy = SCREEN_H / 2;
    textout_centre_ex(buffer, font, "ENTREZ VOTRE PSEUDO :", cx, cy - 40, makecol(255, 255, 255), -1);

    // Boîte de texte blanche
    rectfill(buffer, cx - 150, cy - 15, cx + 150, cy + 25, makecol(0, 0, 0));
    rect(buffer, cx - 150, cy - 15, cx + 150, cy + 25, makecol(255, 255, 255));

    // Affichage du pseudo en cours de frappe
    char affichage_pseudo[30];
    sprintf(affichage_pseudo, "%s_", pseudo_actuel); // Ajout du curseur "_"
    textout_centre_ex(buffer, font, affichage_pseudo, cx, cy, makecol(0, 255, 0), -1);

    // Message d'erreur si la sauvegarde n'existe pas
    if (message_erreur_pseudo) textout_centre_ex(buffer, font, "ERREUR: PSEUDO INTROUVABLE !", cx, cy + 50, makecol(255, 0, 0), -1);

    textout_centre_ex(buffer, font, "Appuyez sur ENTRER pour valider", cx, SCREEN_H - 170, makecol(200, 200, 200), -1);
    textout_centre_ex(buffer, font, "Appuyez sur 'ESC' ou Clic Droit pour revenir", cx, SCREEN_H - 140, makecol(255, 255, 0), -1);
}

// Menu des paramètres
void dessiner_options() {
    dessiner_fond_menu_assombri();
    textout_centre_ex(buffer, font, "OPTIONS", SCREEN_W/2, 130, makecol(255, 255, 0), -1);

    int cx = SCREEN_W / 2; int bar_w = 200; int start_x = cx - (bar_w / 2);

    // --- Barre de Volume Musique ---
    textprintf_centre_ex(buffer, font, cx, 180, makecol(255, 255, 255), -1, "Volume Musique : %d%%", (get_vol_musique() * 100) / 255);
    rectfill(buffer, start_x, 210, start_x + bar_w, 230, makecol(100, 100, 100)); // Fond gris
    int fill_mus = (get_vol_musique() * bar_w) / 255;
    rectfill(buffer, start_x, 210, start_x + fill_mus, 230, makecol(50, 200, 50)); // Jauge verte
    rectfill(buffer, start_x + fill_mus - 5, 205, start_x + fill_mus + 5, 235, makecol(255, 255, 255)); // Curseur blanc

    // --- Barre de Volume Bruitages (SFX) ---
    textprintf_centre_ex(buffer, font, cx, 280, makecol(255, 255, 255), -1, "Volume Effets : %d%%", (get_vol_sfx() * 100) / 255);
    rectfill(buffer, start_x, 310, start_x + bar_w, 330, makecol(100, 100, 100)); // Fond gris
    int fill_sfx = (get_vol_sfx() * bar_w) / 255;
    rectfill(buffer, start_x, 310, start_x + fill_sfx, 330, makecol(50, 200, 50)); // Jauge verte
    rectfill(buffer, start_x + fill_sfx - 5, 305, start_x + fill_sfx + 5, 335, makecol(255, 255, 255)); // Curseur blanc

    // --- Bouton Plein Écran ---
    textprintf_centre_ex(buffer, font, cx, 380, makecol(255, 255, 255), -1, "Affichage : %s", is_plein_ecran() ? "PLEIN ECRAN" : "FENETRE");
    rectfill(buffer, cx - 75, 410, cx + 75, 440, makecol(100, 100, 200)); // Bouton bleu
    textout_centre_ex(buffer, font, "CHANGER", cx, 420, makecol(255, 255, 255), -1);

    textout_centre_ex(buffer, font, "Appuyez sur 'ESC' ou Clic Droit pour revenir", cx, SCREEN_H - 140, makecol(255, 255, 0), -1);
}

// Menu des explications
void dessiner_regles() {
    dessiner_fond_menu_assombri();

    int cx = SCREEN_W / 2;
    textout_centre_ex(buffer, font, "REGLES DU JEU", cx, 130, makecol(255, 255, 0), -1);

    textout_centre_ex(buffer, font, "- Tirez sur les bulles pour les faire eclater.", cx, 200, makecol(200, 200, 200), -1);
    textout_centre_ex(buffer, font, "- Evitez tout contact avec elles ou les lasers.", cx, 230, makecol(200, 200, 200), -1);
    textout_centre_ex(buffer, font, "- Recuperez les essences pour changer d'arme !", cx, 260, makecol(200, 200, 200), -1);
    textout_centre_ex(buffer, font, "- Joueur 1 : Fleches et Espace", cx, 310, makecol(100, 200, 255), -1);
    textout_centre_ex(buffer, font, "- Joueur 2 : Q/D et Z ", cx, 340, makecol(255, 100, 100), -1);

    textout_centre_ex(buffer, font, "Appuyez sur 'ESC' ou Clic Droit pour revenir", cx, SCREEN_H - 140, makecol(255, 255, 0), -1);
}

// Map du monde pour choisir le niveau
void dessiner_carte_monde() {
    clear_to_color(buffer, makecol(0, 0, 0));
    GestionnaireCarte* etat = get_etat_carte();
    Ville* villes = get_villes();
    int nb = get_nb_villes();

    // Fond de la carte étiré à l'écran
    if (img_fond_map != NULL) stretch_blit(img_fond_map, buffer, 0, 0, img_fond_map->w, img_fond_map->h, 0, 0, SCREEN_W, SCREEN_H);
    textout_centre_ex(buffer, font, "SELECTION DU NIVEAU", SCREEN_W/2, 20, makecol(255, 255, 255), makecol(0,0,0));

    int flag_w = 40, flag_h = 40;
    for (int i = 0; i < nb; i++) {
        // Niveau non débloqué : Cercle gris
        if (villes[i].numero_niveau > get_niveau_debloque()) {
            circlefill(buffer, villes[i].x, villes[i].y, 10, makecol(100, 100, 100));
            textout_centre_ex(buffer, font, "BLOQUE", villes[i].x, villes[i].y + 15, makecol(150, 150, 150), makecol(0,0,0));
        } else {
            // Niveau débloqué : Drapeau Rouge (ou jaune si survolé)
            BITMAP* img_a_dessiner = img_drapeau_rouge;
            if (etat->mode_actuel == MODE_CARTE_PARCOURIR && villes[i].id == etat->id_ville_survolee) {
                img_a_dessiner = img_drapeau_jaune;
                textout_centre_ex(buffer, font, villes[i].nom, villes[i].x, villes[i].y - (flag_h/2) - 10, makecol(255, 255, 0), makecol(0,0,0));
            }
            if (img_a_dessiner != NULL) stretch_sprite(buffer, img_a_dessiner, villes[i].x - flag_w/2, villes[i].y - flag_h/2, flag_w, flag_h);
        }
    }

    // Affichage du panneau d'information si on a cliqué sur une ville
    if (etat->mode_actuel == MODE_CARTE_INFO && etat->id_ville_selectionnee != -1) {
        Ville v = villes[etat->id_ville_selectionnee];
        int panel_w = 260; int panel_h = 160;

        // Ajustement des coordonnées pour ne pas sortir de l'écran
        int px = v.x + 30; if (px + panel_w > SCREEN_W) px = v.x - panel_w - 30;
        int py = v.y - 50; if (py < 0) py = 10; if (py + panel_h > SCREEN_H) py = SCREEN_H - panel_h - 10;
        etat->panel_x = px; etat->panel_y = py;

        if (img_panneau_info != NULL) {
            stretch_sprite(buffer, img_panneau_info, px, py, panel_w, panel_h);
            textout_ex(buffer, font, v.nom, px + 20, py + 20, makecol(255, 255, 255), -1);
            hline(buffer, px+20, py+35, px+panel_w-20, makecol(200, 200, 200));
            textout_ex(buffer, font, v.desc_ligne1, px + 20, py + 50, makecol(255, 255, 0), -1);
            textout_ex(buffer, font, v.desc_ligne2, px + 20, py + 70, makecol(200, 200, 200), -1);

            // Bouton JOUER
            rectfill(buffer, px + 20, py + 110, px + 110, py + 140, makecol(50, 150, 50));
            rect(buffer, px + 20, py + 110, px + 110, py + 140, makecol(255, 255, 255));
            textout_centre_ex(buffer, font, "JOUER", px + 65, py + 120, makecol(255, 255, 255), -1);

            // Bouton FERMER
            rectfill(buffer, px + 130, py + 110, px + 220, py + 140, makecol(150, 50, 50));
            rect(buffer, px + 130, py + 110, px + 220, py + 140, makecol(255, 255, 255));
            textout_centre_ex(buffer, font, "FERMER", px + 175, py + 120, makecol(255, 255, 255), -1);
        }
    }
}

// ==========================================
// SOUS-FONCTIONS (JEU EN COURS)
// ==========================================

// Gère le fond en fonction du niveau
void dessiner_fond_jeu(int niveau) {
    if (niveau >= 1 && niveau <= 4 && img_fonds_niveaux[niveau - 1] != NULL) {
        stretch_blit(img_fonds_niveaux[niveau - 1], buffer, 0, 0, img_fonds_niveaux[niveau - 1]->w, img_fonds_niveaux[niveau - 1]->h, 0, 0, SCREEN_W, SCREEN_H);
    } else {
        clear_to_color(buffer, makecol(135, 206, 235)); // Bleu ciel par défaut
    }
}

// Dessine les lasers verticaux du boss ou du décor
void dessiner_les_rayons(RayonDanger* r) {
    for (int i = 0; i < MAX_RAYONS; i++) {
        if (r[i].actif) {
            // Phase d'avertissement (Rayon rose hachuré)
            if (r[i].timer_avertissement > 0) {
                rectfill(buffer, r[i].x, 0, r[i].x + r[i].w, SCREEN_H, makecol(255, 200, 200));
                for(int y = -r[i].w; y < SCREEN_H; y += 30) line(buffer, r[i].x, y, r[i].x + r[i].w, y + r[i].w, makecol(255, 100, 100));
            }
            // Phase active mortelle (Rayon rouge avec bordure jaune)
            else if (r[i].timer_actif > 0) {
                rectfill(buffer, r[i].x, 0, r[i].x + r[i].w, SCREEN_H, makecol(255, 0, 0));
                rect(buffer, r[i].x, 0, r[i].x + r[i].w, SCREEN_H, makecol(255, 255, 0));
            }
        }
    }
}

// Dessine les boîtes tombantes (Armes)
void dessiner_les_bonus(Bonus* bonus) {
    for (int i = 0; i < MAX_BONUS; i++) {
        if (bonus[i].actif) {
            int col_bonus, col_texte; char lettre;

            // Code couleur par arme
            if (bonus[i].type == ARME_RAFALE) { col_bonus = makecol(255, 100, 100); col_texte = makecol(255, 255, 255); lettre = 'R'; }
            else if (bonus[i].type == ARME_EVENTAIL) { col_bonus = makecol(150, 50, 255); col_texte = makecol(255, 255, 255); lettre = 'V'; }
            else { col_bonus = makecol(255, 255, 0); col_texte = makecol(0, 0, 0); lettre = 'J'; } // Jaune Explosif

            rectfill(buffer, bonus[i].x, bonus[i].y, bonus[i].x + bonus[i].w, bonus[i].y + bonus[i].h, col_bonus);
            rect(buffer, bonus[i].x, bonus[i].y, bonus[i].x + bonus[i].w, bonus[i].y + bonus[i].h, makecol(255, 255, 255));
            textprintf_centre_ex(buffer, font, bonus[i].x + 10, bonus[i].y + 6, col_texte, -1, "%c", lettre);
        }
    }
}

// Dessine le boss final (Niveau 4) et sa barre de vie
void dessiner_le_boss(Boss* boss) {
    if (boss->actif) {
        // Sprite boss
        if (img_boss != NULL) stretch_sprite(buffer, img_boss, boss->x, boss->y, boss->w, boss->h);
        else rectfill(buffer, boss->x, boss->y, boss->x + boss->w, boss->y + boss->h, makecol(50, 0, 0));

        // Barre de santé
        int barre_w = 400; int hp_w = (boss->pv_actuel * barre_w) / boss->pv_max; if (hp_w < 0) hp_w = 0;
        int bx = (SCREEN_W - barre_w) / 2;

        rectfill(buffer, bx, 10, bx + barre_w, 25, makecol(100, 0, 0)); // Fond rouge foncé
        rectfill(buffer, bx, 10, bx + hp_w, 25, makecol(255, 0, 0));    // Jauge de vie rouge
        rect(buffer, bx, 10, bx + barre_w, 25, makecol(255, 255, 255)); // Contour blanc

        if (boss->phase == 3) textout_centre_ex(buffer, font, "!!! ENRAGE !!!", SCREEN_W/2, 14, makecol(255, 255, 0), -1);
        else textout_centre_ex(buffer, font, "LE ROI DES BULLES", SCREEN_W/2, 14, makecol(255, 255, 255), -1);
    }
}

// Dessine les avatars des joueurs (P1 et P2)
void dessiner_les_joueurs(Joueur* jrs) {
    for (int p = 0; p < 2; p++) {
        if (jrs[p].actif && !jrs[p].est_mort) {
            // Clignotement si invulnérable (modulo 10 < 5)
            if (jrs[p].timer_invincibilite == 0 || (jrs[p].timer_invincibilite % 10 < 5)) {
                if (img_player != NULL) stretch_sprite(buffer, img_player, jrs[p].x, jrs[p].y, jrs[p].w, jrs[p].h);
                else {
                    int col = (p == 0) ? makecol(0, 0, 200) : makecol(200, 0, 0); // Bleu ou Rouge
                    rectfill(buffer, jrs[p].x, jrs[p].y, jrs[p].x + jrs[p].w, jrs[p].y + jrs[p].h, col);
                }
                // Tag P1/P2 au-dessus de la tête
                textout_centre_ex(buffer, font, (p == 0) ? "P1" : "P2", jrs[p].x + jrs[p].w / 2, jrs[p].y - 15, makecol(255, 255, 255), -1);
            }
        }
    }
}

// Dessine tous les projectiles lancés par les joueurs
void dessiner_les_tirs(Tir* t) {
    for (int i = 0; i < MAX_TIRS; i++) {
        if (t[i].actif) {
            int col_tir; // Couleur en fonction de l'arme
            if (t[i].type_arme == ARME_RAFALE) col_tir = makecol(255, 100, 100);
            else if (t[i].type_arme == ARME_EVENTAIL) col_tir = makecol(200, 100, 255);
            else if (t[i].type_arme == ARME_EXPLOSIF) col_tir = makecol(255, 0, 0);
            else col_tir = makecol(255, 255, 0); // Base = Jaune

            // Le tir explosif est rond, les autres sont des rectangles (lasers)
            if (t[i].type_arme == ARME_EXPLOSIF) circlefill(buffer, t[i].x + t[i].w/2, t[i].y + t[i].h/2, t[i].w/2, col_tir);
            else rectfill(buffer, t[i].x, t[i].y, t[i].x + t[i].w, t[i].y + t[i].h, col_tir);
        }
    }
}

// Dessine les ennemis rebondissants
void dessiner_les_ballons(Ballon* b) {
    for (int i = 0; i < MAX_BALLONS; i++) {
        if (b[i].actif) {
            BITMAP* sprite_bulle = NULL;
            int col_secours = makecol(200, 30, 30); // Couleur si pas d'image

            // Type de bulle = image correspondante
            if (b[i].type == BULLE_CLASSIQUE) { sprite_bulle = img_bulle_rouge; col_secours = makecol(200, 30, 30); }
            else if (b[i].type == BULLE_MULTI) { sprite_bulle = img_bulle_violette; col_secours = makecol(150, 30, 200); }
            else if (b[i].type == BULLE_EXPLOSIVE) { sprite_bulle = img_bulle_jaune; col_secours = makecol(200, 200, 30); }

            if (sprite_bulle != NULL) stretch_sprite(buffer, sprite_bulle, b[i].x - b[i].rayon, b[i].y - b[i].rayon, b[i].rayon * 2, b[i].rayon * 2);
            else { // Mode secours sans images
                circlefill(buffer, b[i].x, b[i].y, b[i].rayon, col_secours);
                circle(buffer, b[i].x, b[i].y, b[i].rayon, makecol(0,0,0));
            }
        }
    }
}

// Dessine l'animation de particules/explosion
void dessiner_les_explosions(Explosion* e) {
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (e[i].actif && img_explosion[e[i].frame_actuelle] != NULL) {
            stretch_sprite(buffer, img_explosion[e[i].frame_actuelle], e[i].x - (e[i].taille / 2), e[i].y - (e[i].taille / 2), e[i].taille, e[i].taille);
        }
    }
}

// Dessine les informations textuelles (Score, temps, arme) par-dessus le jeu
void dessiner_le_hud(int niveau, Joueur* jrs) {
    // Infos communes en haut
    textprintf_ex(buffer, font, 20, 20, makecol(255, 255, 255), -1, "SCORE TOTAL : %d", get_score_global_partie() + get_score_actuel());
    textprintf_centre_ex(buffer, font, SCREEN_W / 2, 20, makecol(255, 255, 0), -1, "TEMPS : %02d", get_timer_niveau() / FRAMES_PAR_SECONDE);

    // Infos spécifiques à chaque joueur en bas
    for(int p=0; p < get_nombre_joueurs_mode(); p++) {
        if (jrs[p].actif && !jrs[p].est_mort) {
            int x_hud = (p == 0) ? 20 : SCREEN_W - 150; // P1 à gauche, P2 à droite

            // Nom de l'arme et couleur
            char* nom_arme = "HARPON"; int col_arme = makecol(255, 255, 255);
            if (jrs[p].arme_actuelle == ARME_RAFALE) { nom_arme = "RAFALE"; col_arme = makecol(255, 100, 100); }
            else if (jrs[p].arme_actuelle == ARME_EVENTAIL) { nom_arme = "EVENTAIL"; col_arme = makecol(200, 100, 255); }
            else if (jrs[p].arme_actuelle == ARME_EXPLOSIF) { nom_arme = "EXPLOSIF"; col_arme = makecol(255, 255, 0); }

            textprintf_ex(buffer, font, x_hud, SCREEN_H - 45, col_arme, -1, "ARME P%d: %s", p+1, nom_arme);

            // Jauge de temps restante pour l'arme spéciale
            if (jrs[p].arme_actuelle != ARME_BASE) {
                int w_barre = (jrs[p].timer_arme * 100) / JOUEUR_TIMER_ARME_SPECIALE;
                rectfill(buffer, x_hud, SCREEN_H - 30, x_hud + 100, SCREEN_H - 25, makecol(50, 50, 50));
                rectfill(buffer, x_hud, SCREEN_H - 30, x_hud + w_barre, SCREEN_H - 25, col_arme);
            }

            // Vies uniquement affichées contre le boss final
            if (niveau == 4) textprintf_ex(buffer, font, x_hud, SCREEN_H - 65, makecol(255,255,255), -1, "VIES: %d", jrs[p].vies);
        }
    }

    // Affichage des bulles restantes (sauf niveau 4)
    if (niveau != 4) textprintf_centre_ex(buffer, font, SCREEN_W / 2, 40, makecol(255, 255, 0), -1, "OBJECTIF : %d / %d", get_objectif_actuel(), get_objectif_max());

    // Compte à rebours de début de partie (3, 2, 1...)
    int timer = get_timer_debut();
    if (timer > 0) textprintf_centre_ex(buffer, font, SCREEN_W/2, SCREEN_H/2, makecol(255, 0, 0), makecol(0,0,0), " - %d - ", (timer / 40) + 1);
}

// ==========================================
// AFFICHAGE DU JEU ET DES OVERLAYS
// ==========================================

// Fonction principale de dessin appelée à chaque frame (60 FPS)
void dessiner_jeu_en_cours() {
    int niveau = get_niveau_en_cours();

    // Ordre de dessin (du plus profond au plus devant)
    dessiner_fond_jeu(niveau);
    dessiner_les_rayons(get_rayons());
    dessiner_les_bonus(get_bonus());
    dessiner_le_boss(get_boss());
    dessiner_les_joueurs(get_joueurs());
    dessiner_les_tirs(get_tirs());
    dessiner_les_ballons(get_ballons());
    dessiner_les_explosions(get_explosions());
    dessiner_le_hud(niveau, get_joueurs());
}

// Overlay Game Over avec fond assombri
void dessiner_ecran_defaite() {
    dessiner_jeu_en_cours(); // On garde le jeu figé en fond
    set_trans_blender(0, 0, 0, 200);
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
    rectfill(buffer, 0, 0, SCREEN_W, SCREEN_H, makecol(0,0,0)); // Voile noir
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

    int cx = SCREEN_W / 2; int cy = SCREEN_H / 2;
    textout_centre_ex(buffer, font, "GAME OVER", cx, cy - 60, makecol(255, 0, 0), -1);

    // Bouton REJOUER
    rectfill(buffer, cx - 100, cy - 10, cx + 100, cy + 20, makecol(50, 150, 50));
    rect(buffer, cx - 100, cy - 10, cx + 100, cy + 20, makecol(255, 255, 255));
    textout_centre_ex(buffer, font, "REJOUER LE NIVEAU", cx, cy + 2, makecol(255, 255, 255), -1);

    // Bouton CARTE
    rectfill(buffer, cx - 100, cy + 40, cx + 100, cy + 70, makecol(150, 50, 50));
    rect(buffer, cx - 100, cy + 40, cx + 100, cy + 70, makecol(255, 255, 255));
    textout_centre_ex(buffer, font, "RETOUR A LA CARTE", cx, cy + 52, makecol(255, 255, 255), -1);
}

// Menu de pause en plein jeu
void dessiner_pause() {
    dessiner_jeu_en_cours();
    set_trans_blender(0, 0, 0, 150);
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
    rectfill(buffer, 0, 0, SCREEN_W, SCREEN_H, makecol(0,0,0));
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

    int cx = SCREEN_W / 2; int cy = SCREEN_H / 2;
    textout_centre_ex(buffer, font, "PAUSE", cx, cy - 80, makecol(255, 255, 0), -1);

    // Bouton REPRENDRE
    rectfill(buffer, cx - 100, cy - 30, cx + 100, cy, makecol(50, 150, 50));
    rect(buffer, cx - 100, cy - 30, cx + 100, cy, makecol(255, 255, 255));
    textout_centre_ex(buffer, font, "REPRENDRE", cx, cy - 18, makecol(255, 255, 255), -1);

    // Bouton RECOMMENCER
    rectfill(buffer, cx - 100, cy + 20, cx + 100, cy + 50, makecol(200, 100, 0));
    rect(buffer, cx - 100, cy + 20, cx + 100, cy + 50, makecol(255, 255, 255));
    textout_centre_ex(buffer, font, "RECOMMENCER", cx, cy + 32, makecol(255, 255, 255), -1);

    // Bouton QUITTER
    rectfill(buffer, cx - 100, cy + 70, cx + 100, cy + 100, makecol(150, 50, 50));
    rect(buffer, cx - 100, cy + 70, cx + 100, cy + 100, makecol(255, 255, 255));
    textout_centre_ex(buffer, font, "QUITTER (CARTE)", cx, cy + 82, makecol(255, 255, 255), -1);
}

// Overlay Victoire en fin de niveau
void dessiner_ecran_victoire() {
    dessiner_jeu_en_cours();
    set_trans_blender(0, 0, 0, 200);
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
    rectfill(buffer, 0, 0, SCREEN_W, SCREEN_H, makecol(0,0,0));
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

    int cx = SCREEN_W / 2; int cy = SCREEN_H / 2;
    textout_centre_ex(buffer, font, "NIVEAU TERMINE !", cx, cy - 60, makecol(0, 255, 0), -1);
    textprintf_centre_ex(buffer, font, cx, cy - 30, makecol(255, 255, 255), -1, "Score de la manche : %d", get_score_actuel());

    // Bouton CONTINUER
    rectfill(buffer, cx - 100, cy + 10, cx + 100, cy + 40, makecol(50, 150, 50));
    rect(buffer, cx - 100, cy + 10, cx + 100, cy + 40, makecol(255, 255, 255));
    textout_centre_ex(buffer, font, "CONTINUER", cx, cy + 22, makecol(255, 255, 255), -1);
}
