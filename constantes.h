#ifndef CONSTANTES_H
#define CONSTANTES_H

// ==========================================
// ETAT DU JEU (L'Aiguilleur)
// ==========================================
typedef enum {
    ETAT_MENU_PRINCIPAL,
    ETAT_CARTE_MONDE,
    ETAT_CHARGER_PARTIE,
    ETAT_REGLES,
    ETAT_OPTIONS,
    ETAT_JEU_EN_COURS,
    ETAT_PAUSE,
    ETAT_ECRAN_DEFAITE,
    ETAT_ECRAN_VICTOIRE,
    ETAT_QUITTER
} EtatJeu;

// ==========================================
// LIMITES MAXIMALES (Mémoire)
// ==========================================
#define MAX_BALLONS 50
#define MAX_TIRS 40
#define MAX_RAYONS 10
#define MAX_EXPLOSIONS 20
#define MAX_BONUS 5

// ==========================================
// JOUEUR
// ==========================================
#define JOUEUR_LARGEUR 40
#define JOUEUR_HAUTEUR 52
#define JOUEUR_VITESSE_BASE 12.0
#define JOUEUR_TIMER_INVINCIBILITE 120
#define JOUEUR_TIMER_ARME_SPECIALE 600

// ==========================================
// ARMES ET TIRS (Equilibrage)
// ==========================================
#define ARME_BASE_MAX_ECRAN 3
#define ARME_BASE_COOLDOWN 15
#define TIR_BASE_W 6
#define TIR_BASE_H 20
#define TIR_BASE_VY -25.0
#define TIR_BASE_DEGATS 10

#define ARME_RAFALE_MAX_ECRAN 20
#define ARME_RAFALE_COOLDOWN 3
#define TIR_RAFALE_W 4
#define TIR_RAFALE_H 10
#define TIR_RAFALE_VY -35.0

#define ARME_EVENTAIL_MAX_ECRAN 12
#define ARME_EVENTAIL_COOLDOWN 12

#define ARME_EXPLOSIF_MAX_ECRAN 3
#define ARME_EXPLOSIF_COOLDOWN 15
#define TIR_EXPLOSIF_W 12
#define TIR_EXPLOSIF_H 12
#define TIR_EXPLOSIF_VY -15.0
#define TIR_EXPLOSIF_DEGATS 30
#define TIR_EXPLOSIF_EXPLO_TAILLE 150
#define TIR_EXPLOSIF_RAYON_SOUFFLE 120

// ==========================================
// BULLES ET BONUS
// ==========================================
#define BULLE_RAYON_MINIMUM 16
#define POINTS_PAR_BULLE 150
#define BONUS_CHANCE_DROP 40
#define BONUS_LARGEUR 20
#define BONUS_HAUTEUR 20
#define BONUS_VITESSE_Y 3.0
#define POINTS_BONUS_RAMASSE 50

// ==========================================
// BOSS
// ==========================================
#define BOSS_LARGEUR 400
#define BOSS_HAUTEUR 190
#define BOSS_PV_MAX 500
#define POINTS_VICTOIRE_BOSS 5000

// ==========================================
// CHRONOMETRE NIVEAU
// ==========================================
#define TEMPS_NIVEAU_SECONDES 90
#define FRAMES_PAR_SECONDE 60
#define TIMER_DEBUT_NIVEAU 120
#define POINTS_PAR_SECONDE_RESTANTE 10

// ==========================================
// INTERFACE MENU PRINCIPAL
// ==========================================
#define BOUTON_MENU_LARGEUR 240
#define BOUTON_MENU_HAUTEUR 35
#define BOUTON_MENU_START_Y 270
#define BOUTON_MENU_ESPACE 45

#endif
