#include "logique.h"
#include "sauvegarde.h"
#include <allegro.h>
#include <stdlib.h>
#include <time.h>

// ==========================================
// VARIABLES GLOBALES
// ==========================================
Joueur joueurs[2];          // P1 et P2
int nb_joueurs_choisis = 1; // Mode de jeu actuel

// Tableaux d'entités (Pool d'objets réutilisables)
Tir tirs[MAX_TIRS];
Ballon ballons[MAX_BALLONS];
RayonDanger rayons[MAX_RAYONS];
Explosion explosions[MAX_EXPLOSIONS];
Bonus bonus_liste[MAX_BONUS];
Boss boss;                  // Instance unique du boss

// Variables de progression du niveau
int score_actuel = 0;
int timer_debut = 0;        // Compte à rebours avant le START
int timer_prochain_spawn = 0; // Délai avant la prochaine bulle
int timer_prochain_rayon = 0; // Délai avant le prochain laser
int niveau_en_cours = 1;
int ballons_detruits = 0;
int ballons_a_detruire = 10;
int timer_niveau = 0;       // Chrono global (90 secondes)

// ==========================================
// Générateurs d'entités
// ==========================================

void creer_ballon(float x, float y, int rayon, float vx, float vy, TypeBulle type) {
    // Parcourt le tableau pour trouver un emplacement inactif
    for (int i = 0; i < MAX_BALLONS; i++) {
        if (!ballons[i].actif) {
            ballons[i].actif = 1;
            ballons[i].x = x; ballons[i].y = y;
            ballons[i].rayon = rayon;
            ballons[i].vx = vx; ballons[i].vy = vy;
            ballons[i].type = type;
            break; // On arrête dès qu'on a trouvé une place
        }
    }
}

void creer_explosion(float x, float y, int taille) {
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (!explosions[i].actif) {
            explosions[i].actif = 1;
            explosions[i].x = x; explosions[i].y = y;
            explosions[i].taille = taille;
            explosions[i].frame_actuelle = 0; // Débute à l'image 0
            explosions[i].timer_frame = 0;
            break;
        }
    }
}

void creer_bonus(float x, float y, TypeArme type) {
    for (int i = 0; i < MAX_BONUS; i++) {
        if (!bonus_liste[i].actif) {
            bonus_liste[i].actif = 1;
            bonus_liste[i].x = x; bonus_liste[i].y = y;
            bonus_liste[i].w = BONUS_LARGEUR; bonus_liste[i].h = BONUS_HAUTEUR;
            bonus_liste[i].vy = BONUS_VITESSE_Y; // Tombe vers le bas
            bonus_liste[i].type = type;
            break;
        }
    }
}

void lancer_tir(int id_joueur, float px, float py, float vx, float vy, TypeArme type) {
    for (int i = 0; i < MAX_TIRS; i++) {
        if (!tirs[i].actif) {
            tirs[i].actif = 1;
            tirs[i].id_joueur = id_joueur;
            tirs[i].type_arme = type;

            // Dimensions dynamiques selon l'arme
            tirs[i].w = (type == ARME_EXPLOSIF) ? TIR_EXPLOSIF_W : (type == ARME_RAFALE ? TIR_RAFALE_W : TIR_BASE_W);
            tirs[i].h = (type == ARME_EXPLOSIF) ? TIR_EXPLOSIF_H : (type == ARME_RAFALE ? TIR_RAFALE_H : TIR_BASE_H);

            // Centre le tir horizontalement sur le joueur
            tirs[i].x = px - (tirs[i].w / 2);
            tirs[i].y = py;
            tirs[i].vx = vx; tirs[i].vy = vy;
            break;
        }
    }
}

// ==========================================
// DIVISER BULLES
// ==========================================

// Lance les dés pour savoir si une arme tombe (40% de chance)
void tenter_drop_bonus(Ballon b) {
    if (niveau_en_cours >= 2 && (b.type == BULLE_MULTI || b.type == BULLE_EXPLOSIVE)) {
        if (rand() % 100 < BONUS_CHANCE_DROP) {
            TypeArme arme_drop = (rand() % 3) + 1; // Génère une arme aléatoire (sauf base)
            creer_bonus(b.x, b.y, arme_drop);
        }
    }
}

