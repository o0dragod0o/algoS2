#ifndef CARTE_H
#define CARTE_H

// Définit si le joueur balade sa souris (PARCOURIR) ou s'il lit un panneau (INFO)
typedef enum { MODE_CARTE_PARCOURIR, MODE_CARTE_INFO } ModeCarte;

// Représente un niveau cliquable sur la carte du monde
typedef struct {
    int id;                 // Identifiant unique
    int x, y;               // Position du drapeau sur l'écran
    char nom[50];           // Nom affiché (ex: "Paris - France")
    char desc_ligne1[50];   // 1ère ligne du panneau d'info
    char desc_ligne2[50];   // 2ème ligne du panneau d'info
    int rayon_clic;         // Taille de la zone cliquable pour la souris
    int numero_niveau;      // Niveau chargé si on clique sur "Jouer"
} Ville;

// Gère l'état global de l'écran de sélection des niveaux
typedef struct {
    ModeCarte mode_actuel;
    int id_ville_survolee;     // Ville sous la souris (-1 si aucune)
    int id_ville_selectionnee; // Ville cliquée (-1 si aucune)
    int panel_x, panel_y;      // Position d'affichage du panneau d'info
} GestionnaireCarte;

// --- Fonctions publiques ---
void initialiser_donnees_carte(); // Prépare les villes au lancement
void detruire_donnees_carte();    // Nettoyage mémoire en fin de jeu

// --- Accesseurs ---
Ville* get_villes();                 // Renvoie le tableau des villes
int get_nb_villes();                 // Renvoie le nombre total de villes
GestionnaireCarte* get_etat_carte(); // Renvoie l'état de l'interface

#endif
