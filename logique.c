#include "logique.h"
#include "sauvegarde.h"
#include <allegro.h>
#include <stdlib.h>
#include <time.h>

static Joueur joueurs[2];
static int nb_joueurs_choisis = 1;

static Tir tirs[MAX_TIRS];
static Ballon ballons[MAX_BALLONS];
static RayonDanger rayons[MAX_RAYONS];
static Explosion explosions[MAX_EXPLOSIONS];
static Boss boss;

static int score_actuel = 0;
static int timer_debut = 0;
static int timer_prochain_spawn = 0;
static int timer_prochain_rayon = 0;
static int niveau_en_cours = 1;
static int ballons_detruits = 0;
static int ballons_a_detruire = 10;

void set_nombre_joueurs_mode(int n) { nb_joueurs_choisis = n; }
int get_nombre_joueurs_mode() { return nb_joueurs_choisis; }
int get_niveau_en_cours() { return niveau_en_cours; }

void creer_ballon(float x, float y, int rayon, float vx, float vy, TypeBulle type) {
    for (int i = 0; i < MAX_BALLONS; i++) {
        if (!ballons[i].actif) {
            ballons[i].actif = 1; ballons[i].x = x; ballons[i].y = y; ballons[i].rayon = rayon;
            ballons[i].vx = vx; ballons[i].vy = vy; ballons[i].type = type; break;
        }
    }
}

void creer_explosion(float x, float y, int taille) {
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (!explosions[i].actif) {
            explosions[i].actif = 1; explosions[i].x = x; explosions[i].y = y;
            explosions[i].taille = taille; explosions[i].frame_actuelle = 0; explosions[i].timer_frame = 0; break;
        }
    }
}

void diviser_ballon(int index) {
    Ballon b = ballons[index];
    ballons[index].actif = 0;
    creer_explosion(b.x, b.y, b.rayon * 2.5);

    int nouveau_rayon = b.rayon / 1.5;

    // CORRECTION ICI : Si le rayon est inférieur à 16, la bulle est détruite et ne se divise plus
    if (nouveau_rayon < 16) return;

    switch(b.type) {
        case BULLE_CLASSIQUE:
            creer_ballon(b.x, b.y, nouveau_rayon, -5.0, -10.0, BULLE_CLASSIQUE);
            creer_ballon(b.x, b.y, nouveau_rayon,  5.0, -10.0, BULLE_CLASSIQUE);
            break;
        case BULLE_MULTI:
            creer_ballon(b.x, b.y, nouveau_rayon, -6.0, -12.0, BULLE_CLASSIQUE);
            creer_ballon(b.x, b.y, nouveau_rayon,  6.0, -12.0, BULLE_CLASSIQUE);
            creer_ballon(b.x, b.y, nouveau_rayon, -2.5, -5.0, BULLE_CLASSIQUE);
            creer_ballon(b.x, b.y, nouveau_rayon,  2.5, -5.0, BULLE_CLASSIQUE);
            break;
        case BULLE_EXPLOSIVE:
            for(int i = 0; i < 6; i++) {
                float vit_x = ((rand() % 100) / 10.0) - 5.0;
                float vit_y = -5.0 - (rand() % 6);
                creer_ballon(b.x, b.y, nouveau_rayon - 2, vit_x, vit_y, BULLE_CLASSIQUE);
            }
            break;
    }
}

EtatJeu blesser_joueur(EtatJeu etat_courant, int id_joueur) {
    if (joueurs[id_joueur].actif && !joueurs[id_joueur].est_mort && joueurs[id_joueur].timer_invincibilite == 0) {
        joueurs[id_joueur].vies--;

        if (joueurs[id_joueur].vies <= 0) {
            joueurs[id_joueur].est_mort = 1;
            creer_explosion(joueurs[id_joueur].x, joueurs[id_joueur].y, 100);

            int tout_le_monde_mort = 1;
            for(int p=0; p < nb_joueurs_choisis; p++) {
                if (!joueurs[p].est_mort) tout_le_monde_mort = 0;
            }

            if (tout_le_monde_mort) {
                ajouter_score(get_score_global_partie() + score_actuel);
                return ETAT_ECRAN_DEFAITE;
            }
        } else {
            joueurs[id_joueur].timer_invincibilite = 120;
        }
    }
    return etat_courant;
}