// Spawns les bulles enfant selon la règle de leur parent
void generer_sous_bulles(Ballon b, int nouveau_rayon) {
    switch(b.type) {
        case BULLE_CLASSIQUE: // Coupe en 2
            creer_ballon(b.x, b.y, nouveau_rayon, -5.0, -10.0, BULLE_CLASSIQUE);
            creer_ballon(b.x, b.y, nouveau_rayon,  5.0, -10.0, BULLE_CLASSIQUE);
            break;
        case BULLE_MULTI: // Coupe en 4
            creer_ballon(b.x, b.y, nouveau_rayon, -6.0, -12.0, BULLE_CLASSIQUE);
            creer_ballon(b.x, b.y, nouveau_rayon,  6.0, -12.0, BULLE_CLASSIQUE);
            creer_ballon(b.x, b.y, nouveau_rayon, -2.5, -5.0, BULLE_CLASSIQUE);
            creer_ballon(b.x, b.y, nouveau_rayon,  2.5, -5.0, BULLE_CLASSIQUE);
            break;
        case BULLE_EXPLOSIVE: // Coupe en 6 fragments chaotiques
            for(int i = 0; i < 6; i++) {
                float vit_x = ((rand() % 100) / 10.0) - 5.0; // Vitesse X aléatoire
                float vit_y = -5.0 - (rand() % 6);           // Vitesse Y aléatoire
                creer_ballon(b.x, b.y, nouveau_rayon - 2, vit_x, vit_y, BULLE_CLASSIQUE);
            }
            break;
    }
}

// Fonction coordinatrice de la destruction d'une bulle
void diviser_ballon(int index) {
    Ballon b = ballons[index];
    ballons[index].actif = 0; // Suppression instantanée
    creer_explosion(b.x, b.y, b.rayon * 2.5); // Effet visuel

    tenter_drop_bonus(b);

    // Calcul de la réduction de taille
    int nouveau_rayon = b.rayon / 1.5;
    if (nouveau_rayon >= BULLE_RAYON_MINIMUM) {
        generer_sous_bulles(b, nouveau_rayon); // Crée les enfants si pas trop petit
    }
}

// ==========================================
// DEGATS JOUEURS
// ==========================================

EtatJeu blesser_joueur(EtatJeu etat_courant, int id_joueur) {
    // Protège si le joueur clignote (invincible) ou est déjà mort
    if (joueurs[id_joueur].actif && !joueurs[id_joueur].est_mort && joueurs[id_joueur].timer_invincibilite == 0) {
        joueurs[id_joueur].vies--;

        if (joueurs[id_joueur].vies <= 0) {
            joueurs[id_joueur].est_mort = 1;
            creer_explosion(joueurs[id_joueur].x, joueurs[id_joueur].y, 100);

            // Vérifie le Game Over global (coop)
            int tout_mort = 1;
            for(int p=0; p < nb_joueurs_choisis; p++) if (!joueurs[p].est_mort) tout_mort = 0;

            if (tout_mort) {
                ajouter_score_partie(score_actuel);
                return ETAT_ECRAN_DEFAITE; // On arrête la boucle principale
            }
        } else {
            // Survie : donne des Frames d'invincibilité et retire le bonus
            joueurs[id_joueur].timer_invincibilite = JOUEUR_TIMER_INVINCIBILITE;
            joueurs[id_joueur].arme_actuelle = ARME_BASE;
        }
    }
    return etat_courant; // On continue à jouer
}

// ==========================================
// GESTION DES TIRS
// ==========================================

