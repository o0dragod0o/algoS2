#include "logique.h"
#include "sauvegarde.h"
#include <allegro.h>
#include <stdlib.h>
#include <time.h>

static Joueur joueur;
static Tir tirs[MAX_TIRS];
static Ballon ballons[MAX_BALLONS];
static RayonDanger rayons[MAX_RAYONS]; // NOUVEAU

static int score_actuel = 0;
static int timer_debut = 0;
static int timer_prochain_spawn = 0;
static int timer_prochain_rayon = 0; // NOUVEAU

static int niveau_en_cours = 1;
static int ballons_detruits = 0;
static int ballons_a_detruire = 10;

int get_niveau_en_cours() { return niveau_en_cours; }

// --- NOUVEAU : Fonction utilitaire pour créer une bulle ---
void creer_ballon(float x, float y, int rayon, float vx, float vy, TypeBulle type) {
    for (int i = 0; i < MAX_BALLONS; i++) {
        if (!ballons[i].actif) {
            ballons[i].actif = 1;
            ballons[i].x = x;
            ballons[i].y = y;
            ballons[i].rayon = rayon;
            ballons[i].vx = vx;
            ballons[i].vy = vy;
            ballons[i].type = type;
            break;
        }
    }
}

// --- NOUVEAU : Mécanique de division ---
void diviser_ballon(int index) {
    Ballon b = ballons[index];
    ballons[index].actif = 0; // La bulle touchée disparait

    int nouveau_rayon = b.rayon / 1.5;
    if (nouveau_rayon < 12) return; // Disparition définitive si trop petite

    switch(b.type) {
        case BULLE_CLASSIQUE:
            // Se divise en 2
            creer_ballon(b.x, b.y, nouveau_rayon, -2.0, -4.0, BULLE_CLASSIQUE);
            creer_ballon(b.x, b.y, nouveau_rayon,  2.0, -4.0, BULLE_CLASSIQUE);
            break;

        case BULLE_MULTI:
            // Se divise en 4 bulles moyennes
            creer_ballon(b.x, b.y, nouveau_rayon, -2.5, -5.0, BULLE_CLASSIQUE);
            creer_ballon(b.x, b.y, nouveau_rayon,  2.5, -5.0, BULLE_CLASSIQUE);
            creer_ballon(b.x, b.y, nouveau_rayon, -1.0, -2.0, BULLE_CLASSIQUE);
            creer_ballon(b.x, b.y, nouveau_rayon,  1.0, -2.0, BULLE_CLASSIQUE);
            break;

        case BULLE_EXPLOSIVE:
            // Eclate en 6 petites bulles très rapides dans toutes les directions
            for(int i = 0; i < 6; i++) {
                float vit_x = ((rand() % 100) / 15.0) - 3.0; // Entre -3.3 et +3.3
                float vit_y = -3.0 - (rand() % 4);           // Vers le haut
                creer_ballon(b.x, b.y, nouveau_rayon - 2, vit_x, vit_y, BULLE_CLASSIQUE);
            }
            break;
    }
}

void initialiser_niveau(int numero_niveau, int ecran_w, int ecran_h) {
    srand(time(NULL));

    joueur.w = 40; joueur.h = 60;
    joueur.x = (ecran_w / 2) - (joueur.w / 2);
    joueur.y = ecran_h - joueur.h;
    joueur.vitesse = 5.0;

    for (int i = 0; i < MAX_TIRS; i++) { tirs[i].actif = 0; tirs[i].w = 4; tirs[i].h = 20; tirs[i].vitesse = 10.0; }
    for (int i = 0; i < MAX_BALLONS; i++) ballons[i].actif = 0;
    for (int i = 0; i < MAX_RAYONS; i++) rayons[i].actif = 0; // Réinitialiser les rayons

    score_actuel = 0;
    timer_debut = 120;
    timer_prochain_spawn = 30;
    timer_prochain_rayon = 300; // Le 1er rayon arrive après un petit moment

    niveau_en_cours = numero_niveau;
    ballons_detruits = 0;
    // Plus il y a de bulles, plus l'objectif est grand
    ballons_a_detruire = numero_niveau * 25;
}

