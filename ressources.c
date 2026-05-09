#include "ressources.h"
#include <stdio.h>

BITMAP* img_fond_menu = NULL;
BITMAP* img_logo = NULL;
BITMAP* img_sprite_sheet = NULL;
BITMAP* img_curseur = NULL;
BITMAP* img_fond_map = NULL;
BITMAP* img_drapeau_rouge = NULL;
BITMAP* img_drapeau_jaune = NULL;
BITMAP* img_panneau_info = NULL;

BITMAP* img_fonds_niveaux[4] = {NULL, NULL, NULL, NULL};

// Sprites de jeu
BITMAP* img_bulle_rouge = NULL;
BITMAP* img_bulle_violette = NULL;
BITMAP* img_bulle_jaune = NULL;
BITMAP* img_player = NULL;
BITMAP* img_explosion[8] = {NULL};
BITMAP* img_boss = NULL;

SAMPLE* son_beep = NULL;
SAMPLE* mus_menu = NULL;

void initialiser_ressources() {
    img_fond_menu = load_bitmap("asset/Background.bmp", NULL);
    img_logo = load_bitmap("asset/Logo.bmp", NULL);
    img_curseur = load_bitmap("asset/navigation_e.bmp", NULL);
    img_fond_map = load_bitmap("asset/mapBackground.bmp", NULL);
    img_drapeau_rouge = load_bitmap("asset/REDflagbmp.bmp", NULL);
    img_drapeau_jaune = load_bitmap("asset/YELLOWflagbmp.bmp", NULL);
    img_panneau_info = load_bitmap("asset/info.bmp", NULL);

    img_fonds_niveaux[0] = load_bitmap("asset/fond_paris.bmp", NULL);
    img_fonds_niveaux[1] = load_bitmap("asset/fond_londres.bmp", NULL);
    img_fonds_niveaux[2] = load_bitmap("asset/fond_tokyo.bmp", NULL);
    img_fonds_niveaux[3] = load_bitmap("asset/fond_newyork.bmp", NULL);

    // Chargement des nouveaux sprites
    img_bulle_rouge = load_bitmap("asset/bulle_rouge.bmp", NULL);
    img_bulle_violette = load_bitmap("asset/bulle_violet.bmp", NULL);
    img_bulle_jaune = load_bitmap("asset/bulle_jaune.bmp", NULL);
    img_player = load_bitmap("asset/player.bmp", NULL);

    img_boss = load_bitmap("asset/boss.bmp", NULL);

    char chemin[50];
    for(int i = 0; i < 8; i++) {
        sprintf(chemin, "asset/explosion-f%d.bmp", i+1);
        img_explosion[i] = load_bitmap(chemin, NULL);
    }

    son_beep = load_sample("asset/beep.wav");
    mus_menu = load_sample("asset/musique_menu.wav");
}

void detruire_ressources() {
    if (img_fond_menu) destroy_bitmap(img_fond_menu);
    if (img_logo) destroy_bitmap(img_logo);
    if (img_curseur) destroy_bitmap(img_curseur);
    if (img_fond_map) destroy_bitmap(img_fond_map);
    if (img_drapeau_rouge) destroy_bitmap(img_drapeau_rouge);
    if (img_drapeau_jaune) destroy_bitmap(img_drapeau_jaune);
    if (img_panneau_info) destroy_bitmap(img_panneau_info);

    for(int i = 0; i < 4; i++) if (img_fonds_niveaux[i]) destroy_bitmap(img_fonds_niveaux[i]);

    // Destruction des nouveaux sprites
    if (img_bulle_rouge) destroy_bitmap(img_bulle_rouge);
    if (img_bulle_violette) destroy_bitmap(img_bulle_violette);
    if (img_bulle_jaune) destroy_bitmap(img_bulle_jaune);
    if (img_player) destroy_bitmap(img_player);

    for(int i = 0; i < 8; i++) if (img_explosion[i]) destroy_bitmap(img_explosion[i]);
    if (img_boss) destroy_bitmap(img_boss);

    if (son_beep) destroy_sample(son_beep);
    if (mus_menu) destroy_sample(mus_menu);
}