// Traduit le type d'arme en projectiles concrets
void declencher_tir_arme(int p, float px, float py, TypeArme arme) {
    if (arme == ARME_EVENTAIL) {
        lancer_tir(p, px, py, -6.0, -22.0, ARME_EVENTAIL); // Gauche
        lancer_tir(p, px, py,  0.0, -25.0, ARME_EVENTAIL); // Centre
        lancer_tir(p, px, py,  6.0, -22.0, ARME_EVENTAIL); // Droite
    } else if (arme == ARME_EXPLOSIF) {
        lancer_tir(p, px, py, 0.0, TIR_EXPLOSIF_VY, ARME_EXPLOSIF);
    } else if (arme == ARME_RAFALE) {
        lancer_tir(p, px, py, 0.0, TIR_RAFALE_VY, ARME_RAFALE);
    } else {
        lancer_tir(p, px, py, 0.0, TIR_BASE_VY, ARME_BASE);
    }
}

// Vérifie les touches et les limites avant de tirer
void gerer_tirs_joueurs() {
    static int cd[2] = {0, 0}; // Cooldown (Délai) individuel

    for(int p=0; p<2; p++) {
        if (cd[p] > 0) cd[p]--; // Décrémente le cooldown

        int touche_tir = (p == 0) ? key[KEY_SPACE] : (key[KEY_Z] || key[KEY_W]);

        if (joueurs[p].actif && !joueurs[p].est_mort && touche_tir && cd[p] == 0) {

            // Chargement des limites selon l'arme tenue
            int max_s = (joueurs[p].arme_actuelle == ARME_BASE) ? ARME_BASE_MAX_ECRAN :
                        (joueurs[p].arme_actuelle == ARME_RAFALE) ? ARME_RAFALE_MAX_ECRAN :
                        (joueurs[p].arme_actuelle == ARME_EVENTAIL) ? ARME_EVENTAIL_MAX_ECRAN : ARME_EXPLOSIF_MAX_ECRAN;

            int cd_val = (joueurs[p].arme_actuelle == ARME_RAFALE) ? ARME_RAFALE_COOLDOWN :
                         (joueurs[p].arme_actuelle == ARME_EVENTAIL) ? ARME_EVENTAIL_COOLDOWN :
                         (joueurs[p].arme_actuelle == ARME_EXPLOSIF) ? ARME_EXPLOSIF_COOLDOWN : ARME_BASE_COOLDOWN;

            // Bloque le tir si trop de balles sont déjà à l'écran
            int count = 0;
            for(int i=0; i<MAX_TIRS; i++) if(tirs[i].actif && tirs[i].id_joueur == p) count++;

            if (count < max_s) {
                float px = joueurs[p].x + (joueurs[p].w / 2); // Centre la balle
                declencher_tir_arme(p, px, joueurs[p].y, joueurs[p].arme_actuelle);
                cd[p] = cd_val; // Réinitialise le cooldown
            }
        }
    }
}

// ==========================================
// LE BOSS
// ==========================================

// Rend le boss plus dangereux quand sa vie baisse
void actualiser_phase_boss() {
    if (boss.pv_actuel <= 150 && boss.phase == 2) {
        boss.phase = 3; boss.vy = -8.0; boss.timer_attaque = 0; boss.vx = 8.0; // Mode berserk
    } else if (boss.pv_actuel <= 300 && boss.phase == 1) {
        boss.phase = 2; boss.timer_attaque = 60; // Mode laser
    }
}

// Déplacement spécifique au boss
void deplacer_boss(int ecran_w, int ecran_h) {
    boss.x += boss.vx;
    // Rebonds latéraux
    if (boss.x < 0) { boss.x = 0; boss.vx = -boss.vx; }
    if (boss.x + boss.w > ecran_w) { boss.x = ecran_w - boss.w; boss.vx = -boss.vx; }

    // En phase 3, le boss bondit sur l'écran (gravité et rebond y)
    if (boss.phase == 3) {
        boss.vy += 0.4;
        boss.y += boss.vy;
        if (boss.y + boss.h > ecran_h) { boss.y = ecran_h - boss.h; boss.vy = -15.0; }
        if (boss.y < 0) { boss.y = 0; boss.vy = 5.0; }
    }
}

