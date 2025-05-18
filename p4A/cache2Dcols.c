////////////////////////////////////////////////////////////////////////////////
// Main File: cache2Dcols.c
// This File: cache2Dcols.c
// Other Files: cache2Drows.c, cache1D.c, cache2Dclash.c
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
    for (int j = 0; j < COLS; j++) 
	{
        for (int i = 0; i < ROWS; i++) 
		{
            arr2D[i][j] = i + j;
        }
    }
    return 0;
}

