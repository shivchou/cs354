////////////////////////////////////////////////////////////////////////////////
// Main File: cache2Drows.c
// This File: cache2Drows.c
// Other Files: cache1D.c, cache2Dcols.c, cache2Dclash.c
// Semester: CS 354 Lecture 03, Fall 2024
// Grade Group: gg19 (See canvas.wisc.edu/groups for your gg#)
// Instructor: Hina Mahmood
//
// Author: Shivani Choudhary
// Email: choudhary26@wisc.edu
// CS Login: shivani
//
/////////////////////////// WORK LOG ////////////////////////////


#include <stdio.h>

#define ROWS 3000
#define COLS 500

int arr2D[ROWS][COLS];

int main() 
{
    for (int i = 0; i < ROWS; i++) 
	{
        for (int j = 0; j < COLS; j++) 
		{
            arr2D[i][j] = i + j;
        }
    }
    return 0;
}

