#ifndef RESSOURCES_H
#define RESSOURCES_H

#include <allegro.h>

// ==========================================
// VARIABLES GLOBALES
// ==========================================

// --- Images de l'interface (Menus et Carte) ---
extern BITMAP* img_fond_menu;
extern BITMAP* img_logo;
extern BITMAP* img_curseur;
extern BITMAP* img_fond_map;
extern BITMAP* img_drapeau_rouge;
extern BITMAP* img_drapeau_jaune;
extern BITMAP* img_panneau_info;
extern BITMAP* img_sprite_sheet;

// --- Images du jeu (Fonds, Entités) ---
extern BITMAP* img_fonds_niveaux[4];
extern BITMAP* img_bulle_rouge;
extern BITMAP* img_bulle_violette;
extern BITMAP* img_bulle_jaune;
extern BITMAP* img_player;
extern BITMAP* img_explosion[8];
extern BITMAP* img_boss;

// --- Sons et Musiques ---
extern SAMPLE* son_beep;
extern SAMPLE* mus_menu;

// ==========================================
// FONCTIONS PRINCIPALES
// ==========================================

// Charge les fichiers depuis le dossier "asset/" au démarrage
void initialiser_ressources();

// Libère la RAM et la mémoire vidéo (VRAM) à la fermeture
void detruire_ressources();

#endif