EtatJeu mettre_a_jour_jeu(EtatJeu etat_courant, int ecran_w, int ecran_h) {
    if (key[KEY_ESC]) {
        ajouter_score(get_score_global_partie() + score_actuel);
        return ETAT_CARTE_MONDE;
    }

    if (timer_debut > 0) { timer_debut--; return ETAT_JEU_EN_COURS; }

    if (key[KEY_LEFT] && joueur.x > 0) joueur.x -= joueur.vitesse;
    if (key[KEY_RIGHT] && joueur.x < ecran_w - joueur.w) joueur.x += joueur.vitesse;

    static int cooldown_tir = 0;
    if (cooldown_tir > 0) cooldown_tir--;

    if (key[KEY_SPACE] && cooldown_tir == 0) {
        for (int i = 0; i < MAX_TIRS; i++) {
            if (!tirs[i].actif) {
                tirs[i].actif = 1; tirs[i].x = joueur.x + (joueur.w / 2) - (tirs[i].w / 2); tirs[i].y = joueur.y;
                cooldown_tir = 15;
                break;
            }
        }
    }

    for (int i = 0; i < MAX_TIRS; i++) {
        if (tirs[i].actif) { tirs[i].y -= tirs[i].vitesse; if (tirs[i].y < 0) tirs[i].actif = 0; }
    }

    // --- GENERATION DES NOUVELLES BULLES ---
    if (timer_prochain_spawn > 0) {
        timer_prochain_spawn--;
    } else {
        // Choix du type de bulle selon la difficulté
        TypeBulle nouveau_type = BULLE_CLASSIQUE;
        if (niveau_en_cours >= 2 && rand() % 100 < 30) nouveau_type = BULLE_MULTI;
        if (niveau_en_cours >= 3 && rand() % 100 < 20) nouveau_type = BULLE_EXPLOSIVE;

        float start_x = 30 + rand() % (ecran_w - 60);
        float vit_x = (rand() % 2 == 0 ? 1 : -1) * (1.0 + (rand() % 20) / 10.0);

        creer_ballon(start_x, -40, 20 + rand() % 20, vit_x, 0, nouveau_type);

        timer_prochain_spawn = 80 + rand() % 100;
    }

    // --- NOUVEAU : GENERATION ET MISE A JOUR DES RAYONS LASERS ---
    if (niveau_en_cours >= 1) {
        if (timer_prochain_rayon > 0) {
            timer_prochain_rayon--;
        } else {
            for (int i = 0; i < MAX_RAYONS; i++) {
                if (!rayons[i].actif) {
                    rayons[i].actif = 1;
                    rayons[i].w = 80; // Largeur du danger
                    rayons[i].x = rand() % (ecran_w - rayons[i].w);
                    rayons[i].timer_avertissement = 100; // ~1.5 secondes d'avertissement
                    rayons[i].timer_actif = 50;          // ~0.8 secondes de rayon mortel
                    break;
                }
            }
            timer_prochain_rayon = 200 + rand() % 300;
        }
    }

    // Mise à jour des Rayons
    for (int i = 0; i < MAX_RAYONS; i++) {
        if (rayons[i].actif) {
            if (rayons[i].timer_avertissement > 0) {
                rayons[i].timer_avertissement--;
            } else if (rayons[i].timer_actif > 0) {
                rayons[i].timer_actif--;

                // Détection de collision Rayon mortel vs Joueur
                if (joueur.x < rayons[i].x + rayons[i].w && joueur.x + joueur.w > rayons[i].x) {
                    ajouter_score(get_score_global_partie() + score_actuel);
                    return ETAT_ECRAN_DEFAITE;
                }
            } else {
                rayons[i].actif = 0; // Terminé
            }
        }
    }

    // --- PHYSIQUE DES BULLES ---
    for (int i = 0; i < MAX_BALLONS; i++) {
        if (ballons[i].actif) {
            ballons[i].vy += 0.06;
            ballons[i].x += ballons[i].vx; ballons[i].y += ballons[i].vy;

            if (ballons[i].x - ballons[i].rayon < 0) { ballons[i].x = ballons[i].rayon; ballons[i].vx = -ballons[i].vx; }
            else if (ballons[i].x + ballons[i].rayon > ecran_w) { ballons[i].x = ecran_w - ballons[i].rayon; ballons[i].vx = -ballons[i].vx; }

            if (ballons[i].y + ballons[i].rayon > ecran_h) { ballons[i].y = ecran_h - ballons[i].rayon; ballons[i].vy = -7.5; }

            // Collisions Tir vs Bulle
            for (int j = 0; j < MAX_TIRS; j++) {
                if (tirs[j].actif) {
                    if (tirs[j].x < ballons[i].x + ballons[i].rayon && tirs[j].x + tirs[j].w > ballons[i].x - ballons[i].rayon &&
                        tirs[j].y < ballons[i].y + ballons[i].rayon && tirs[j].y + tirs[j].h > ballons[i].y - ballons[i].rayon) {

                        tirs[j].actif = 0;
                        score_actuel += 150;
                        ballons_detruits++;

                        // NOUVEAU : Diviser au lieu de juste désactiver
                        diviser_ballon(i);
                        break; // On a détruit ce ballon, on passe au suivant
                    }
                }
            }

            // Défaite
            if (ballons[i].actif && joueur.x < ballons[i].x + ballons[i].rayon && joueur.x + joueur.w > ballons[i].x - ballons[i].rayon &&
                joueur.y < ballons[i].y + ballons[i].rayon && joueur.y + joueur.h > ballons[i].y - ballons[i].rayon) {
                ajouter_score(get_score_global_partie() + score_actuel);
                return ETAT_ECRAN_DEFAITE;
            }
        }
    }

    if (ballons_detruits >= ballons_a_detruire) {
        ajouter_score_partie(score_actuel);
        ajouter_score(get_score_global_partie());
        set_niveau_debloque(niveau_en_cours + 1);
        return ETAT_CARTE_MONDE;
    }

    return ETAT_JEU_EN_COURS;
}

Joueur* get_joueur() { return &joueur; }
Tir* get_tirs() { return tirs; }
Ballon* get_ballons() { return ballons; }
RayonDanger* get_rayons() { return rayons; } // NOUVEAU

int get_score_actuel() { return score_actuel; }
int get_timer_debut() { return timer_debut; }
int get_objectif_actuel() { return ballons_detruits; }
int get_objectif_max() { return ballons_a_detruire; }
