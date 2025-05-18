////////////////////////////////////////////////////////////////////////////////
// Main File: cache1D.c
// This File: cache1D.c
// Other Files: cache2Drows.c, cache2Dcols.c, cache2Dclash.c
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

int arr[100000];

int main() 
{
    for (int i = 0; i < 100000; i++)
	{
        arr[i] = i;
    }

    return 0;
}
