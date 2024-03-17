#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define HAUTEUR 1000
#define LARGEUR 1000

// Recherche du plus court chemin
/*
pour tous cases c du tableau
    faire Visite(c) = Faux
Fin
Visite(depart) = Vrai
Cout_total(depart) = 0
Cases_a_explorer = {depart}

tant que Visite(arrivee) = Faux faire
    Soit c l'élément de Cases_a_explorer tel que Cout_total(c) soit le plus petit
    Cases_a_explorer = Cases_a_explorer \{c}
    pour tous cases k voisines de c faire
        si Visite(k) = Faux alors
            Visite(k) = Vrai
            Cout_total(k) = Cout_total(c) + Tab(k)
            Cases_a_explorer = Cases_a_explore ∪ {k}
        Fin
    Fin
Fin
Afficher Cout_total(arrivee)
*/

typedef enum
{
    NORD,
    EST,
    OUEST,
    SUD
} direction;

typedef struct maillon
{
    uint64_t cout_total;
    uint32_t ligne;
    uint32_t colonne;
    struct maillon *suivant;
} maillon;

// Structure représentant une liste
typedef struct liste_s
{
    maillon *tete;
    maillon *queue;
} liste;

// Fonction pour initialiser une nouvelle liste vide
liste *initialiserListe()
{
    liste *nouvelleListe = (liste *)malloc(sizeof(liste));
    if (nouvelleListe == NULL)
    {
        fprintf(stderr, "Erreur d'allocation de mémoire pour la liste\n");
        exit(EXIT_FAILURE);
    }
    nouvelleListe->tete = NULL;
    nouvelleListe->queue = NULL;
    return nouvelleListe;
}

// Fonction pour initialiser un maillon
maillon *initialiserMaillon(uint64_t cout_total, uint32_t ligne, uint32_t colonne)
{
    maillon *nouveauMaillon = (maillon *)malloc(sizeof(maillon));
    if (nouveauMaillon == NULL)
    {
        perror("Erreur d'allocation de mémoire");
        exit(EXIT_FAILURE);
    }
    nouveauMaillon->cout_total = cout_total;
    nouveauMaillon->ligne = ligne;
    nouveauMaillon->colonne = colonne;
    nouveauMaillon->suivant = NULL;

    return nouveauMaillon;
}

// Fonction pour retirer le maillon de la tête de la liste
void retirerEnTete(liste *liste)
{
    if (liste->tete == NULL)
    {
        fprintf(stderr, "La liste est vide\n");
        exit(EXIT_FAILURE);
    }
    maillon *p = liste->tete;
    liste->tete = p->suivant;
    free(p);
}

// Fonction pour ajouter un maillon dans une liste triée par ordre croissant
void ajouterDansListeTriee(liste *liste, maillon *nouveauMaillon)
{
    if (liste->tete == NULL || nouveauMaillon->cout_total < liste->tete->cout_total)
    {
        nouveauMaillon->suivant = liste->tete;
        liste->tete = nouveauMaillon;

        if (liste->queue == NULL)
        {
            liste->queue = nouveauMaillon;
        }

        return;
    }

    maillon *courant = liste->tete;
    while (courant->suivant != NULL && courant->suivant->cout_total < nouveauMaillon->cout_total)
    {
        courant = courant->suivant;
    }

    nouveauMaillon->suivant = courant->suivant;
    courant->suivant = nouveauMaillon;

    if (nouveauMaillon->suivant == NULL)
    {
        liste->queue = nouveauMaillon;
    }
}

// Fonction pour libérer entièrement la liste de la mémoire
void libererListe(liste *liste)
{
    maillon *courant = liste->tete;
    while (courant != NULL)
    {
        maillon *suivant = courant->suivant;
        free(courant);
        courant = suivant;
    }
    free(liste);
}

// Fonction pour afficher tous les entiers dans la liste
void afficherListe(liste *liste)
{
    maillon *p = liste->tete;
    while (p != NULL)
    {
        printf("%llu ", p->cout_total);
        p = p->suivant;
    }
    printf("\n");
}

maillon *chercherMaillon(liste *file, uint32_t ligne, uint32_t colonne)
{
    maillon *courant = file->tete;
    while (courant != NULL)
    {
        if (courant->ligne == ligne && courant->colonne == colonne)
        {
            return courant; // Retourne le maillon si les coordonnées correspondent
        }
        courant = courant->suivant;
    }
    return NULL; // Retourne NULL si le maillon n'est pas trouvé
}

