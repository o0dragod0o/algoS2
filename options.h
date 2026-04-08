#ifndef OPTIONS_H
#define OPTIONS_H

extern int flag_musique_changee;
extern int flag_resolution_changee;

int get_vol_musique();
void set_vol_musique(int v);

int get_vol_sfx();
void set_vol_sfx(int v);

int get_res_w();
int get_res_h();

// NOUVEAU : Plein écran
int is_plein_ecran();
void toggle_plein_ecran();

#endif
