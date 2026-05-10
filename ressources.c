#include "ressources.h"
#include <stdio.h>

// ==========================================
// DÉCLARATION DES VARIABLES
// ==========================================

// Images des menus
BITMAP* img_fond_menu = NULL;
BITMAP* img_logo = NULL;
BITMAP* img_sprite_sheet = NULL;
BITMAP* img_curseur = NULL;
BITMAP* img_fond_map = NULL;
BITMAP* img_drapeau_rouge = NULL;
BITMAP* img_drapeau_jaune = NULL;
BITMAP* img_panneau_info = NULL;

// Images In-Game
BITMAP* img_fonds_niveaux[4] = {NULL, NULL, NULL, NULL};
BITMAP* img_bulle_rouge = NULL;
BITMAP* img_bulle_violette = NULL;
BITMAP* img_bulle_jaune = NULL;
BITMAP* img_player = NULL;
BITMAP* img_explosion[8] = {NULL};
BITMAP* img_boss = NULL;

// Audio
SAMPLE* son_beep = NULL;
SAMPLE* mus_menu = NULL;

// ==========================================
// CHARGEMENT DES RESSOURCES
// ==========================================

void initialiser_ressources() {
    // --- Chargement de l'Interface ---
    img_fond_menu = load_bitmap("asset/Background.bmp", NULL);
    img_logo = load_bitmap("asset/Logo.bmp", NULL);
    img_curseur = load_bitmap("asset/navigation_e.bmp", NULL);
    img_fond_map = load_bitmap("asset/mapBackground.bmp", NULL);
    img_drapeau_rouge = load_bitmap("asset/REDflagbmp.bmp", NULL);
    img_drapeau_jaune = load_bitmap("asset/YELLOWflagbmp.bmp", NULL);
    img_panneau_info = load_bitmap("asset/info.bmp", NULL);

    // --- Chargement des Fonds de Niveaux ---
    img_fonds_niveaux[0] = load_bitmap("asset/fond_paris.bmp", NULL);
    img_fonds_niveaux[1] = load_bitmap("asset/fond_londres.bmp", NULL);
    img_fonds_niveaux[2] = load_bitmap("asset/fond_tokyo.bmp", NULL);
    img_fonds_niveaux[3] = load_bitmap("asset/fond_newyork.bmp", NULL);

    // --- Chargement des Entités ---
    img_bulle_rouge = load_bitmap("asset/bulle_rouge.bmp", NULL);
    img_bulle_violette = load_bitmap("asset/bulle_violet.bmp", NULL);
    img_bulle_jaune = load_bitmap("asset/bulle_jaune.bmp", NULL);
    img_player = load_bitmap("asset/player.bmp", NULL);
    img_boss = load_bitmap("asset/boss.bmp", NULL);

    // --- Chargement de l'Animation d'Explosion (Boucle) ---
    char chemin[50];
    for(int i = 0; i < 8; i++) {
        // Crée dynamiquement le nom du fichier : "explosion-f1.bmp", "explosion-f2.bmp", etc.
        sprintf(chemin, "asset/explosion-f%d.bmp", i+1);
        img_explosion[i] = load_bitmap(chemin, NULL);
    }

    // --- Chargement de l'Audio ---
    son_beep = load_sample("asset/beep.wav");
    mus_menu = load_sample("asset/musique_menu.wav");
}

// ==========================================
// LIBÉRATION DE LA MÉMOIRE
// ==========================================

void detruire_ressources() {
    // --- Nettoyage de l'Interface ---
    if (img_fond_menu) destroy_bitmap(img_fond_menu);
    if (img_logo) destroy_bitmap(img_logo);
    if (img_sprite_sheet) destroy_bitmap(img_sprite_sheet);
    if (img_curseur) destroy_bitmap(img_curseur);
    if (img_fond_map) destroy_bitmap(img_fond_map);
    if (img_drapeau_rouge) destroy_bitmap(img_drapeau_rouge);
    if (img_drapeau_jaune) destroy_bitmap(img_drapeau_jaune);
    if (img_panneau_info) destroy_bitmap(img_panneau_info);

    // --- Nettoyage des Fonds ---
    for(int i = 0; i < 4; i++) {
        if (img_fonds_niveaux[i]) destroy_bitmap(img_fonds_niveaux[i]);
    }

    // --- Nettoyage des Entités ---
    if (img_bulle_rouge) destroy_bitmap(img_bulle_rouge);
    if (img_bulle_violette) destroy_bitmap(img_bulle_violette);
    if (img_bulle_jaune) destroy_bitmap(img_bulle_jaune);
    if (img_player) destroy_bitmap(img_player);
    if (img_boss) destroy_bitmap(img_boss);

    // --- Nettoyage de l'Animation ---
    for(int i = 0; i < 8; i++) {
        if (img_explosion[i]) destroy_bitmap(img_explosion[i]);
    }

    // --- Nettoyage de l'Audio ---
    if (son_beep) destroy_sample(son_beep);
    if (mus_menu) destroy_sample(mus_menu);
}