void printProgressBar(double percentage)
{
    int width = 50; // Largeur de la barre de progression
    int progress = percentage * width;

    printf("[");
    for (int i = 0; i < width; i++)
    {
        if (i < progress)
            printf("=");
        else
            printf(" ");
    }
    printf("] %.2f%%\r", percentage * 100);
    fflush(stdout);
}

void plusCourtChemin(uint64_t **cout, uint32_t depart_ligne, uint32_t depart_colonne, uint32_t arrivee_ligne, uint32_t arrivee_colonne)
{
    liste *file = initialiserListe();

    // Ajouter le point de départ à la file de priorité
    maillon *depart = initialiserMaillon(0, depart_ligne, depart_colonne);
    ajouterDansListeTriee(file, depart);
    uint32_t **visite = (uint32_t **)malloc(HAUTEUR * sizeof(uint32_t *));
    uint32_t **visite_inv = (uint32_t **)malloc(HAUTEUR * sizeof(uint32_t *));
    uint64_t **chemin = (uint64_t **)malloc(HAUTEUR * sizeof(uint64_t *));

    for (int i = 0; i < HAUTEUR; i++)
    {
        visite[i] = (uint32_t *)malloc(LARGEUR * sizeof(uint32_t));
        visite_inv[i] = (uint32_t *)malloc(LARGEUR * sizeof(uint32_t));
        chemin[i] = (uint64_t *)malloc(LARGEUR * sizeof(uint64_t));

        if (visite[i] == NULL || chemin[i] == NULL)
        {
            fprintf(stderr, "Memory allocation error for visite or chemin.\n");
            exit(EXIT_FAILURE);
        }
    }

    for (uint32_t i = 0; i < HAUTEUR; i++)
    {
        for (uint32_t j = 0; j < LARGEUR; j++)
        {
            visite[i][j] = 0;
            visite_inv[i][j] = 0;
            chemin[i][j] = 0;
        }
    }

    chemin[depart_ligne][depart_colonne] = 0;
    visite[depart_ligne][depart_colonne] = 1;
    visite_inv[arrivee_ligne][arrivee_colonne] = 1;

    double totalIterations = HAUTEUR * LARGEUR; // Pire cas
    double completedIterations = 0;

    printf("\n----------------------------------------------------------\n");

    while (visite[arrivee_ligne][arrivee_colonne] != 1)
    {

        maillon *courant = initialiserMaillon(file->tete->cout_total, file->tete->ligne, file->tete->colonne); // case ou le cout est minimal <=> c

        //  Retirer le maillon de la file de priorité

        retirerEnTete(file); // cases = cases \c

        // Vérifier les voisins valides
        for (int dir = NORD; dir <= SUD; dir++)
        {
            int32_t ligne = courant->ligne;
            int32_t colonne = courant->colonne;
            int32_t new;

            // ↑ → ← ↓
            switch (dir)
            {
            case NORD:
                new = ligne - 1;
                if (new < 0)
                {
                    break;
                }
                else
                {
                    if (visite[new][colonne] == 0)
                    {
                        visite[new][colonne] = 1;
                        maillon *n = initialiserMaillon(courant->cout_total + cout[new][colonne], new, colonne);
                        chemin[new][colonne] = courant->cout_total + cout[new][colonne];
                        ajouterDansListeTriee(file, n);
                    }
                }
                break;

            case EST:
                new = colonne + 1;
                if (new >= LARGEUR)
                {
                    break;
                }
                else
                {
                    if (visite[ligne][new] == 0)
                    {
                        visite[ligne][new] = 1;
                        maillon *e = initialiserMaillon(courant->cout_total + cout[ligne][new], ligne, new);
                        chemin[ligne][new] = courant->cout_total + cout[ligne][new];
                        ajouterDansListeTriee(file, e);
                    }
                }
                break;

            case OUEST:
                new = colonne - 1;
                if (new < 0)
                {
                    break;
                }
                else
                {
                    if (visite[ligne][new] == 0)
                    {
                        visite[ligne][new] = 1;
                        maillon *o = initialiserMaillon(courant->cout_total + cout[ligne][new], ligne, new);
                        chemin[ligne][new] = courant->cout_total + cout[ligne][new];
                        ajouterDansListeTriee(file, o);
                    }
                }
                break;

            case SUD:
                new = ligne + 1;
                if (new >= HAUTEUR)
                {
                    break;
                }
                else
                {
                    if (visite[new][colonne] == 0)
                    {
                        visite[new][colonne] = 1;
                        maillon *s = initialiserMaillon(courant->cout_total + cout[new][colonne], new, colonne);
                        chemin[new][colonne] = courant->cout_total + cout[new][colonne];
                        ajouterDansListeTriee(file, s);
                    }
                }
                break;
            }
        }
        free(courant);
        completedIterations++;
        printProgressBar(completedIterations / totalIterations);
    }

    maillon *arrivee = chercherMaillon(file, arrivee_ligne, arrivee_colonne);
    uint64_t result = arrivee->cout_total;
    chemin[arrivee_ligne][arrivee_colonne] = result;

    FILE *fp = fopen("cout.txt", "w");
    if (fp == NULL)
    {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier cout.txt\n");
        return;
    }

    for (uint32_t i = 0; i < HAUTEUR; i++)
    {
        for (uint32_t j = 0; j < LARGEUR; j++)
        {
            fprintf(fp, "%llu ", chemin[i][j]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);

    printf("\nLe coût total du chemin le plus court est : %llu\n", result);

    printf("\nChemin optimal :\n");

    /*uint32_t ligne = arrivee_ligne;
    uint32_t colonne = arrivee_colonne;

    printf("Départ : (%u, %u) Arrivée : (%u, %u)\n", ligne, colonne, depart_ligne, depart_colonne);

    while (ligne != depart_ligne || colonne != depart_colonne)
    {
        for (direction dir = NORD; dir <= SUD; dir++)
        {
            switch (dir)
            {

            case NORD:
                if (ligne > 0 && chemin[ligne][colonne] == chemin[ligne - 1][colonne] + (int32_t)cout[ligne][colonne] && visite_inv[ligne - 1][colonne] == 0)
                {
                    ligne -= 1;
                    visite_inv[ligne][colonne] = 1;
                    printf("↑ ");
                    fflush(NULL);
                    // printf("↑ (%u, %u) ", ligne, colonne);
                }

                break;
            case EST:
                if (colonne + 1 < LARGEUR && chemin[ligne][colonne] == chemin[ligne][colonne + 1] + (int32_t)cout[ligne][colonne] && visite_inv[ligne][colonne + 1] == 0)
                {
                    colonne += 1;
                    visite_inv[ligne][colonne] = 1;
                    printf("→ ");
                    fflush(NULL);
                    // printf("→ (%u, %u) ", ligne, colonne);
                }

                break;
            case OUEST:
                if (colonne > 0 && chemin[ligne][colonne] == chemin[ligne][colonne - 1] + (int32_t)cout[ligne][colonne] && visite_inv[ligne][colonne - 1] == 0)
                {
                    colonne -= 1;
                    visite_inv[ligne][colonne] = 1;
                    printf("← ");
                    fflush(NULL);
                    // printf("← (%u, %u) ", ligne, colonne);
                }

                break;
            case SUD:
                if (ligne + 1 < HAUTEUR && chemin[ligne][colonne] == chemin[ligne + 1][colonne] + (int32_t)cout[ligne][colonne] && visite_inv[ligne + 1][colonne] == 0)
                {
                    ligne += 1;
                    visite_inv[ligne][colonne] = 1;
                    printf("↓ ");
                    fflush(NULL);
                    // printf("↓ (%u, %u) ", ligne, colonne);
                }

                break;
            default:
                printf("Erreur traçage chemin\n");
                break;
            }
        }
    }*/
    libererListe(file);
    for (int i = 0; i < HAUTEUR; i++)
    {
        free(visite[i]);
        free(chemin[i]);
        free(visite_inv[i]);
    }

    free(visite);
    free(chemin);
    free(visite_inv);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Arguments non valides.\n");
        return 1;
    }

    srand(time(NULL));

    uint64_t **tab = (uint64_t **)malloc(HAUTEUR * sizeof(uint64_t *));
    for (int i = 0; i < HAUTEUR; i++)
    {
        tab[i] = (uint64_t *)malloc(LARGEUR * sizeof(uint64_t));
        if (tab[i] == NULL)
        {
            fprintf(stderr, "Erreur d'allocation de mémoire pour le tableau.\n");
            return 1;
        }
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL)
    {
        fprintf(stderr, "Impossible d'ouvrir le fichier.\n");
        return 1;
    }

    for (int i = 0; i < HAUTEUR; i++)
    {
        for (int j = 0; j < LARGEUR; j++)
        {
            char value[30];

            if (fscanf(file, "%29[^;];", value) != 1)
            {
                fprintf(stderr, "Erreur lors de la lecture du fichier\n");
                return 1;
            }

            char new_value[17] = "0000000000000000";

            int l = 0;
            for (uint32_t k = 0; k < strlen(value) && k < 17; k++)
            {
                if (value[k] != '.')
                {
                    new_value[l] = value[k];
                    l++;
                }
            }

            uint64_t result = strtoull(new_value, NULL, 10);
            tab[i][j] = result;

            int nextChar = fgetc(file);
            if (nextChar == '\n')
            {
                break;
            }
            else if (nextChar == EOF)
            {
                break;
            }
            else
            {
                ungetc(nextChar, file);
            }
        }
    }

    FILE *fp = fopen("mat.txt", "w");
    if (fp == NULL)
    {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier cout.txt\n");
        return 1;
    }

    for (uint32_t i = 0; i < HAUTEUR; i++)
    {
        for (uint32_t j = 0; j < LARGEUR; j++)
        {
            fprintf(fp, "%llu ", tab[i][j]);
        }
        fprintf(fp, "\n");
    }

    uint32_t x_depart = 2;
    uint32_t y_depart = 4;
    uint32_t x_arrivee = 976;
    uint32_t y_arrivee = 961;
    clock_t start, end;
    double cpu_time_used;

    if (x_depart >= 0 && x_depart < LARGEUR && y_depart >= 0 && y_depart < HAUTEUR &&
        x_arrivee >= 0 && x_arrivee < LARGEUR && y_arrivee >= 0 && y_arrivee < HAUTEUR)
    {
        printf("Coordonnées valides : \n");
        printf("Depart : (%d, %d)\n", x_depart, y_depart);
        printf("Arrivee : (%d, %d)\n", x_arrivee, y_arrivee);
        double distance = sqrt(pow((double)x_arrivee - (double)x_depart, 2) + pow((double)y_arrivee - (double)y_depart, 2));
        printf("Distance entre le départ et l'arrivée : %.2f\n", distance);
        double somme = 0.0;
        for (int i = 0; i < HAUTEUR; i++)
        {
            for (int j = 0; j < LARGEUR; j++)
            {
                somme += (double)tab[i][j];
            }
        }
        printf("Le coût moyen du tableau est : %.2f\n", somme / (double)(HAUTEUR * LARGEUR));
        start = clock();
        plusCourtChemin(tab, x_depart, y_depart, x_arrivee, y_arrivee);
        end = clock();
    }
    else
    {
        x_depart = rand() % LARGEUR;
        y_depart = rand() % HAUTEUR;
        x_arrivee = rand() % LARGEUR;
        y_arrivee = rand() % HAUTEUR;
        printf("Coordonnées invalides. De nouvelles coordonnées ont été générées.\n");
        printf("Depart : (%d, %d)\n", x_depart, y_depart);
        printf("Arrivee : (%d, %d)\n", x_arrivee, y_arrivee);
        double distance = sqrt(pow((double)x_arrivee - (double)x_depart, 2) + pow((double)y_arrivee - (double)y_depart, 2));
        printf("Distance entre le départ et l'arrivée : %.2f\n", distance);
        double somme = 0.0;
        for (int i = 0; i < HAUTEUR; i++)
        {
            for (int j = 0; j < LARGEUR; j++)
            {
                somme += (double)tab[i][j];
            }
        }
        printf("Le coût moyen du tableau est : %.2f\n", somme / (double)(HAUTEUR * LARGEUR));
        start = clock();
        plusCourtChemin(tab, x_depart, y_depart, x_arrivee, y_arrivee);
        end = clock();
    }

    fclose(file);
    fclose(fp);

    for (int i = 0; i < HAUTEUR; i++)
    {
        free(tab[i]);
    }
    free(tab);

    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("\nCPU time used: %f seconds\n", cpu_time_used);

    return 0;
}
