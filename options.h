#ifndef OPTIONS_H
#define OPTIONS_H

extern int flag_musique_changee;
extern int flag_resolution_changee;

// ==========================================
// FONCTIONS DE GESTION DU SON
// ==========================================
int get_vol_musique();           // Récupère le volume actuel de la musique
void set_vol_musique(int v);     // Modifie le volume de la musique

int get_vol_sfx();               // Récupère le volume des bruitages (tirs, clics)
void set_vol_sfx(int v);         // Modifie le volume des bruitages

// ==========================================
// FONCTIONS DE GESTION DE L'ÉCRAN
// ==========================================
int get_res_w();                 // Renvoie la largeur de l'écran
int get_res_h();                 // Renvoie la hauteur de l'écran

int is_plein_ecran();            // Indique si le jeu est en plein écran (1) ou fenêtré (0)
void toggle_plein_ecran();       // Bascule d'un mode d'affichage à l'autre

#endif
