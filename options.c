#include "options.h"

static int vol_musique = 150;
static int vol_sfx = 255;
static int res_w = 800;
static int res_h = 600;

int flag_resolution_changee = 0;
int flag_musique_changee = 0;

int get_vol_musique() { return vol_musique; }

// NOUVEAU : Application directe de la valeur avec sécurité
void set_vol_musique(int val) {
    vol_musique = val;
    if(vol_musique < 0) vol_musique = 0;
    if(vol_musique > 255) vol_musique = 255;
    flag_musique_changee = 1; // On prévient le main qu'il faut ajuster le son
}

int get_vol_sfx() { return vol_sfx; }

void set_vol_sfx(int val) {
    vol_sfx = val;
    if(vol_sfx < 0) vol_sfx = 0;
    if(vol_sfx > 255) vol_sfx = 255;
}

void toggle_resolution() {
    if (res_w == 800) {
        res_w = 1024;
        res_h = 768;
    } else {
        res_w = 800;
        res_h = 600;
    }
    flag_resolution_changee = 1;
}

int get_res_w() { return res_w; }
int get_res_h() { return res_h; }
