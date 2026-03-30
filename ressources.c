#include "ressources.h"

// Définitions Menu
BITMAP* img_fond_menu = NULL;
BITMAP* img_logo = NULL;
BITMAP* img_sprite_sheet = NULL;
BITMAP* img_curseur = NULL;

// Définitions Carte
BITMAP* img_fond_map = NULL;
BITMAP* img_drapeau_rouge = NULL;
BITMAP* img_drapeau_jaune = NULL;
BITMAP* img_panneau_info = NULL;

// Audio
SAMPLE* son_beep = NULL;
SAMPLE* mus_menu = NULL;

// Fonction utilitaire pour charger en prévenant si erreur
BITMAP* charger_bmp(const char* chemin) {
    BITMAP* bmp = load_bitmap(chemin, NULL);
    if (!bmp) {
        allegro_message("Erreur critique : Impossible de charger l'image :\n%s\nVérifiez qu'elle est bien dans le dossier /asset/", chemin);
        exit(EXIT_FAILURE);
    }
    return bmp;
}

void initialiser_ressources() {
    // --- Chargement Menu (Chemins modifiés !) ---
    img_fond_menu = charger_bmp("asset/Background.bmp");
    img_logo      = charger_bmp("asset/Logo.bmp");
    img_sprite_sheet = charger_bmp("asset/Player.bmp");

    if (img_sprite_sheet != NULL) {
        img_curseur = create_sub_bitmap(img_sprite_sheet, 0, 0, 32, 32);
    }

    // --- NOUVEAU : Chargement Carte (Avec dossier asset/) ---
    img_fond_map      = charger_bmp("asset/mapBackground.bmp");
    img_drapeau_rouge = charger_bmp("asset/REDflagbmp.bmp");
    img_drapeau_jaune = charger_bmp("asset/YELLOWflagbmp.bmp");
    img_panneau_info  = charger_bmp("asset/info.bmp");

    // --- Chargement Sons (Chemins modifiés !) ---
    son_beep = load_sample("asset/beep.wav");
    mus_menu = load_sample("asset/musique_menu.wav");

    if (!son_beep) allegro_message("Alerte: asset/beep.wav introuvable !");
    if (!mus_menu) allegro_message("Alerte: asset/musique_menu.wav introuvable !");
}

void detruire_ressources() {
    // Audio
    if (son_beep) destroy_sample(son_beep);
    if (mus_menu) destroy_sample(mus_menu);

    // Images Carte
    if (img_panneau_info) destroy_bitmap(img_panneau_info);
    if (img_drapeau_jaune) destroy_bitmap(img_drapeau_jaune);
    if (img_drapeau_rouge) destroy_bitmap(img_drapeau_rouge);
    if (img_fond_map) destroy_bitmap(img_fond_map);

    // Images Menu
    if (img_curseur) destroy_bitmap(img_curseur);
    if (img_sprite_sheet) destroy_bitmap(img_sprite_sheet);
    if (img_logo) destroy_bitmap(img_logo);
    if (img_fond_menu) destroy_bitmap(img_fond_menu);
}