// Le boss génère des obstacles
void gerer_attaques_boss() {
    if (boss.timer_attaque > 0) boss.timer_attaque--;
    else {
        if (boss.phase == 1) {
            // Crache des bulles par le dessous
            TypeBulle type = (rand() % 2 == 0) ? BULLE_MULTI : BULLE_EXPLOSIVE;
            creer_ballon(boss.x + boss.w/2, boss.y + boss.h, 30, (rand()%2==0?3.0:-3.0), 0, type);
            boss.timer_attaque = 90;
        } else if (boss.phase == 2) {
            // Invoque des lasers au centre de sa hitbox
            for (int i = 0; i < MAX_RAYONS; i++) {
                if (!rayons[i].actif) {
                    rayons[i].actif = 1; rayons[i].w = 80; rayons[i].x = boss.x + boss.w/2 - 40;
                    rayons[i].timer_avertissement = 60; rayons[i].timer_actif = 30; break;
                }
            }
            boss.timer_attaque = 45;
        }
    }
}

// Le boss écrase le joueur
EtatJeu gerer_collision_boss_joueurs(EtatJeu etat_courant) {
    for(int p=0; p<2; p++) {
        if (joueurs[p].actif && !joueurs[p].est_mort &&
            joueurs[p].x < boss.x + boss.w && joueurs[p].x + joueurs[p].w > boss.x &&
            joueurs[p].y < boss.y + boss.h && joueurs[p].y + boss.h > boss.y) {
            etat_courant = blesser_joueur(etat_courant, p);
        }
    }
    return etat_courant;
}

// Le joueur inflige des dégâts au boss
void gerer_collision_boss_tirs() {
    for (int i = 0; i < MAX_TIRS; i++) {
        if (tirs[i].actif && tirs[i].x < boss.x + boss.w && tirs[i].x + tirs[i].w > boss.x &&
            tirs[i].y < boss.y + boss.h && tirs[i].y + tirs[i].h > boss.y) {

            // Dégâts majeurs pour le tir jaune
            if (tirs[i].type_arme == ARME_EXPLOSIF) {
                creer_explosion(tirs[i].x, tirs[i].y, TIR_EXPLOSIF_EXPLO_TAILLE);
                boss.pv_actuel -= TIR_EXPLOSIF_DEGATS;
            } else {
                creer_explosion(tirs[i].x, tirs[i].y, 60);
                boss.pv_actuel -= TIR_BASE_DEGATS;
            }

            tirs[i].actif = 0; // Détruit la balle
            score_actuel += 50;

            // Mort du boss
            if (boss.pv_actuel <= 0) {
                boss.actif = 0;
                score_actuel += POINTS_VICTOIRE_BOSS;
                ballons_detruits = ballons_a_detruire; // Valide l'objectif du niveau
            }
        }
    }
}

// Fonction coordinatrice du boss
EtatJeu mettre_a_jour_boss(EtatJeu etat_courant, int ecran_w, int ecran_h) {
    actualiser_phase_boss();
    deplacer_boss(ecran_w, ecran_h);
    gerer_attaques_boss();
    etat_courant = gerer_collision_boss_joueurs(etat_courant);
    gerer_collision_boss_tirs();
    return etat_courant;
}

// ==========================================
// LES RAYONS LASERS
// ==========================================

void generer_nouveaux_rayons(int ecran_w) {
    if (niveau_en_cours >= 1) {
        if (timer_prochain_rayon > 0) { timer_prochain_rayon--; }
        else {
            for (int i = 0; i < MAX_RAYONS; i++) {
                if (!rayons[i].actif) {
                    rayons[i].actif = 1; rayons[i].w = 80;
                    rayons[i].x = rand() % (ecran_w - rayons[i].w); // Spawn aléatoire
                    rayons[i].timer_avertissement = 100; // Temps pour fuir (Rose)
                    rayons[i].timer_actif = 50;          // Temps actif (Rouge mortel)
                    break;
                }
            }
            timer_prochain_rayon = 200 + rand() % 300;
        }
    }
}

