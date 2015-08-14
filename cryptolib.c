#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include "andy.h"

#define SIZE 256

int* freqanalysis(char* text){
//this module returns an array containing the total number of each char in the string sent.
	int* array;
	uint ch;
	int tndx;
	int len = strlen(text);

	//define array size
	array = calloc(SIZE, INTSIZE);

	//cycle through the text
	for(tndx = 0; tndx < len; tndx++){
		//extract each char
		ch = (uint) text[tndx];

		//add one to that array element
		array[ch]++;
	}//end counter loop

	return array;
}//enc countChar

char* rempunc(char* text){
//this module removes all non-alpha
//charactors in the string.

	char* new;
	int i = 0, j = 0;
	int len = strlen(text);

	new = calloc(len, CHARSIZE);

	//cycle through the text
	for(i = 0; i < len; i++){

		//check if is alpha
		if(isalpha(text[i])){
			strncpy(&new[j], &text[i], 1);
			j++;
		}//end if isalpha

	}//end copy loop

	//cut the string short at the end
	new[j] = '\0';

	return new;
}//end rempunc

void ucase(char* text){
//this module makes the line uppercase

	int i;
	int len = strlen(text);

	//cycle through and make uppercase the lower case letters
	for(i = 0; i < len; i++){
		if(islower(text[i]) && isalpha(text[i]))
		{
			text[i] += ('A' - 'a');
		}//end check if lower
	}//end to upper loop

}//end ucase

char* wheelgen(int len){
//creates a random string between 'A' and 'Z' of length len.

	char* wheel = malloc(len);
	int i;

	srand(time(NULL));

	//generate random numbers and assign them to each element
	for(i = 0; i < len; i++){
		wheel[i] = 'A' + (rand() % 26);
	}//end generate loop

	wheel[i] = '\0';

	return wheel;

}//end wheelgen

void randfile(char* filename, int len, int lines){
/*
	Open and write to a file, filename
	that contains random letters 
	betwwen 'A' to 'Z'.
*/

	FILE* fd;
	int i, j;
	char* wheel = malloc(len);

	fd = fopen(filename, "w");

	srand(time(NULL));

	//generate one random string of length len.
	for(i = 0; i < lines; i++){

		//generate random numbers and assign them to each element
		for(j = 0; j < len; j++){
			wheel[j] = 'A' + (rand() % 26);
		}//end generate loop

		wheel[j] = '\0';
		
		fwrite(wheel, len, CHARSIZE, fd);
		fwrite("\n", 1, CHARSIZE, fd);
	}//end generate and write loop

	fclose(fd);
	free(wheel);
	
}//end randFile

char* finitewheelgen(){
//creates a random string between 'A' and 'Z' without repitiion.

	char* wheel = malloc(26);
	char ch;
	int i = 0, j = 0;
	bool exists = FALSE;

	srand(time(NULL));

	//generate random numbers and assign them to each element
	while(i < 26){
		ch = 'A' + (rand() % 26);

		//check if charactor already exists
		for(j = 0; j < i; j++){
			if(wheel[j] == ch)
				exists = TRUE;
		}//end check loop

		//if unique, add the char to the string
		if(!exists){
			wheel[i] = ch;
			i++;
		}//end if

		exists = FALSE;

	}//end generate loop

	wheel[i] = '\0';

	return wheel;

}//end finitewheelgen

void finiterandfile(char* filename, int lines){
//creates a random string between 'A' and 'Z' without repitiion.

	FILE* fd;
	char* wheel = malloc(26);
	char ch;
	int i, j, k;
	bool exists = FALSE;

	srand(time(NULL));
	fd = fopen(filename, "w");

	//write a one line of text until all the lines are filled
	for(k = 0; k < lines; k++){

		i = 0;
		
		//generate random numbers and assign them to each element
		while(i < 26){
			ch = 'A' + (rand() % 26);

			//check if charactor already exists
			for(j = 0; j < i; j++){
				if(wheel[j] == ch)
					exists = TRUE;
			}//end check loop

			//if unique, add the char to the string
			if(!exists){
				wheel[i] = ch;
				i++;
			}//end if

			exists = FALSE;

		}//end generate loop

		wheel[i] = '\0';

		fwrite(wheel, 26, CHARSIZE, fd);
		fwrite("\n", 1, CHARSIZE, fd);

	}//end write line loop

	fclose(fd);
	free(wheel);

}//end finiterandfile
