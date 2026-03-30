#ifndef OPTIONS_H
#define OPTIONS_H

int get_vol_musique();
void set_vol_musique(int val); // NOUVEAU

int get_vol_sfx();
void set_vol_sfx(int val); // NOUVEAU

void toggle_resolution();
int get_res_w();
int get_res_h();

extern int flag_resolution_changee;
extern int flag_musique_changee;

#endif