EtatJeu appliquer_degats_rayons(EtatJeu etat_courant) {
    for (int i = 0; i < MAX_RAYONS; i++) {
        if (rayons[i].actif) {
            // Décompte de la phase de prévention
            if (rayons[i].timer_avertissement > 0) {
                rayons[i].timer_avertissement--;
            }
            // Phase active : Blesse tout joueur dans le périmètre horizontal
            else if (rayons[i].timer_actif > 0) {
                rayons[i].timer_actif--;
                for(int p=0; p<2; p++) {
                    if (joueurs[p].actif && !joueurs[p].est_mort &&
                        joueurs[p].x < rayons[i].x + rayons[i].w && joueurs[p].x + joueurs[p].w > rayons[i].x) {
                        etat_courant = blesser_joueur(etat_courant, p);
                    }
                }
            } else {
                rayons[i].actif = 0; // Fin du laser
            }
        }
    }
    return etat_courant;
}

// Fonction coordinatrice des lasers
EtatJeu mettre_a_jour_rayons(EtatJeu etat_courant, int ecran_w) {
    generer_nouveaux_rayons(ecran_w);
    return appliquer_degats_rayons(etat_courant);
}

// ==========================================
// LES BULLES
// ==========================================

void deplacer_un_ballon(int i, int ecran_w, int ecran_h) {
    ballons[i].vy += 0.2; // Applique la gravité (Tire vers le bas)
    ballons[i].x += ballons[i].vx;
    ballons[i].y += ballons[i].vy;

    // Rebonds latéraux (Inverse la vélocité X)
    if (ballons[i].x - ballons[i].rayon < 0) { ballons[i].x = ballons[i].rayon; ballons[i].vx = -ballons[i].vx; }
    else if (ballons[i].x + ballons[i].rayon > ecran_w) { ballons[i].x = ecran_w - ballons[i].rayon; ballons[i].vx = -ballons[i].vx; }

    // Rebond au sol (Force la vélocité vers le haut)
    if (ballons[i].y + ballons[i].rayon > ecran_h) { ballons[i].y = ecran_h - ballons[i].rayon; ballons[i].vy = -12.0; }
}

// Vérifie si un tir percute la bulle i. Retourne 1 si impact, 0 sinon.
int gerer_impact_tir_sur_ballon(int i) {
    for (int j = 0; j < MAX_TIRS; j++) {
        // Simple détection de boîte englobante
        if (tirs[j].actif && tirs[j].x < ballons[i].x + ballons[i].rayon && tirs[j].x + tirs[j].w > ballons[i].x - ballons[i].rayon &&
            tirs[j].y < ballons[i].y + ballons[i].rayon && tirs[j].y + tirs[j].h > ballons[i].y - ballons[i].rayon) {

            // Comportement spécifique : La bombe explose et souffle en zone
            if (tirs[j].type_arme == ARME_EXPLOSIF) {
                float tx = tirs[j].x; float ty = tirs[j].y;
                tirs[j].actif = 0;
                creer_explosion(tx, ty, TIR_EXPLOSIF_EXPLO_TAILLE);

                // Dégâts de zone (Théorème de Pythagore pour un cercle d'explosion)
                for(int k=0; k < MAX_BALLONS; k++) {
                    if (ballons[k].actif) {
                        float dx = ballons[k].x - tx; float dy = ballons[k].y - ty;
                        if (dx*dx + dy*dy < TIR_EXPLOSIF_RAYON_SOUFFLE * TIR_EXPLOSIF_RAYON_SOUFFLE) {
                            score_actuel += POINTS_PAR_BULLE; ballons_detruits++; diviser_ballon(k);
                        }
                    }
                }
            } else {
                // Comportement normal : Destruction cible unique
                tirs[j].actif = 0;
                score_actuel += POINTS_PAR_BULLE;
                ballons_detruits++;
                diviser_ballon(i);
            }
            return 1; // Impact validé
        }
    }
    return 0;
}

