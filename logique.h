#ifndef LOGIQUE_H
#define LOGIQUE_H

#include "constantes.h"

#define MAX_BALLONS 50 // Augmenté pour supporter les divisions
#define MAX_TIRS 5
#define MAX_RAYONS 3   // NOUVEAU : Nombre max de rayons mortels simultanés

// NOUVEAU : Enumération pour différencier les bulles
typedef enum {
    BULLE_CLASSIQUE, // Rouge : se divise en 2
    BULLE_MULTI,     // Violette : se divise en 4
    BULLE_EXPLOSIVE  // Grise : éclate en 6 très petites bulles
} TypeBulle;

typedef struct { float x, y; int w, h; float vitesse; } Joueur;
typedef struct { float x, y; int w, h; float vitesse; int actif; } Tir;

typedef struct {
    float x, y;
    int rayon;
    float vx, vy;
    int actif;
    TypeBulle type; // NOUVEAU
} Ballon;

// NOUVEAU : Structure pour les rayons (lasers)
typedef struct {
    int x, w;                // Position X et largeur du rayon (couvre tout l'écran en hauteur)
    int timer_avertissement; // Temps restant de l'aura rouge (inoffensif)
    int timer_actif;         // Temps restant du rayon mortel
    int actif;
} RayonDanger;

void initialiser_niveau(int numero_niveau, int ecran_w, int ecran_h);
EtatJeu mettre_a_jour_jeu(EtatJeu etat_courant, int ecran_w, int ecran_h);

Joueur* get_joueur();
Tir* get_tirs();
Ballon* get_ballons();
RayonDanger* get_rayons(); // NOUVEAU

int get_score_actuel();
int get_timer_debut();
int get_objectif_actuel();
int get_objectif_max();
int get_niveau_en_cours();

#endif