void initialiser_niveau(int numero_niveau, int ecran_w, int ecran_h) {
    srand(time(NULL));

    for (int p = 0; p < 2; p++) {
        joueurs[p].actif = (p < get_nombre_joueurs_mode());

        // CORRECTION TAILLE : On double la taille (40x52) pour garder le ratio exact (20x26) sans être minuscule !
        joueurs[p].w = 40;
        joueurs[p].h = 52;

        joueurs[p].vitesse = 12.0;
        joueurs[p].timer_invincibilite = 0;
        joueurs[p].est_mort = 0;
        joueurs[p].vies = (numero_niveau == 4) ? 3 : 1;

        if (p == 0) joueurs[p].x = (ecran_w / 2) - 60;
        else joueurs[p].x = (ecran_w / 2) + 20;

        joueurs[p].y = ecran_h - joueurs[p].h;
    }

    for (int i = 0; i < MAX_TIRS; i++) {
        tirs[i].actif = 0;
        tirs[i].w = 4;
        tirs[i].h = 20;
        tirs[i].vitesse = 25.0;
    }

    for (int i = 0; i < MAX_BALLONS; i++) ballons[i].actif = 0;
    for (int i = 0; i < MAX_RAYONS; i++) rayons[i].actif = 0;
    for (int i = 0; i < MAX_EXPLOSIONS; i++) explosions[i].actif = 0;

    score_actuel = 0; timer_debut = 120; timer_prochain_spawn = 30; timer_prochain_rayon = 300;
    niveau_en_cours = numero_niveau; ballons_detruits = 0; ballons_a_detruire = numero_niveau * 25;

    if (numero_niveau == 4) {
        boss.actif = 1; boss.w = 400; boss.h = 190;
        boss.x = (ecran_w - boss.w) / 2; boss.y = 10;
        boss.pv_max = 500; boss.pv_actuel = 500;
        boss.phase = 1; boss.vx = 6.0; boss.vy = 0.0; boss.timer_attaque = 60;
    } else boss.actif = 0;
}

