#include <allegro.h>
#include <stdio.h>
#include "init.h"

// ==========================================
// INITIALISATION D'ALLEGRO
// ==========================================

void initialiser_allegro() {
    // 1. Initialisation des périphériques
    allegro_init();
    install_keyboard();
    install_mouse();

    // 2. Initialisation du module audio
    if (install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL) != 0) {
        // Pop-up d'erreur Windows si pas de carte son détectée
        allegro_message("Erreur d'initialisation du son : %s", allegro_error);
    }

    // 3. Configuration de la fenêtre graphique
    set_color_depth(32); // Mode couleurs HD (32 bits)

    // On utilise les constantes au lieu de "800, 600" !
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0) != 0) {
        allegro_message("Erreur graphique : %s", allegro_error);
        exit(EXIT_FAILURE);
    }

    // Affiche le curseur de base au démarrage
    show_mouse(screen);
}
