#ifndef RESSOURCES_H
#define RESSOURCES_H

#include <allegro.h>

// Images Menu
extern BITMAP* img_fond_menu;
extern BITMAP* img_logo;
extern BITMAP* img_sprite_sheet;
extern BITMAP* img_curseur;

// Images Carte du Monde
extern BITMAP* img_fond_map;
extern BITMAP* img_drapeau_rouge;
extern BITMAP* img_drapeau_jaune;
extern BITMAP* img_panneau_info;

// Audio
extern SAMPLE* son_beep;
extern SAMPLE* mus_menu;

void initialiser_ressources();
void detruire_ressources();

#endif