EtatJeu mettre_a_jour_jeu(EtatJeu etat_courant, int ecran_w, int ecran_h) {
    static int esc_presse = 0;
    if (key[KEY_ESC]) { if (!esc_presse) { esc_presse = 1; return ETAT_PAUSE; } } else esc_presse = 0;
    if (timer_debut > 0) { timer_debut--; return ETAT_JEU_EN_COURS; }

    for(int p=0; p<2; p++) {
        if (joueurs[p].actif && !joueurs[p].est_mort) {
            if (joueurs[p].timer_invincibilite > 0) joueurs[p].timer_invincibilite--;

            if (p == 0) {
                if (key[KEY_LEFT] && joueurs[0].x > 0) joueurs[0].x -= joueurs[0].vitesse;
                if (key[KEY_RIGHT] && joueurs[0].x < ecran_w - joueurs[0].w) joueurs[0].x += joueurs[0].vitesse;
            } else {
                if ((key[KEY_Q] || key[KEY_A]) && joueurs[1].x > 0) joueurs[1].x -= joueurs[1].vitesse;
                if (key[KEY_D] && joueurs[1].x < ecran_w - joueurs[1].w) joueurs[1].x += joueurs[1].vitesse;
            }
        }
    }

    static int cd[2] = {0, 0};
    for(int p=0; p<2; p++) {
        if (cd[p] > 0) cd[p]--;
        int k = (p == 0) ? key[KEY_SPACE] : (key[KEY_Z] || key[KEY_W]);
        if (joueurs[p].actif && !joueurs[p].est_mort && k && cd[p] == 0) {
            int count = 0;
            for(int i=0; i<MAX_TIRS; i++) if(tirs[i].actif && tirs[i].id_joueur == p) count++;
            if (count < 5) {
                for(int i=0; i<MAX_TIRS; i++) if(!tirs[i].actif) {
                    tirs[i].actif = 1; tirs[i].id_joueur = p;
                    tirs[i].x = joueurs[p].x + (joueurs[p].w/2) - 2; tirs[i].y = joueurs[p].y;
                    cd[p] = 10; break;
                }
            }
        }
    }

    for (int i = 0; i < MAX_TIRS; i++) { if (tirs[i].actif) { tirs[i].y -= tirs[i].vitesse; if (tirs[i].y < 0) tirs[i].actif = 0; } }

    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (explosions[i].actif) {
            explosions[i].timer_frame++;
            if (explosions[i].timer_frame > 2) {
                explosions[i].timer_frame = 0; explosions[i].frame_actuelle++;
                if (explosions[i].frame_actuelle >= 8) explosions[i].actif = 0;
            }
        }
    }

    // --- GESTION BOSS ---
    if (boss.actif) {
        if (boss.pv_actuel <= 150 && boss.phase == 2) { boss.phase = 3; boss.vy = -8.0; boss.timer_attaque = 0; boss.vx = 8.0; }
        else if (boss.pv_actuel <= 300 && boss.phase == 1) { boss.phase = 2; boss.timer_attaque = 60; }

        boss.x += boss.vx;
        if (boss.x < 0) { boss.x = 0; boss.vx = -boss.vx; }
        if (boss.x + boss.w > ecran_w) { boss.x = ecran_w - boss.w; boss.vx = -boss.vx; }

        if (boss.phase == 3) {
            boss.vy += 0.4; boss.y += boss.vy;
            if (boss.y + boss.h > ecran_h) { boss.y = ecran_h - boss.h; boss.vy = -15.0; }
            if (boss.y < 0) { boss.y = 0; boss.vy = 5.0; }
        }

        if (boss.timer_attaque > 0) boss.timer_attaque--;
        else {
            if (boss.phase == 1) {
                TypeBulle type = (rand() % 2 == 0) ? BULLE_MULTI : BULLE_EXPLOSIVE;
                creer_ballon(boss.x + boss.w/2, boss.y + boss.h, 30, (rand()%2==0?3.0:-3.0), 0, type);
                boss.timer_attaque = 90;
            } else if (boss.phase == 2) {
                for (int i = 0; i < MAX_RAYONS; i++) {
                    if (!rayons[i].actif) {
                        rayons[i].actif = 1; rayons[i].w = 80; rayons[i].x = boss.x + boss.w/2 - 40;
                        rayons[i].timer_avertissement = 60; rayons[i].timer_actif = 30; break;
                    }
                }
                boss.timer_attaque = 45;
            }
        }

        // Collisions Boss -> Joueurs
        for(int p=0; p<2; p++) {
            if (joueurs[p].actif && !joueurs[p].est_mort &&
                joueurs[p].x < boss.x + boss.w && joueurs[p].x + joueurs[p].w > boss.x &&
                joueurs[p].y < boss.y + boss.h && joueurs[p].y + boss.h > boss.y) {
                etat_courant = blesser_joueur(etat_courant, p);
                if (etat_courant == ETAT_ECRAN_DEFAITE) return etat_courant;
            }
        }

        // Tirs -> Boss
        for (int i = 0; i < MAX_TIRS; i++) {
            if (tirs[i].actif && tirs[i].x < boss.x + boss.w && tirs[i].x + tirs[i].w > boss.x &&
                tirs[i].y < boss.y + boss.h && tirs[i].y + tirs[i].h > boss.y) {
                tirs[i].actif = 0; boss.pv_actuel -= 10; score_actuel += 50;
                creer_explosion(tirs[i].x, tirs[i].y, 60);
                if (boss.pv_actuel <= 0) { boss.actif = 0; score_actuel += 5000; ballons_detruits = ballons_a_detruire; }
            }
        }
    }

    if (!boss.actif) {
        if (timer_prochain_spawn > 0) { timer_prochain_spawn--; }
        else {
            TypeBulle nouveau_type = BULLE_CLASSIQUE;
            if (niveau_en_cours >= 2 && rand() % 100 < 30) nouveau_type = BULLE_MULTI;
            if (niveau_en_cours >= 3 && rand() % 100 < 20) nouveau_type = BULLE_EXPLOSIVE;
            float start_x = 30 + rand() % (ecran_w - 60);
            float vit_x = (rand() % 2 == 0 ? 1 : -1) * (2.0 + (rand() % 20) / 10.0);
            creer_ballon(start_x, -40, 20 + rand() % 20, vit_x, 0, nouveau_type);
            timer_prochain_spawn = 80 + rand() % 100;
        }
    }

    if (!boss.actif && niveau_en_cours >= 1) {
        if (timer_prochain_rayon > 0) { timer_prochain_rayon--; }
        else {
            for (int i = 0; i < MAX_RAYONS; i++) {
                if (!rayons[i].actif) {
                    rayons[i].actif = 1; rayons[i].w = 80; rayons[i].x = rand() % (ecran_w - rayons[i].w);
                    rayons[i].timer_avertissement = 100; rayons[i].timer_actif = 50; break;
                }
            }
            timer_prochain_rayon = 200 + rand() % 300;
        }
    }

    for (int i = 0; i < MAX_RAYONS; i++) {
        if (rayons[i].actif) {
            if (rayons[i].timer_avertissement > 0) { rayons[i].timer_avertissement--; }
            else if (rayons[i].timer_actif > 0) {
                rayons[i].timer_actif--;
                for(int p=0; p<2; p++) {
                    if (joueurs[p].actif && !joueurs[p].est_mort &&
                        joueurs[p].x < rayons[i].x + rayons[i].w && joueurs[p].x + joueurs[p].w > rayons[i].x) {
                        etat_courant = blesser_joueur(etat_courant, p);
                        if (etat_courant == ETAT_ECRAN_DEFAITE) return etat_courant;
                    }
                }
            } else { rayons[i].actif = 0; }
        }
    }

    for (int i = 0; i < MAX_BALLONS; i++) {
        if (ballons[i].actif) {
            ballons[i].vy += 0.2;
            ballons[i].x += ballons[i].vx; ballons[i].y += ballons[i].vy;

            if (ballons[i].x - ballons[i].rayon < 0) { ballons[i].x = ballons[i].rayon; ballons[i].vx = -ballons[i].vx; }
            else if (ballons[i].x + ballons[i].rayon > ecran_w) { ballons[i].x = ecran_w - ballons[i].rayon; ballons[i].vx = -ballons[i].vx; }
            if (ballons[i].y + ballons[i].rayon > ecran_h) { ballons[i].y = ecran_h - ballons[i].rayon; ballons[i].vy = -12.0; }

            for (int j = 0; j < MAX_TIRS; j++) {
                if (tirs[j].actif && tirs[j].x < ballons[i].x + ballons[i].rayon && tirs[j].x + tirs[j].w > ballons[i].x - ballons[i].rayon &&
                    tirs[j].y < ballons[i].y + ballons[i].rayon && tirs[j].y + tirs[j].h > ballons[i].y - ballons[i].rayon) {
                    tirs[j].actif = 0; score_actuel += 150; ballons_detruits++; diviser_ballon(i); break;
                }
            }

            for(int p=0; p<2; p++) {
                if (joueurs[p].actif && !joueurs[p].est_mort &&
                    ballons[i].actif && joueurs[p].x < ballons[i].x + ballons[i].rayon && joueurs[p].x + joueurs[p].w > ballons[i].x - ballons[i].rayon &&
                    joueurs[p].y < ballons[i].y + ballons[i].rayon && joueurs[p].y + joueurs[p].h > ballons[i].y - ballons[i].rayon) {
                    etat_courant = blesser_joueur(etat_courant, p);
                    if (etat_courant == ETAT_ECRAN_DEFAITE) return etat_courant;
                }
            }
        }
    }

    if (ballons_detruits >= ballons_a_detruire) {
        ajouter_score_partie(score_actuel);
        ajouter_score(get_score_global_partie());
        set_niveau_debloque(niveau_en_cours + 1);

        // --- SAUVEGARDE AUTOMATIQUE SUR LE FICHIER ---
        sauvegarder_partie(pseudo_actuel, get_niveau_debloque(), get_score_global_partie());

        return ETAT_ECRAN_VICTOIRE;
    }

    return ETAT_JEU_EN_COURS;
}

Joueur* get_joueurs() { return joueurs; }
Tir* get_tirs() { return tirs; }
Ballon* get_ballons() { return ballons; }
RayonDanger* get_rayons() { return rayons; }
Explosion* get_explosions() { return explosions; }
Boss* get_boss() { return &boss; }
int get_score_actuel() { return score_actuel; }
int get_timer_debut() { return timer_debut; }
int get_objectif_actuel() { return ballons_detruits; }
int get_objectif_max() { return ballons_a_detruire; }
