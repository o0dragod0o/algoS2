#ifndef MENU_H
#define MENU_H

#include "constantes.h"

// ==========================================
// STRUCTURES DE DONNÉES
// ==========================================

// Zone cliquable rectangulaire dans l'interface
typedef struct {
    int x, y;               // Position du coin supérieur gauche
    int largeur, hauteur;   // Dimensions de la boîte (Hitbox)
    char texte[50];         // Le mot écrit sur le bouton
    EtatJeu etat_cible;     // Le prochain écran à charger si on clique dessus
} Bouton;

// ==========================================
// FONCTIONS PRINCIPALES
// ==========================================

// Calcule les coordonnées pour centrer tous les boutons
void initialiser_boutons_menu(int ecran_w, int ecran_h);

// Vérifie si la souris survole le bouton
int est_clic_sur_bouton(Bouton b, int mx, int my);

// ==========================================
// ACCESSEURS
// ==========================================

Bouton* get_boutons_menu_principal(); // Renvoie le tableau de boutons
int get_nb_boutons_menu_principal();  // Renvoie le nombre de boutons

#endif