EtatJeu gerer_impact_joueur_sur_ballon(EtatJeu etat_courant, int i) {
    for(int p=0; p<2; p++) {
        if (joueurs[p].actif && !joueurs[p].est_mort && ballons[i].actif &&
            joueurs[p].x < ballons[i].x + ballons[i].rayon && joueurs[p].x + joueurs[p].w > ballons[i].x - ballons[i].rayon &&
            joueurs[p].y < ballons[i].y + ballons[i].rayon && joueurs[p].y + joueurs[p].h > ballons[i].y - ballons[i].rayon) {

            etat_courant = blesser_joueur(etat_courant, p);
        }
    }
    return etat_courant;
}

// Fonction coordinatrice des bulles
EtatJeu mettre_a_jour_ballons(EtatJeu etat_courant, int ecran_w, int ecran_h) {
    for (int i = 0; i < MAX_BALLONS; i++) {
        if (ballons[i].actif) {
            deplacer_un_ballon(i, ecran_w, ecran_h);

            // Si la bulle a été détruite par un tir, inutile de calculer si elle blesse le joueur
            if (gerer_impact_tir_sur_ballon(i)) {
                continue; // Saute au prochain tour de boucle
            }

            // Vérifie si la bulle écrase un joueur
            etat_courant = gerer_impact_joueur_sur_ballon(etat_courant, i);
            if (etat_courant == ETAT_ECRAN_DEFAITE) return etat_courant; // Mort
        }
    }
    return etat_courant;
}

// ==========================================
// SOUS-FONCTIONS GLOBALES
// ==========================================

void initialiser_niveau(int numero_niveau, int ecran_w, int ecran_h) {
    srand(time(NULL)); // Mélange la graine aléatoire

    // 1. Initialise les joueurs
    for (int p = 0; p < 2; p++) {
        joueurs[p].actif = (p < nb_joueurs_choisis);
        joueurs[p].w = JOUEUR_LARGEUR;
        joueurs[p].h = JOUEUR_HAUTEUR;
        joueurs[p].vitesse = JOUEUR_VITESSE_BASE;
        joueurs[p].timer_invincibilite = 0;
        joueurs[p].est_mort = 0;
        joueurs[p].arme_actuelle = ARME_BASE;
        joueurs[p].timer_arme = 0;
        joueurs[p].vies = (numero_niveau == 4) ? 3 : 1; // Bonus boss final

        if (p == 0) joueurs[p].x = (ecran_w / 2) - 60; else joueurs[p].x = (ecran_w / 2) + 20;
        joueurs[p].y = ecran_h - joueurs[p].h;
    }

    // 2. Vide tous les tableaux d'entités
    for (int i = 0; i < MAX_TIRS; i++) tirs[i].actif = 0;
    for (int i = 0; i < MAX_BALLONS; i++) ballons[i].actif = 0;
    for (int i = 0; i < MAX_RAYONS; i++) rayons[i].actif = 0;
    for (int i = 0; i < MAX_EXPLOSIONS; i++) explosions[i].actif = 0;
    for (int i = 0; i < MAX_BONUS; i++) bonus_liste[i].actif = 0;

    // 3. Reset les règles du niveau
    score_actuel = 0;
    timer_debut = TIMER_DEBUT_NIVEAU;
    timer_prochain_spawn = 30;
    timer_prochain_rayon = 300;
    timer_niveau = TEMPS_NIVEAU_SECONDES * FRAMES_PAR_SECONDE;
    niveau_en_cours = numero_niveau;
    ballons_detruits = 0;
    ballons_a_detruire = numero_niveau * 25; // Difficulté croissante

    // 4. Initialise le boss s'il y a lieu
    if (numero_niveau == 4) {
        boss.actif = 1; boss.w = BOSS_LARGEUR; boss.h = BOSS_HAUTEUR;
        boss.x = (ecran_w - boss.w) / 2; boss.y = 10;
        boss.pv_max = BOSS_PV_MAX; boss.pv_actuel = BOSS_PV_MAX;
        boss.phase = 1; boss.vx = 6.0; boss.vy = 0.0; boss.timer_attaque = 60;
    } else {
        boss.actif = 0;
    }
}

