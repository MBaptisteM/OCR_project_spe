#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "Solver.h"

#define MAX 100		// Taille maximale de la grille (100x100)

char grid[MAX][MAX];
int rows = 0, cols = 0;	// Dimensions de la grille

//loadGrid function
int loadGrid(const char *filename) {
	FILE *f = fopen(filename, "r");
	if (!f) {
		printf("Erreur ouverture fichier '%s'\n", filename);
		return 0;
	}

	char buffer[MAX + 2];	// Ligne temporaire pour lire chaque ligne du fichier
	int expected_len = -1;	// Longueur attendue des lignes (vérifie que toutes les lignes ont la même taille)

	// Lecture du fichier ligne par ligne
	while (fgets(buffer, sizeof(buffer), f)) {
		size_t len = strlen(buffer);

		// Retire le '\n' de fin de ligne si présent
		if (buffer[len - 1] == '\n') buffer[len - 1] = '\0';
		len = strlen(buffer);

		if (len == 0) continue;	// Ignore les lignes vides

		// Si c’est la première ligne, on met la longueur attendue
		if (expected_len == -1) expected_len = len;
		else if ((int)len != expected_len) {
			// Si une ligne est plus courte ou plus longue -> erreur
			printf("Ligne pas de même longeur\n");
			fclose(f);
			return 0;
		}

		// Si on dépasse la taille maximale de la grille
		if (rows >= MAX) {
			printf("Trop large\n");
			fclose(f);
			return 0;
		}

		// Vérifie que chaque caractère est une lettre (pas de chiffres ou symboles)
		for (size_t i = 0; i < len; i++) {
			if (!isalpha((unsigned char)buffer[i])) {
				printf("Error: caractère '%c' invalide\n", buffer[i]);
				fclose(f);
				return 0;
			}
			grid[rows][i] = buffer[i];	// Copie le caractère dans la grille
		}

		grid[rows][len] = '\0';
		rows++;
	}

	fclose(f);

	// Vérifie que la grille n’est pas vide
	if (rows == 0 || expected_len <= 0) {
		printf("Erreur : Grille vide ou invalide\n");
		return 0;
	}

	cols = expected_len;	// Définit le nombre de colonnes
	return 1;
}

//Liste des déplacements possibles
int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};

//searchWord function
int searchWord(const char *word, int *x0, int *y0, int *x1, int *y1) {
	int len = strlen(word);

	// Parcours de toutes les cases de la grille
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
			// Test dans les 8 directions
			for (int d = 0; d < 8; d++) {
				int nx = x;
				int ny = y;
				int k;

				// Vérifie chaque lettre du mot dans la direction choisie
				for (k = 0; k < len; k++) {
					// Si on sort de la grille, on arrête
					if (ny < 0 || ny >= rows || nx < 0 || nx >= cols) {
						break;
					}

					// Si la lettre ne correspond pas (insensible à la casse)
					if (toupper(grid[ny][nx]) != toupper(word[k])) {
						break;
					}

					// Avance dans la direction actuelle
					nx += dx[d];
					ny += dy[d];
				}

				// Si toutes les lettres ont correspondu
				if (k == len) {
					*x0 = x; *y0 = y;	// Coordonnées de départ
					*x1 = nx - dx[d]; *y1 = ny - dy[d];	// Coordonnées de fin (on revient d’un pas)
					return 1;
				}
			}
		}
	}
	return 0;
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Mauvais nombre d'args\n");
		return 1;
	}

	if (!loadGrid(argv[1])) {
		return 1;
	}

	int x0, y0, x1, y1;

	if (searchWord(argv[2], &x0, &y0, &x1, &y1))
		printf("(%d,%d)(%d,%d)\n", x0, y0, x1, y1);
	else
		printf("Mot pas trouvé dans la grille\n");

	return 0;
}