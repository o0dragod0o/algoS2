#ifndef LOGIQUE_H
#define LOGIQUE_H

#include "constantes.h"

// ==========================================
// Types d'objets
// ==========================================
// Comportement des bulles lors de leur destruction
typedef enum {
    BULLE_CLASSIQUE,  // Se divise en 2 bulles plus petites
    BULLE_MULTI,      // Se divise en 4 bulles (diagonales)
    BULLE_EXPLOSIVE   // Éclate en 6 petits fragments chaotiques
} TypeBulle;

// Arsenal disponible pour les joueurs
typedef enum {
    ARME_BASE,        // Harpon classique (1 à 3 tirs max)
    ARME_RAFALE,      // Mitraillette ultra rapide (Rouge)
    ARME_EVENTAIL,    // Tir triple en diagonale (Violet)
    ARME_EXPLOSIF     // Dégâts de zone massifs (Jaune)
} TypeArme;

// ==========================================
// STRUCTURES DE DONNÉES
// ==========================================

// Le personnage contrôlé par le joueur
typedef struct {
    float x, y;              // Position sur l'écran
    int w, h;                // Dimensions de la hitbox
    float vitesse;           // Vitesse de déplacement horizontal
    int timer_invincibilite; // Compte à rebours d'invulnérabilité après un dégât
    int actif;               // 1 = Joueur en jeu, 0 = Non participant
    int vies;                // Nombre de points de vie restants
    int est_mort;            // 1 = Éliminé de la partie
    TypeArme arme_actuelle;  // Arme équipée en ce moment
    int timer_arme;          // Temps restant avant de perdre l'arme spéciale
} Joueur;

// Un projectile tiré par un joueur
typedef struct {
    float x, y;          // Position
    int w, h;            // Dimensions de la balle/laser
    float vx, vy;        // Vitesse de déplacement
    int actif;           // 1 = Tir en vol, 0 = Tir détruit
    int id_joueur;       // Permet de savoir qui a tiré (0 = P1, 1 = P2)
    TypeArme type_arme;  // Propriétés du tir (Base, Explosif, etc.)
} Tir;

// Une caisse d'arme qui tombe du ciel
typedef struct {
    float x, y;          // Position
    int w, h;            // Taille de la caisse
    float vy;            // Vitesse de chute (Gravité)
    int actif;           // 1 = En chute, 0 = Ramassé ou disparu
    TypeArme type;       // L'arme que contient cette caisse
} Bonus;

// Un ennemi rebondissant (Bulle)
typedef struct {
    float x, y;          // Position centrale de la bulle
    int rayon;           // Taille (Hitbox circulaire)
    float vx, vy;        // Vecteur de déplacement (rebonds)
    int actif;           // 1 = Bulle vivante, 0 = Éclatée
    TypeBulle type;      // Couleur/Comportement de la bulle
} Ballon;

// Laser mortel venant du plafond
typedef struct {
    int x, w;                // Position horizontale et largeur du laser
    int timer_avertissement; // Phase 1 : Zone rose inoffensive
    int timer_actif;         // Phase 2 : Laser rouge mortel
    int actif;               // 1 = Laser présent
} RayonDanger;

// Animation visuelle d'une explosion
typedef struct {
    float x, y;          // Centre de l'explosion
    int taille;          // Échelle d'affichage
    int frame_actuelle;  // Étape de l'animation (ex: 0 à 7)
    int timer_frame;     // Vitesse de défilement de l'animation
    int actif;           // 1 = Animation en cours
} Explosion;

// Le Boss final du niveau 4
typedef struct {
    float x, y;          // Position (coin supérieur gauche)
    int w, h;            // Dimensions de l'énorme hitbox
    int pv_max;          // Santé totale de départ
    int pv_actuel;       // Santé restante
    int phase;           // Phase de combat (1, 2 ou 3)
    float vx, vy;        // Vitesse de vol/rebond
    int timer_attaque;   // Délai avant d'invoquer des bulles ou lasers
    int actif;           // 1 = Combat en cours
} Boss;

// ==========================================
// FONCTIONS PRINCIPALES
// ==========================================
// Charge les données à zéro et prépare les variables au début d'un niveau
void initialiser_niveau(int numero_niveau, int ecran_w, int ecran_h);

// Cœur du jeu : Exécutée à chaque frame (60 FPS) pour faire avancer le temps
EtatJeu mettre_a_jour_jeu(EtatJeu etat_courant, int ecran_w, int ecran_h);

// ==========================================
// SOUS-FONCTIONS
// ==========================================
// -- Bulles et Armes --
void tenter_drop_bonus(Ballon b);                             // Chance de faire tomber une caisse
void generer_sous_bulles(Ballon b, int nouveau_rayon);        // Fragmente une bulle détruite
void declencher_tir_arme(int p, float px, float py, TypeArme arme); // Instancie un/des projectiles

// -- IA du Boss --
void actualiser_phase_boss();                                 // Change la phase selon les PV
void deplacer_boss(int ecran_w, int ecran_h);                 // Mouvements et rebonds du boss
void gerer_attaques_boss();                                   // Invocation des sbires/lasers
EtatJeu gerer_collision_boss_joueurs(EtatJeu etat_courant);   // Écrasement du joueur
void gerer_collision_boss_tirs();                             // Le boss prend des dégâts

// -- Environnement (Lasers) --
void generer_nouveaux_rayons(int ecran_w);                    // Spawne des lasers aléatoires
EtatJeu appliquer_degats_rayons(EtatJeu etat_courant);        // Blesse les joueurs touchés

// -- Physique des Ballons --
void deplacer_un_ballon(int i, int ecran_w, int ecran_h);     // Applique la gravité et les rebonds
int gerer_impact_tir_sur_ballon(int i);                       // Détecte si un tir touche la bulle
EtatJeu gerer_impact_joueur_sur_ballon(EtatJeu etat_courant, int i); // Détecte si la bulle blesse un joueur

// Envoient l'adresse des tableaux pour que affichage.c puisse les dessiner
Joueur* get_joueurs();
Tir* get_tirs();
Bonus* get_bonus();
Ballon* get_ballons();
RayonDanger* get_rayons();
Explosion* get_explosions();
Boss* get_boss();

// Gestion des paramètres globaux du niveau
void set_nombre_joueurs_mode(int n);
int get_nombre_joueurs_mode();
int get_score_actuel();
int get_timer_debut();
int get_objectif_actuel();
int get_objectif_max();
int get_niveau_en_cours();
int get_timer_niveau();

#endif
