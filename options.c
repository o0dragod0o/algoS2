#include "options.h"

// ==========================================
// VARIABLES GLOBALES
// ==========================================

// Signaux d'alerte pour dire au moteur qu'un réglage vient d'être modifié
int flag_musique_changee = 0;
int flag_resolution_changee = 0;

// Valeurs par défaut au lancement du jeu (128 = 50% du volume max)
int volume_musique = 128;
int volume_sfx = 128;
int plein_ecran = 0; // 0 = Mode Fenêtre, 1 = Mode Plein écran

// ==========================================
// GESTION DU VOLUME DE LA MUSIQUE
// ==========================================

int get_vol_musique() {
    return volume_musique;
}

void set_vol_musique(int v) {
    volume_musique = v;

    // Sécurité : Bloque la valeur entre 0 (Muet) et 255 (Maximum d'Allegro)
    if(volume_musique < 0) volume_musique = 0;
    if(volume_musique > 255) volume_musique = 255;

    // Lève le drapeau pour que main.c actualise le son de fond
    flag_musique_changee = 1;
}

// ==========================================
// GESTION DU VOLUME DES BRUITAGES
// ==========================================

int get_vol_sfx() {
    return volume_sfx;
}

void set_vol_sfx(int v) {
    volume_sfx = v;

    // Sécurité de la valeur (0 à 255)
    if(volume_sfx < 0) volume_sfx = 0;
    if(volume_sfx > 255) volume_sfx = 255;
}

// ==========================================
// GESTION DE LA RÉSOLUTION ET DE L'AFFICHAGE
// ==========================================

// On retourne la taille physique brute pour ne pas perturber Allegro
int get_res_w() { return 800; }
int get_res_h() { return 600; }

int is_plein_ecran() {
    return plein_ecran;
}

void toggle_plein_ecran() {
    plein_ecran = !plein_ecran; // Inverse la valeur (0 devient 1, 1 devient 0)

    // Lève le drapeau pour que main.c détruise et recrée la fenêtre
    flag_resolution_changee = 1;
}
