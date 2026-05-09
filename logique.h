#ifndef LOGIQUE_H
#define LOGIQUE_H

#include "constantes.h"

#define MAX_BALLONS 50
#define MAX_TIRS 10 // 5 par joueur
#define MAX_RAYONS 10
#define MAX_EXPLOSIONS 20

typedef enum { BULLE_CLASSIQUE, BULLE_MULTI, BULLE_EXPLOSIVE } TypeBulle;

// Chaque joueur a ses propres vies et son état de vie
typedef struct {
    float x, y;
    int w, h;
    float vitesse;
    int timer_invincibilite;
    int actif;
    int vies;
    int est_mort;
} Joueur;

typedef struct { float x, y; int w, h; float vitesse; int actif; int id_joueur; } Tir;
typedef struct { float x, y; int rayon; float vx, vy; int actif; TypeBulle type; } Ballon;
typedef struct { int x, w; int timer_avertissement; int timer_actif; int actif; } RayonDanger;
typedef struct { float x, y; int taille; int frame_actuelle; int timer_frame; int actif; } Explosion;

typedef struct {
    float x, y; int w, h;
    int pv_max; int pv_actuel;
    int phase;
    float vx, vy;
    int timer_attaque;
    int actif;
} Boss;

void initialiser_niveau(int numero_niveau, int ecran_w, int ecran_h);
EtatJeu mettre_a_jour_jeu(EtatJeu etat_courant, int ecran_w, int ecran_h);

Joueur* get_joueurs();
Tir* get_tirs();
Ballon* get_ballons();
RayonDanger* get_rayons();
Explosion* get_explosions();
Boss* get_boss();

// Gestion du mode choisi au menu
void set_nombre_joueurs_mode(int n);
int get_nombre_joueurs_mode();

int get_score_actuel();
int get_timer_debut();
int get_objectif_actuel();
int get_objectif_max();
int get_niveau_en_cours();

#endif
