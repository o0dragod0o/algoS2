#ifndef CONSTANTES_H
#define CONSTANTES_H

// Définition des différents états du jeu (Logique métier)
typedef enum {
    ETAT_MENU_PRINCIPAL,
    ETAT_REGLES,
    ETAT_CARTE_MONDE,
    ETAT_SAUVEGARDE,
    ETAT_OPTIONS,      // NOUVEAU
    ETAT_JEU_EN_COURS,
    ETAT_QUITTER
} EtatJeu;

// Sous-états spécifiques à la carte du monde
typedef enum {
    MODE_CARTE_PARCOURIR, // Le joueur bouge la souris sur la carte
    MODE_CARTE_INFO       // Un panneau d'info ville est ouvert
} ModeCarte;

#endif
