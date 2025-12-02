#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "Solver.h"

#define MAX 100		// Maximum grid size (100x100)

char grid[MAX][MAX];
int rows = 0, cols = 0;	// Grid dimensions

//loadGrid function
int loadGrid(const char *filename) {
	FILE *f = fopen(filename, "r");
	if (!f) {
		printf("Error opening file '%s'\n", filename);
		return 0;
	}

	char buffer[MAX + 2];	// Temporary line buffer to read each line from the file
	int expected_len = -1;	// Expected line length (checks that all lines have the same size)

	// Read the file line by line
	while (fgets(buffer, sizeof(buffer), f)) {
		size_t len = strlen(buffer);

		// Remove the '\n' at the end of the line if present
		if (buffer[len - 1] == '\n') buffer[len - 1] = '\0';
		len = strlen(buffer);

		if (len == 0) continue;	// Ignore empty lines

		// If this is the first line, set the expected length
		if (expected_len == -1) expected_len = len;
		else if ((int)len != expected_len) {
			// If a line is shorter or longer -> error
			printf("Line length mismatch\n");
			fclose(f);
			return 0;
		}

		// If we exceed the maximum grid size
		if (rows >= MAX) {
			printf("Grid too large\n");
			fclose(f);
			return 0;
		}

		// Check that each character is a letter (no digits or symbols)
		for (size_t i = 0; i < len; i++) {
			if (!isalpha((unsigned char)buffer[i])) {
				printf("Error: invalid character '%c'\n", buffer[i]);
				fclose(f);
				return 0;
			}
			grid[rows][i] = buffer[i];	// Copy the character into the grid
		}

		grid[rows][len] = '\0';
		rows++;
	}

	fclose(f);

	// Check that the grid is not empty
	if (rows == 0 || expected_len <= 0) {
		printf("Error: Empty or invalid grid\n");
		return 0;
	}

	cols = expected_len;	// Set the number of columns
	return 1;
}

//List of possible moves
int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};

//searchWord function
int searchWord(const char *word, int *x0, int *y0, int *x1, int *y1) {
	int len = strlen(word);

	// Traverse all cells of the grid
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
			// Test in 8 directions
			for (int d = 0; d < 8; d++) {
				int nx = x;
				int ny = y;
				int k;

				// Check each letter of the word in the chosen direction
				for (k = 0; k < len; k++) {
					// If we go out of the grid, stop
					if (ny < 0 || ny >= rows || nx < 0 || nx >= cols) {
						break;
					}

					// If the letter does not match (case insensitive)
					if (toupper(grid[ny][nx]) != toupper(word[k])) {
						break;
					}

					// Move forward in the current direction
					nx += dx[d];
					ny += dy[d];
				}

				// If all letters matched
				if (k == len) {
					*x0 = x; *y0 = y;	// Starting coordinates
					*x1 = nx - dx[d]; *y1 = ny - dy[d];	// Ending coordinates (step back one)
					return 1;
				}
			}
		}
	}
	return 0;
}

//was a main
int solve(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Incorrect number of arguments\n");
		return 1;
	}

	if (!loadGrid(argv[1])) {
		return 1;
	}

	int x0, y0, x1, y1;

	if (searchWord(argv[2], &x0, &y0, &x1, &y1))
		printf("(%d,%d)(%d,%d)\n", x0, y0, x1, y1);
	else
		printf("Word not found in the grid\n");

	return 0;
}