#include "carte.h"
#include "constantes.h"
#include <string.h> // Indispensable pour utiliser strcpy()

// Variables globales (sans static)
Ville liste_villes[4];
GestionnaireCarte etat_carte;

// Fonction utilitaire pour créer une ville proprement en une seule ligne
void creer_ville(int index, int id, int niveau, int x, int y, char* nom, char* desc1, char* desc2) {
    liste_villes[index].id = id;
    liste_villes[index].numero_niveau = niveau;
    liste_villes[index].x = x;
    liste_villes[index].y = y;
    liste_villes[index].rayon_clic = 30; // Taille standard du bouton invisible

    // Copie sécurisée des textes dans les tableaux de caractères
    strcpy(liste_villes[index].nom, nom);
    strcpy(liste_villes[index].desc_ligne1, desc1);
    strcpy(liste_villes[index].desc_ligne2, desc2);
}

// Prépare les données de la carte et positionne les drapeaux
void initialiser_donnees_carte() {
    // 1. Initialisation par défaut (le joueur regarde la carte)
    etat_carte.mode_actuel = MODE_CARTE_PARCOURIR;
    etat_carte.id_ville_survolee = -1;
    etat_carte.id_ville_selectionnee = -1;

    // 2. Création des villes (Très lisible grâce à la sous-fonction !)
    creer_ville(0, 0, 1, 420, 200, "Paris - France", "Niveau 1 : Debut de l'aventure.", "Difficulte : Facile");
    creer_ville(1, 1, 2, 380, 160, "Londres - UK", "Niveau 2 : Pluie de bulles.", "Difficulte : Moyenne");
    creer_ville(2, 2, 3, 700, 250, "Tokyo - Japon", "Niveau 3 : Le defi ultime.", "Difficulte : Difficile");
    creer_ville(3, 3, 4, 220, 230, "New York (BOSS)", "Niveau 4 FINAL.", "Attention au Boss des bulles !");
}

// Nettoyage appelé par le main.c à la fermeture du jeu
void detruire_donnees_carte() {
    // Rien à détruire dynamiquement ici car nos villes ont une taille fixe en mémoire
}

// Permet aux autres fichiers (comme affichage.c) de lire la liste des villes
Ville* get_villes() {
    return liste_villes;
}

// Permet de savoir combien de villes existent au total
int get_nb_villes() {
    return 4;
}

// Permet de savoir où le joueur a cliqué
GestionnaireCarte* get_etat_carte() {
    return &etat_carte;
}
