#ifndef CONSTANTES_H
#define CONSTANTES_H

typedef enum {
    ETAT_MENU_PRINCIPAL,
    ETAT_REGLES,
    ETAT_CARTE_MONDE,
    ETAT_CHARGER_PARTIE,
    ETAT_OPTIONS,
    ETAT_JEU_EN_COURS,
    ETAT_PAUSE,           // NOUVEAU : Menu Pause
    ETAT_ECRAN_VICTOIRE,  // NOUVEAU : Ecran de fin de niveau
    ETAT_ECRAN_DEFAITE,
    ETAT_QUITTER
} EtatJeu;

typedef enum {
    MODE_CARTE_PARCOURIR,
    MODE_CARTE_INFO
} ModeCarte;

#endif
