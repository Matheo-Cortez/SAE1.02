/*
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Défines et Constantes
#define N 3
#define TAILLE (N * N)

// Définition de nouveau type
typedef struct {
  int valeur;
  bool candidats[TAILLE + 1]; // la case d’indice 0 est neutralisée
  int nbCandidats;
} tCase;

typedef tCase tGrille[TAILLE][TAILLE];

// Appel des procédure
void afficherGrille(tGrille grille);
void afficherStats(int nbCasesVides, int nbCasesVidesInitial,
                   int nbCandidatsElimines, int nbCandidatsInitial,
                   char nomFichier[30]);
int chargerGrille(tGrille grille, char *nomFichier);
int initialiserCandidats(tGrille grille);
bool possible(tGrille g, int numlig, int numcol, int valeursaisie);
void ajouterCandidat(tCase *laCase, int val);
int singletonNu(tGrille grille, bool *avancer);
int singletonCache(tGrille grille, bool *avancer);

// Programme principal
int main() {
  // Déclaration des variables
  tGrille g;
  char nomFichier[30];
  bool progression;
  int nbCasesVidesInitial;
  int nbCasesVidesRestent;
  int nbCandidatsInitial;
  int nbCandidatsRestent;

  // Initialisation des variables
  nbCasesVidesInitial = chargerGrille(g, &nomFichier);
  nbCasesVidesRestent = nbCasesVidesInitial;
  nbCandidatsInitial = initialiserCandidats(g);
  progression = true;

  // traitement des données
  afficherGrille(g);
  clock_t begin = clock();
  while (nbCasesVidesRestent != 0 && progression) {
    progression = false;

    // technique du singleton nu
    nbCasesVidesRestent = nbCasesVidesRestent - singletonNu(g, &progression);
    nbCandidatsRestent = initialiserCandidats(g);
    // technique du singleton caché
    nbCasesVidesRestent = nbCasesVidesRestent - singletonCache(g, &progression);
    nbCandidatsRestent = initialiserCandidats(g);
    // autres techniques...
  }

  // Affichage des résultats
  afficherGrille(g);
  clock_t end = clock();
  double tmpsCPU = (end - begin) * 1.0 / CLOCKS_PER_SEC;
  printf("Temps CPU = %.6f secondes\n", tmpsCPU);
  afficherStats(nbCasesVidesRestent, nbCasesVidesInitial, nbCandidatsRestent,
                nbCandidatsInitial, nomFichier);

  return EXIT_SUCCESS;
}

// Définition des fonctions
int chargerGrille(tGrille grille, char *nomFichier) {
  FILE *f;
  int val;
  int nbCasesVides;
  nbCasesVides = 0;
  printf("Nom du fichier ? ");
  scanf("%s", nomFichier);
  f = fopen(nomFichier, "rb");
  if (f == NULL) {
    printf("\n ERREUR sur le fichier %s\n", nomFichier);
  } else {
    for (int lig = 0; lig < TAILLE; lig++) {
      for (int col = 0; col < TAILLE; col++) {
        fread(&val, sizeof(int), 1, f);
        grille[lig][col].valeur = val;
        if (val == 0) {
          nbCasesVides++;
        }
      }
    }
    fclose(f);
  }
  return nbCasesVides;
}

//---------------Implémentation Candidats---------------
int initialiserCandidats(tGrille grille) {
  // Déclaration des variables
  int lig, col, val;
  int nbCandidatsTrouves; // conteur de candidats trouvés. Sert à la fin pour
                          // calculé le nombre de candidats éliminés
  nbCandidatsTrouves = 0;
  // Initialisation des candidats
  for (lig = 0; lig < TAILLE; lig++) {
    for (col = 0; col < TAILLE; col++) {
      if (grille[lig][col].valeur == 0) {
        grille[lig][col].nbCandidats = 0;
        for (val = 1; val <= TAILLE; val++) {
          if (possible(grille, lig, col, val)) {
            ajouterCandidat(&grille[lig][col], val);
            nbCandidatsTrouves = nbCandidatsTrouves + 1;
          }
          else{
            grille[lig][col].candidats[val]=false;
          }
        }
      }
    }
  }
  return nbCandidatsTrouves;
}

bool possible(tGrille g, int numlig, int numcol, int valeursaisie) {
  // Déclaration variable
  int indicelig, indicecol;
  int ligTempo, colTempo;
  bool possible;
  // Initialisation variable
  possible = true;
  indicelig = 0;
  indicecol = 0;
  ligTempo = numlig;
  colTempo = numcol;
  // Test dans les ligne et les colonne
  while ((indicelig < TAILLE) && (indicecol < TAILLE) && possible) {
    // test ligne
    if (valeursaisie == g[numlig][indicecol].valeur) {
      possible = false;
    }
    // test colonne
    else if (valeursaisie == g[indicelig][numcol].valeur) {
      possible = false;
    }

    indicelig = indicelig + 1;
    indicecol = indicecol + 1;
  }
  // Test dans une blocks
  if (possible != false) {
    while (ligTempo % N != 0) {
      ligTempo = ligTempo - 1;
    }
    while (colTempo % N != 0) {
      colTempo = colTempo - 1;
    }
    do {
      do {
        if (valeursaisie == g[ligTempo][colTempo].valeur) {
          possible = false;
        }
        colTempo = colTempo + 1;
      } while (colTempo % N != 0);
      colTempo = colTempo - N;
      ligTempo = ligTempo + 1;
    } while (ligTempo % N != 0);
  }
  return possible;
}

void ajouterCandidat(tCase *laCase, int val) {
  if (laCase->nbCandidats < TAILLE) {
    laCase->candidats[val] = true;
    laCase->nbCandidats++;
  }
}

//---------------Technique---------------
int singletonNu(tGrille grille, bool *avancer) {
  // Déclaration des variables
  int lig, col;
  int casesRemplies;
  int indCandidat = 0;
  casesRemplies = 0;
  // Traitement
  for (lig = 0; lig < TAILLE; lig++) {
    for (col = 0; col < TAILLE; col++) {
      if (grille[lig][col].valeur == 0 && grille[lig][col].nbCandidats == 1) {
        indCandidat = 0;
        while(indCandidat<=TAILLE &&
grille[lig][col].candidats[indCandidat]!=true) { indCandidat++;
        }
        if(indCandidat<=TAILLE &&
grille[lig][col].candidats[indCandidat]==true){ grille[lig][col].valeur =
indCandidat; *avancer = true; casesRemplies = casesRemplies + 1;
        }
      }
    }
  }
  return casesRemplies;
}

int singletonCache(tGrille grille, bool *avancer) {
  // Déclaration des variables
  int nbCandPoubelle;
  int lig, col, val;
  int indLigTempo, indColtempo; // Stock la colonne et la ligne où on a trouvé
  int casesRemplies;
  int apparait; // compte le nombre de fois où apparait le candidat dans la
                // ligne ou la colonne

  casesRemplies = 0;
  indColtempo = 0; // Surement la où il y a un problème
  indLigTempo = 0;
  apparait = 0;
  // Traitement des lignes
  // Test toutes les valeurs d'une ligne avant de passer à la ligne suivante
  for (lig = 0; lig < TAILLE; lig++) {
    for (val = 1; val <= TAILLE; val++) {
      indColtempo = 0;
      indLigTempo = 0;
      col = 0;      // Reset de la colonne
      apparait = 0; // Reset

      while (col < TAILLE && apparait < 2) {
        if (grille[lig][col].valeur == 0 && grille[lig][col].candidats[val] ==
true) {

          // fois le candidat dans la ligne
          if (apparait == 1 && col != indColtempo) {
            apparait = apparait + 1; // sort de toutes les boucles
          }
          // Si c'est la 1ère fois le candidat alors on enregistre la
          // colonne et la ligne
          if (apparait == 0) {
            indLigTempo = lig;
            indColtempo = col;
            apparait = apparait + 1;
          }
        }
        col++;
      }
      // Si a la fin de la ligne on a trouvé un candidat unique alors on le met
dans la case if (apparait == 1 && indColtempo <= TAILLE) {
        grille[indLigTempo][indColtempo].valeur = val;
        *avancer = true;                   // Pour la boucle du main
        casesRemplies = casesRemplies + 1; // Pour le nombre de cases remplies
      }
    }
  }

  nbCandPoubelle = initialiserCandidats(grille);
  apparait = 0;

  // Traitement des colonnes
  for (col = 0; col < TAILLE; col++) {
    for (val = 1; val <= TAILLE; val++) {
      indColtempo = 0;
      indLigTempo = 0;
      lig = 0;      // Reset de la colonne
      apparait = 0; // Reset

      while (lig < TAILLE && apparait < 2) {
        if (grille[lig][col].valeur == 0 && grille[lig][col].candidats[val] ==
true) {
          // fois le candidat dans la ligne
          if (apparait == 1 && lig != indLigTempo) {
            apparait = apparait + 1; // sort de toutes les boucles
          }
          // Si c'est la 1ère fois le candidat alors on enregistre la
          // colonne et la ligne
          if (apparait == 0) {
            indLigTempo = lig;
            indColtempo = col;
            apparait = apparait + 1;
          }
        }
        lig++;
      }
      // Si a la fin de la ligne on a trouvé un candidat unique alors on le
metndans la case if (apparait == 1 && indLigTempo <= TAILLE) {
        grille[indLigTempo][indColtempo].valeur = val;
        *avancer = true;                   // Pour la boucle du main
        casesRemplies = casesRemplies + 1; // Pour le nombre de cases remplies
      }
    }
  }

  nbCandPoubelle = initialiserCandidats(grille);
  apparait = 0;

  // Traitement des blocs
  for (int ligBloc = 0; ligBloc < TAILLE; ligBloc += N) { // boucle pour les
lignes du bloc for (int colBloc = 0; colBloc < TAILLE; colBloc += N) { // boucle
pour les colonnes du bloc for (val = 1; val <= TAILLE; val++) { apparait = 0; //
reset indLigTempo = 0; // pour stocker la ligne du bloc indColtempo = 0; // pour
stocker la colonne du bloc for (lig = ligBloc; lig < ligBloc + N; lig++) { //
Parcourt le bloc pour chercher la valeur for (col = colBloc; col < colBloc + N;
col++) { // Pareil if (grille[lig][col].valeur == 0) { if
(grille[lig][col].candidats[val] == true) {             // si le candidat est
égal à la valeur if (apparait == 1 && (col != indColtempo || lig !=
indLigTempo)) { apparait = apparait + 1; // sort de toutes les boucles
                }
                if (apparait == 0) { // si c'est la 1ère fois qu'il apaprait
                  indLigTempo = lig;    // enregistre la ligne
                  indColtempo = col; // enregistre la colonne
                  apparait = apparait + 1; // sort de la boucle
                }
              }
            }
          }
        }
        if (apparait == 1 && indLigTempo < ligBloc + N && indColtempo < colBloc
+ N) {  // Si la valeur n'apparaît qu'une seule fois dans le bloc, la placer
          grille[indLigTempo][indColtempo].valeur = val;
          *avancer = true;
          casesRemplies++;
        }
      }
    }
  }
  return casesRemplies;
}

//---------------Affichage---------------
void afficherGrille(tGrille grille) {
  int indicelig, indicecol;
  printf("   ");
  for (indicelig = 1; indicelig <= TAILLE; indicelig++) {
    if ((indicelig > 1) && (indicelig % N == 1)) {
      printf("%4d", indicelig);
    } else {
      printf("%3d", indicelig);
    }
  }
  printf("\n");
  printf("   +---------+---------+---------+");
  printf("\n");
  for (indicelig = 0; indicelig < TAILLE; indicelig++) {
    if ((indicelig > 1) && (indicelig % N == 0)) {
      printf("   +---------+---------+---------+");
      printf("\n");
    }
    printf("%d  ", indicelig + 1);
    for (indicecol = 0; indicecol < TAILLE; indicecol++) {
      if (indicecol % N == 0) {
        printf("|");
      }
      if (grille[indicelig][indicecol].valeur == 0) {
        printf(" . ");
      } else {
        printf(" %d ", grille[indicelig][indicecol].valeur);
      }
    }

    printf("|\n");
  }
  printf("   +---------+---------+---------+");
  printf("\n");
}

void afficherStats(int nbCasesVidesFinale, int nbCasesVidesInitial, int
nbCandidatsFinale, int nbCandidatsInitial, char nomFichier[30]) {

  float nbCasesRemplis = nbCasesVidesInitial - nbCasesVidesFinale;
  float nbCandidatsElimines = nbCandidatsInitial - nbCandidatsFinale;

  float tauxRemplis = (nbCasesRemplis / nbCasesVidesInitial) * 100.0;
  float tauxCandidats = (nbCandidatsElimines / nbCandidatsInitial) * 100.0;

  printf("******   RESULTATS POUR %s   ******\n", nomFichier);
  printf("Nombre de cases remplies = %.0f sur %d\t", nbCasesRemplis,
         nbCasesVidesInitial);
  printf("Taux de remplissage = %.3f%%\n", tauxRemplis);
  printf("Nombre de candidats eliminés = %.0f\t", nbCandidatsElimines);
  printf("Pourcentage d'élimination = %.3f%%\n", tauxCandidats);
}*/