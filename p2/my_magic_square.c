////////////////////////////////////////////////////////////////////////////////
// Main File: my_magic_square.c
// This File: my_magic_sqaure.c
// Other Files: n/a
// Semester: CS 354 Lecture 03, Fall 2024
// Grade Group: gg19 (See canvas.wisc.edu/groups for your gg#)
// Instructor: Hina Mahmood
//
// Author: Shivani Choudhary
// Email: choudhary26@wisc.edu
// CS Login: shivani
//
/////////////////////////// WORK LOG //////////////////////////////
// Document your work sessions on your copy http://tiny.cc/work-log
// Download and submit a pdf of your work log for each project.
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
// Persons: Identify persons by name, relationship to you, and email.
// Describe in detail the the ideas and help they provided.
//
// Online sources: avoid web searches to solve your problems, but if you do
// search, be sure to include Web URLs and description of
// of any information you find.
//
// AI chats: save a transcript and submit with project.
//////////////////////////// 80 columns wide ///////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Copyright 2020 Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
// Used by permission, CS 354 Fall 2024, Deb Deppeler
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure that represents a magic square
typedef struct {
	int size;           // dimension of the square
	int **magic_square; // 2D heap array that stores magic square
} MagicSquare;

/* TODO:
 * Prompts the user for the magic square's size, read size,
 * check if it's an odd number >= 3 
 * (if not valid size, display the required error message and exit)
 *
 * return the valid number
 */
int getSize() {
	int size;

    // Prompt the user for the magic square's size
    printf("Enter the size of the magic square (odd number >= 3): ");

    // Read input using scanf and check for invalid inputs
    if (scanf("%d", &size) != 1) {
        fprintf(stderr, "Error: Invalid input. Please enter a number.\n");
        exit(1);  // Exit on invalid input
    }

	 if(size % 2 == 0)
     {
	    printf("Magic square size must be odd. \n");
        exit(1);
     }   

	else if(size < 3)
	{
		printf("Magic square size must be >= 3. \n");
		exit(1);
	}

	return size;   
} 

/* TODO:
 * Creates a magic square of size n on the heap.
 *
 * May use the Siamese magic square algorithm or alternate
 * valid algorithm that produces a magic square. 
 *
 * n - the number of rows and columns
 *
 * returns a pointer to the completed MagicSquare struct.
 */
MagicSquare *generateMagicSquare(int n) {
	MagicSquare *magic = malloc(sizeof(MagicSquare));
	int **square = calloc(n, sizeof(int));
	for(int i = 0; i < n; i++)
	{
		*(square + i) = calloc(n, sizeof(int));
	}
	int row = n / 2;
    int col = n - 1;

    //alternate siamese method: down-right instead of diagonally up
    for (int num = 1; num <= n * n; num++) 
	{
        *(*(square + row) + col) = num;

        int newRow = (row + 1) % n;      // Move down, wrap around if necessary
        int newCol = (col + 1) % n;      // Move right, wrap around if necessary

        //if the new cell is already filled, move up instead of down-right
        if (*(*(square + newRow) + newCol) != 0) 
		{
            col = (col - 1 + n) % n;     //move left
        } 
		else
		{
            row = newRow;
            col = newCol;
        }
    }
	magic->size = n;
    magic->magic_square = square;
	return magic;    
} 

/* TODO:  
 * Open a new file (or overwrites the existing file)
 * and write the magic square values to the file
 * in the format specified by assignment.
 *
 * See assigntment for required file format.
 *
 * magic_square - the magic square to write to a file
 * filename - the name of the output file
 */
void fileOutputMagicSquare(MagicSquare *magic_square, char *filename)
{
// Open the file for writing (overwrite if it exists)
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file %s for writing.\n", filename);
        if(fclose(file) == EOF)
		{
			printf("problem closing file \n");
		}
		exit(1);
    }

    fprintf(file, "%d\n", magic_square->size);

    // Write each row of the magic square
    for (int i = 0; i < magic_square->size; i++) {
        for (int j = 0; j < magic_square->size; j++) {
            fprintf(file, "%d", *(*(magic_square->magic_square + i) + j));
            if (j < magic_square->size - 1) {
                fprintf(file, " "); 
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);	
}


/* TODO:
 * Call other functions to generate a magic square 
 * of the user specified size and outputs 
 * the created square to the output filename.
 * 
 * Add description of required CLAs here
 */
int main(int argc, char **argv) {
	// TODO: Check input arguments to get output filename

	// TODO: Get magic square's size from user

	// TODO: Generate the magic square by correctly interpreting 
	//       the algorithm(s) in the write-up or by writing or your own.  
	//       You must confirm that your program produces a 
	//       Magic Square. See description in the linked Wikipedia page.

	// TODO: Output the magic square
	if(argc != 2)
	{
		printf("Usage: ./my_magic_square <output_filename> \n");
		exit(1);
	}

	int size = getSize();

	MagicSquare *magic = generateMagicSquare(size);

	fileOutputMagicSquare(magic, argv[1]);
	
	int **square = magic->magic_square;

    for (int i = 0; i < size; i++)
	 {
        free(*(square + i));  
		*(square + i) = NULL;
	 }
    free(square);  
    square = NULL;
	magic->magic_square = NULL;
	free(magic);
	magic = NULL;
	return 0;
} 

// 202409


