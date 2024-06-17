
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Défines et Constantes
#define N 4
#define TAILLE (N * N)

// Définition de nouveau type
typedef struct {
  int valeur;
  int candidats[TAILLE];
  int nbCandidats;
} tCase;

typedef tCase tGrille[TAILLE][TAILLE];

// Appel des procédure
void afficherGrille(tGrille grille);
int chargerGrille(tGrille grille);
void initialiserCandidats(tGrille grille);
bool possible(tGrille g, int numlig, int numcol, int valeursaisie);
void ajouterCandidat(tCase *laCase, int val);
bool backtracking(tGrille grille, int numeroCase);

// Programme principal
int main() {
  // Déclaration des variables
  tGrille g; 
  bool progression;
  
  // Initialisation des variables
  chargerGrille(g);
  initialiserCandidats(g);
  progression = true;

  // traitement des données
  afficherGrille(g);

  clock_t begin = clock();
  //for (int numCase = 0; numCase < TAILLE; numCase++){
  progression = backtracking(g, 0);
  //}
  //initialiserCandidats(g);
  // autres techniques...

  // Affichage des résultats
  clock_t end = clock();
  afficherGrille(g);
  
  double tmpsCPU = (end - begin) * 1.0 / CLOCKS_PER_SEC;
  printf("Résolution de la grille en : %.6f secondes\n", tmpsCPU);
  return EXIT_SUCCESS;
}

// Définition des fonctions
int chargerGrille(tGrille grille) {
  FILE *f;
  int val;
  int nbCasesVides;
  char nomFichier[30];
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
void initialiserCandidats(tGrille grille) {
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
        }
      }
    }
  }
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
    laCase->candidats[laCase->nbCandidats] = val;
    laCase->nbCandidats = laCase->nbCandidats + 1;
  }
}

//---------------Technique---------------
bool backtracking(tGrille grille, int numeroCase){
  int ligne;
  int colonne;
  bool resultat;

  resultat = false;
  if (numeroCase == TAILLE * TAILLE){
    //on a traité toutes les cases, la grille est résolue
    resultat = true;
  }
  else{
    // On récupère les "coordonnées" de la case
    ligne = numeroCase / TAILLE;
    colonne = numeroCase % TAILLE;
    if (grille[ligne][colonne].valeur != 0){
    // Si la case n’est pas vide, on passe à la suivante
    // (appel récursif)
    resultat = backtracking(grille, numeroCase+1);
    }
    else{
      for(int valeur = 1 ; valeur <= TAILLE ; valeur++){
        if(possible(grille, ligne, colonne, valeur)){
          // Si la valeur est autorisée on l'inscrit
          //dans la case...
          grille[ligne][colonne].valeur = valeur;
          // ... et on passe à la suivante : appel récursif
          // pour voir si ce choix est bon par la suite
          if(backtracking(grille,numeroCase+1) == true ){
            resultat = true;
          }
          else{
            grille[ligne][colonne].valeur = 0;
          }
        }
      }
    }
  }
  return resultat;
}
//---------------Affichage---------------
void afficherGrille(tGrille grille) {
  int indicelig, indicecol;
  printf("   ");
  for (indicelig = 1; indicelig <= TAILLE; indicelig++) {
    if ((indicelig > 1) && (indicelig % N == 1)) {
      printf("%5d", indicelig);
    } else {
      printf("%4d", indicelig);
    }
  }
  printf("\n");
  printf(
      "   +----------------+----------------+----------------+----------------+");
  printf("\n");
  for (indicelig = 0; indicelig < TAILLE; indicelig++) {
    if ((indicelig > 1) && (indicelig % N == 0)) {
      printf(
          "   "
          "+----------------+----------------+----------------+----------------+");
      printf("\n");
    }
    if (indicelig < 9) {
      printf("%d  ", indicelig + 1);
    } else {
      printf("%d ", indicelig + 1);
    }
    for (indicecol = 0; indicecol < TAILLE; indicecol++) {
      if (indicecol % N == 0) {
        printf("|");
      }
      if (grille[indicelig][indicecol].valeur == 0) {
        printf("  . ");
      } else {
        printf(" %2d ", grille[indicelig][indicecol].valeur);
      }
    }

    printf("|\n");
  }
  printf(
      "   +----------------+----------------+----------------+----------------+");
  printf("\n");
}