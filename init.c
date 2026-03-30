#include <allegro.h>
#include <stdio.h>
#include "init.h"

void initialiser_allegro() {
    allegro_init();
    install_keyboard();
    install_mouse();

    // NOUVEAU : Initialisation du module son d'Allegro
    if (install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL) != 0) {
        allegro_message("Erreur d'initialisation du son : %s", allegro_error);
    }

    set_color_depth(32);
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0) != 0) {
        allegro_message("Erreur d'initialisation graphique: %s", allegro_error);
        exit(EXIT_FAILURE);
    }
    show_mouse(screen);
}
