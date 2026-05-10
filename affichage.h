#ifndef AFFICHAGE_H
#define AFFICHAGE_H

// --- Fonctions de base de l'affichage ---
void initialiser_affichage(int ecran_w, int ecran_h); // Crée le double buffer
void synchroniser_affichage();                        // Envoie le buffer à l'écran
void detruire_affichage();                            // Libère la mémoire du buffer

// --- Animations globales ---
void animer_fond_menu(); // Fait défiler l'image de fond

// --- Écrans hors-jeu (Menus) ---
void dessiner_menu_principal();   // Écran d'accueil avec les boutons
void dessiner_carte_monde();      // Écran de sélection des niveaux
void dessiner_charger_partie();   // Écran de saisie du pseudo
void dessiner_options();          // Écran des paramètres (volume, écran)
void dessiner_regles();           // Écran explicatif des touches

// --- Écrans en jeu ---
void dessiner_jeu_en_cours();     // Cœur de l'affichage (joueurs, bulles, tirs)
void dessiner_ecran_defaite();    // Overlay Game Over
void dessiner_pause();            // Overlay Menu Pause
void dessiner_ecran_victoire();   // Overlay Niveau terminé

#endif