void gerer_mouvements_joueurs(int ecran_w) {
    for(int p = 0; p < 2; p++) {
        if (joueurs[p].actif && !joueurs[p].est_mort) {

            // Mise à jour des chronos personnels
            if (joueurs[p].timer_invincibilite > 0) joueurs[p].timer_invincibilite--;
            if (joueurs[p].timer_arme > 0) {
                joueurs[p].timer_arme--;
                if (joueurs[p].timer_arme == 0) joueurs[p].arme_actuelle = ARME_BASE; // Fin de buff
            }

            // Déplacements P1 et P2 limités par les bords de l'écran
            if (p == 0) {
                if (key[KEY_LEFT] && joueurs[0].x > 0) joueurs[0].x -= joueurs[0].vitesse;
                if (key[KEY_RIGHT] && joueurs[0].x < ecran_w - joueurs[0].w) joueurs[0].x += joueurs[0].vitesse;
            } else {
                if ((key[KEY_Q] || key[KEY_A]) && joueurs[1].x > 0) joueurs[1].x -= joueurs[1].vitesse;
                if (key[KEY_D] && joueurs[1].x < ecran_w - joueurs[1].w) joueurs[1].x += joueurs[1].vitesse;
            }
        }
    }
}

void mettre_a_jour_bonus(int ecran_h) {
    for(int i=0; i<MAX_BONUS; i++) {
        if(bonus_liste[i].actif) {
            bonus_liste[i].y += bonus_liste[i].vy; // Gravité

            if (bonus_liste[i].y > ecran_h) { // Sort de l'écran par le bas
                bonus_liste[i].actif = 0;
            } else {
                for(int p=0; p<2; p++) {
                    // Si joueur et bonus se chevauchent = Ramassage
                    if (joueurs[p].actif && !joueurs[p].est_mort &&
                        joueurs[p].x < bonus_liste[i].x + bonus_liste[i].w && joueurs[p].x + joueurs[p].w > bonus_liste[i].x &&
                        joueurs[p].y < bonus_liste[i].y + bonus_liste[i].h && joueurs[p].y + joueurs[p].h > bonus_liste[i].y) {

                        joueurs[p].arme_actuelle = bonus_liste[i].type;
                        joueurs[p].timer_arme = JOUEUR_TIMER_ARME_SPECIALE;
                        bonus_liste[i].actif = 0;
                        score_actuel += POINTS_BONUS_RAMASSE;
                    }
                }
            }
        }
    }
}

void mettre_a_jour_projectiles(int ecran_w) {
    for (int i = 0; i < MAX_TIRS; i++) {
        if (tirs[i].actif) {
            tirs[i].x += tirs[i].vx;
            tirs[i].y += tirs[i].vy;
            // Purge les tirs sortis de la fenêtre (Haut, Gauche, Droite)
            if (tirs[i].y < 0 || tirs[i].x < 0 || tirs[i].x > ecran_w) tirs[i].actif = 0;
        }
    }
}

void mettre_a_jour_explosions() {
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (explosions[i].actif) {
            explosions[i].timer_frame++; // Ralentit l'animation (change tous les 2 ticks)
            if (explosions[i].timer_frame > 2) {
                explosions[i].timer_frame = 0;
                explosions[i].frame_actuelle++;
                if (explosions[i].frame_actuelle >= 8) explosions[i].actif = 0; // Animation finie
            }
        }
    }
}

