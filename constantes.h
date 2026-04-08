#ifndef CONSTANTES_H
#define CONSTANTES_H

typedef enum {
    ETAT_MENU_PRINCIPAL,
    ETAT_REGLES,
    ETAT_CARTE_MONDE,
    ETAT_CHARGER_PARTIE, // NOUVEAU : Remplace ETAT_SAUVEGARDE
    ETAT_OPTIONS,
    ETAT_JEU_EN_COURS,
    ETAT_ECRAN_DEFAITE,
    ETAT_QUITTER
} EtatJeu;

typedef enum {
    MODE_CARTE_PARCOURIR,
    MODE_CARTE_INFO
} ModeCarte;

#endif
