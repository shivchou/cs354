////////////////////////////////////////////////////////////////////////////////
// Main File:        check_sudoku_board.c
// This File:        check_sudoku_board.c
// Other Files:      n/a
// Semester:         CS 354 Lecture 03, Fall 2024
// Grade Group:      gg19  (See canvas.wisc.edu/groups for your gg#)
// Instructor:       Hina Mahmood
// 
// Author:           Shivani Choudhary
// Email:            choudhary26@wisc.edu
// CS Login:         shivani
//
///////////////////////////  WORK LOG  //////////////////////////////
//  Document your work sessions on your copy http://tiny.cc/work-log
//  Download and submit a pdf of your work log for each project.
/////////////////////////// OTHER SOURCES OF HELP ////////////////////////////// 
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of 
//                   of any information you find.
// 
// AI chats:         save a transcript and submit with project.
//////////////////////////// 80 columns wide ///////////////////////////////////
// ALL C SOURCE CODE FILES MUST HAVE FILE HEADER COMMENTS

////////////////////////////////////////////////////////////////////////////////
// Copyright 2019 Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
// Used by permission, Deb Deppeler, FALL 2024
////////////////////////////////////////////////////////////////////////////////
// Fix compiler error
// Implement remaining functionality


///////////////////////////////////////////////////////////////////////////////
// Copyright 2021-24 Deb Deppeler
// Posting or sharing this file is prohibited, including any changes/additions.
//
// We have provided comments and structure for this program to help you get 
// started.  Later programs will not provide the same level of commenting,
// rather you will be expected to add same level of comments to your work.
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *DELIM = ",";  // commas ',' are a common delimiter character for data strings

/* TODO: implement this function
 * Returns 1 if and only if the 2D array of ints in board 
 * is in a valid Sudoku board state.  Otherwise returns 0.
 *
 * DOES NOT PRODUCES ANY PRINTED OUTPUT
 * 
 * A valid row or column contains only blanks or the digits 1-size, 
 * with no duplicate digits, where size is the value 1 to 9.
 * 
 * Note: This function requires only that each row and each column are valid.
 * 
 * board: heap allocated 2D array of integers 
 * size:  number of rows and columns in the board
 */
int valid_sudoku_board(int **board, int size) {
	
	if(size < 1 || size > 9)
	{
		return 0;
	}

	//rows
	for(int i = 0; i < size; i++)
	{
		for(int j = 0; j < size; j++)
		{
			for (int k = j + 1; k < size; k++) {
				if ((k != i && *(*(board + i) + j) == *(*(board + k) + j) && *(*(board + i) + j)!= 0) || *(*(board + i) + j) > size) 
				{
					return 0;
				}
			}	
		}
	}

	//columns
	for (int j = 0; j < size; j++) 
	{
		for (int i = 0; i < size; i++) 
		{
			for (int k = i + 1; k < size; k++) 
			{
				if ((k != i && *(*(board + j) + i) == *(*(board + j) + k) && *(*(board + j) + i)!= 0)  || *(*(board + i) + j) > size) 
				{
					return 0;  
				}
			}
		}
    }

	return 1;   
}    

/* COMPLETED (DO NOT EDIT)       
 * Read the first line of file to get the size of that board.
 * 
 * PRE-CONDITION #1: file exists
 * PRE-CONDITION #2: first line of file contains valid non-zero integer value
 *
 * fptr: file pointer for the board's input file
 * size: a pointer to an int to store the size
 *
 * POST-CONDITION: the integer whos address is passed in as size (int *) 
 * will now have the size (number of rows and cols) of the board being checked.
 */
void get_board_size(FILE *fptr, int *size) {      
	char *line = NULL;
	size_t len = 0;

	// 'man getline' to learn about <stdio.h> getline
	if ( getline(&line, &len, fptr) == -1 ) {
		printf("Error reading the input file.\n");
		free(line);
		exit(1);
	}

	char *size_chars = NULL;
	size_chars = strtok(line, DELIM); // 'man strtok' string tokenizer
	*size = atoi(size_chars);         // 'man atoi' alpha to integer
	free(line);                       // free memory allocated for line 
}


/* TODO: COMPLETE THE MAIN FUNCTION
 * This program prints "valid" (without quotes) if the input file contains
 * a valid state of a Sudoku puzzle board wrt to rows and columns only.
 * It prints "invalid" (without quotes) if the input file is not valid.
 *
 * Usage: A single CLA that is the name of a file that contains data.
 *
 * argc: the number of command line args (CLAs)
 * argv: the CLA strings, includes the program name
 *
 * Returns 0 if file exists and is readable.
 * Exit with any non-zero result if unable to open and read the file given.
 */
int main( int argc, char **argv ) {              

	// TODO: Check if number of command-line arguments is correct.
	if(argc != 2)
	{
		printf("enter correct number of arguments \n");
		exit(1);
	}
	// Open the file 
	FILE *fp = fopen(*(argv + 1), "r");
	if (fp == NULL) {
		printf("Can't open file for reading.\n");
		exit(1);
	}

	// will store the board's size, number of rows and columns
	int size;

	// TODO: Call get_board_size to read first line of file as the board size.
	get_board_size(fp, &size);
	// TODO: Dynamically allocate a 2D array for given board size.
	// You must dyamically create a array of pointers to other arrays of ints
       int **board = malloc((sizeof(int*))*size);
		if(board == NULL)
		{
			printf("not enough memory \n");
			if (fclose(fp) != 0) {
	         printf("Error while closing the file.\n");
	        exit(1);
	     }
			exit(1);
		}
	   for(int i = 0; i < size; i++)
	   {
			*(board+i) = malloc(size * sizeof(int));
    	}
	// Read the remaining lines of the board data file.
	// Tokenize each line and store the values in your 2D array.
	char *line = NULL;
	size_t len = 0;
	char *token = NULL;
	for (int i = 0; i < size; i++) {

		// read the line
		if (getline(&line, &len, fp) == -1) {
			printf("Error while reading line %i of the file.\n", i+2);
			exit(1);
		}

		token = strtok(line, DELIM);
		for (int j = 0; j < size; j++) {
			// TODO: Complete the line of code below
			// to initialize elements of your 2D array.
			/* ADD ARRAY ACCESS CODE HERE */
			 *(*(board + i) + j) = atoi(token); 
			token = strtok(NULL, DELIM);
		}
	}

	// TODO: Call valid_sudoku_board and print the appropriate
	//       output depending on the function's return value.
	int valid = valid_sudoku_board(board, size);
	if(valid == 1)
	{
		printf("valid \n");
	}
	else
	{
		printf("invalid \n");
	}
	// TODO: Free dynamically allocated memory.
	for (int i = 0; i < size; i++) 
	{
		free(*(board + i));
		*(board + i) = NULL;  
	}

	free(board);
	board = NULL;
	free(line);
	line = NULL;
	
	//Close the file.
	if (fclose(fp) != 0) {
		printf("Error while closing the file.\n");
		exit(1);
	} 

	return 0;       
}       