// Lâche des bulles depuis le ciel (Pour le mode de jeu standard hors boss)
void generer_ennemis_aleatoires(int ecran_w) {
    if (timer_prochain_spawn > 0) { timer_prochain_spawn--; }
    else {
        // Apparition conditionnée par la difficulté
        TypeBulle nouveau_type = BULLE_CLASSIQUE;
        if (niveau_en_cours >= 2 && rand() % 100 < 30) nouveau_type = BULLE_MULTI;
        if (niveau_en_cours >= 3 && rand() % 100 < 20) nouveau_type = BULLE_EXPLOSIVE;

        float start_x = 30 + rand() % (ecran_w - 60);
        float vit_x = (rand() % 2 == 0 ? 1 : -1) * (2.0 + (rand() % 20) / 10.0);

        creer_ballon(start_x, -40, 20 + rand() % 20, vit_x, 0, nouveau_type);
        timer_prochain_spawn = 80 + rand() % 100; // Recalcule le prochain spawn
    }
}

EtatJeu verifier_fin_niveau() {
    if (ballons_detruits >= ballons_a_detruire) {
        // Bonus de temps
        int bonus_temps = (timer_niveau / FRAMES_PAR_SECONDE) * POINTS_PAR_SECONDE_RESTANTE;
        score_actuel += bonus_temps;

        // Sauvegarde la progression
        ajouter_score_partie(score_actuel);
        set_niveau_debloque(niveau_en_cours + 1);
        sauvegarder_partie(pseudo_actuel, get_niveau_debloque(), get_score_global_partie());

        return ETAT_ECRAN_VICTOIRE; // Fin du niveau !
    }
    return ETAT_JEU_EN_COURS;
}

// ==========================================
// BOUCLE PRINCIPALE
// ==========================================

// Fonction appelée en continu (60x par seconde) pendant la phase de jeu
EtatJeu mettre_a_jour_jeu(EtatJeu etat_courant, int ecran_w, int ecran_h) {

    // 1. Bouton Pause
    static int esc_presse = 0;
    if (key[KEY_ESC]) { if (!esc_presse) { esc_presse = 1; return ETAT_PAUSE; } } else esc_presse = 0;

    // 2. Gestion des compteurs de temps
    if (timer_debut > 0) { timer_debut--; return ETAT_JEU_EN_COURS; } // Freeze le jeu au start
    else if (timer_niveau > 0) timer_niveau--;

    // 3. Mise à jour des entités actives
    gerer_mouvements_joueurs(ecran_w);
    gerer_tirs_joueurs();
    mettre_a_jour_bonus(ecran_h);
    mettre_a_jour_projectiles(ecran_w);
    mettre_a_jour_explosions();

    // 4. Logique conditionnelle (Boss vs Mode Normal)
    if (boss.actif) {
        etat_courant = mettre_a_jour_boss(etat_courant, ecran_w, ecran_h);
        if (etat_courant != ETAT_JEU_EN_COURS) return etat_courant; // Arrête si GameOver
    } else {
        generer_ennemis_aleatoires(ecran_w);
    }

    // 5. Collisions de l'environnement (Si on ne meurt pas, on continue)
    etat_courant = mettre_a_jour_rayons(etat_courant, ecran_w);
    if (etat_courant != ETAT_JEU_EN_COURS) return etat_courant;

    etat_courant = mettre_a_jour_ballons(etat_courant, ecran_w, ecran_h);
    if (etat_courant != ETAT_JEU_EN_COURS) return etat_courant;

    // 6. Vérification de l'objectif
    return verifier_fin_niveau();
}

// ==========================================
// ACCESSEURS
// ==========================================
void set_nombre_joueurs_mode(int n) { nb_joueurs_choisis = n; }
int get_nombre_joueurs_mode() { return nb_joueurs_choisis; }
int get_niveau_en_cours() { return niveau_en_cours; }
int get_timer_niveau() { return timer_niveau; }
Joueur* get_joueurs() { return joueurs; }
Tir* get_tirs() { return tirs; }
Ballon* get_ballons() { return ballons; }
RayonDanger* get_rayons() { return rayons; }
Explosion* get_explosions() { return explosions; }
Boss* get_boss() { return &boss; }
Bonus* get_bonus() { return bonus_liste; }
int get_score_actuel() { return score_actuel; }
int get_timer_debut() { return timer_debut; }
int get_objectif_actuel() { return ballons_detruits; }
int get_objectif_max() { return ballons_a_detruire; }
