#include "options.h"

int flag_musique_changee = 0;
int flag_resolution_changee = 0;

static int volume_musique = 128;
static int volume_sfx = 128;
static int plein_ecran = 0; // 0 = Fenêtré, 1 = Plein écran

int get_vol_musique() { return volume_musique; }
void set_vol_musique(int v) {
    volume_musique = v;
    if(volume_musique < 0) volume_musique = 0;
    if(volume_musique > 255) volume_musique = 255;
    flag_musique_changee = 1;
}

int get_vol_sfx() { return volume_sfx; }
void set_vol_sfx(int v) {
    volume_sfx = v;
    if(volume_sfx < 0) volume_sfx = 0;
    if(volume_sfx > 255) volume_sfx = 255;
}

int get_res_w() { return 800; } // Taille par défaut de la fenêtre
int get_res_h() { return 600; }

int is_plein_ecran() { return plein_ecran; }
void toggle_plein_ecran() {
    plein_ecran = !plein_ecran;
    flag_resolution_changee = 1;
}
