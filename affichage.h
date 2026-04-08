#ifndef AFFICHAGE_H
#define AFFICHAGE_H

void initialiser_affichage(int ecran_w, int ecran_h);
void synchroniser_affichage();
void detruire_affichage();

void animer_fond_menu();

void dessiner_menu_principal();
void dessiner_carte_monde();
void dessiner_regles();
void dessiner_charger_partie();
void dessiner_options();
void dessiner_jeu_en_cours();
void dessiner_ecran_defaite();

#endif
