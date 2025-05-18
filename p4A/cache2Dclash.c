////////////////////////////////////////////////////////////////////////////////
// Main File: cache2Dclash.c
// This File: cache2Dclash.c
// Other Files: cache2Drows.c, cache2Dcols.c, cache1D.c
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

#define ROWS 128
#define COLS 8

int arr2D[ROWS][COLS];

int main() 
{
    for (int i = 0; i < 100; i++) 
	{
        for (int j = 0; j < 128; j += 64) 
		{
            for (int k = 0; k < 8; k++) 
			{
                arr2D[j][k] = i + j + k;
            }
        }
    }
    return 0;
}
