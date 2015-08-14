#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "andy.h"

#define SIZE 'Z' - 'A'
#define LVAL 'A'
#define HVAL 'Z'

typedef struct _expected{
	char ltr;
	float freq;
}expected;

typedef struct _data{
	char* text;
	long sz;
	int* freq;
	float kpa;
}Data;

expected *alphabet;

void setup(){
	unsigned int i;

	alphabet = calloc(SIZE + 1, sizeof(expected));

	//populate letters
	for(i = LVAL; i <= HVAL; i++)
		alphabet[i - LVAL].ltr = (char) i;
	//add frequencies
	alphabet[0].freq = 0.08167;
	alphabet[1].freq = 0.01492; 
	alphabet[2].freq = 0.02782;
	alphabet[3].freq = 0.04253;
	alphabet[4].freq = 0.12702;
	alphabet[5].freq = 0.02228;
	alphabet[6].freq = 0.02015;
	alphabet[7].freq = 0.06094;
	alphabet[8].freq = 0.06966;
	alphabet[9].freq = 0.00153;
	alphabet[10].freq = 0.00772;
	alphabet[11].freq = 0.04025;
	alphabet[12].freq = 0.02406;
	alphabet[13].freq = 0.06749;
	alphabet[14].freq = 0.07507;
	alphabet[15].freq = 0.01929;
	alphabet[16].freq = 0.00095;
	alphabet[17].freq = 0.05987;
	alphabet[18].freq = 0.06327;
	alphabet[19].freq = 0.09056;
	alphabet[20].freq = 0.02758;
	alphabet[21].freq = 0.00978;
	alphabet[22].freq = 0.02361;
	alphabet[23].freq = 0.00150;
	alphabet[24].freq = 0.01974;
	alphabet[25].freq = 0.00074;

}//end setup

float chisqr(unsigned int* obs, const int len)
{
	int i;
	float output = 0.0;
	float *exp = calloc(SIZE, sizeof(float));
	error(exp, "exp", "calloc failed in chisqr.\n");

	for(i = 0; i < SIZE; i++){
		exp[i] = alphabet[i].freq * len;
	}

	for(i = 0; i < SIZE; i++){
		output += (obs[i] - exp[i]) / exp[i];
	} 
	
	free(exp);
	return output;

}//end chisqr

float kappaTest(unsigned int *freq, int len)
{
	const float nbr = 1.0 / 26.0;
	float kappa = 0.0;
	int i, sum = 0;
	const int n = (len - 1) * ((len - 1) - 1);

	//loop through and calculate the kappa for each char.
	for(i = 0; i < SIZE; i++){
		sum += (freq[i] * (freq[i] - 1));
	}//end for loop

	kappa = (float)sum / (float)n;

	kappa /= nbr;
	
	return kappa;

}//end kappaTest

int freqanalysis(char* text, unsigned int* freq){
//this module returns an array containing the total number of each char in the string sent.
	uint ch;
	int tndx;
	int len = strlen(text) - 1;

	//cycle through the text
	for(tndx = 0; tndx < len; tndx++){
		//extract each char
		ch = (unsigned int) text[tndx]- LVAL;

		//add one to that array element
		freq[ch]++;
	}//end counter loop

	return len - 1;
}//enc countChar

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

char* readFile(int fd){

	long sz;
	char* addr;

	sz = lseek(fd, 0L, SEEK_END);
	lseek(fd, 0L, SEEK_SET);

	addr = mmap(0, sz, PROT_READ, MAP_SHARED, fd, 0L);
	error(addr, "addr", "failed to mmap in readFile.\n");

	fprintf(stderr, "%s", addr);		
	return addr;

}//end readfile

int main(int argc, char const *argv[])
{
	char *text, *addr, *saveptr;
	unsigned int len = 0;
	unsigned int *freq;
	float chi, kappa;
	int fd;

	fd = open(argv[1], O_RDONLY);
	if(fd == -1){
		error((void*) NULL, "fd", "failed to open in readFile.\n");
	}

	addr = readFile(fd);

	text = calloc(100, CHARSIZE);
	error(text, "text", "failed to calloc in in main");

	freq = calloc(SIZE, FLTSIZE);
	error(freq, "freq", "failed to calloc in main.\n");

	setup();

	text = strtok_r(addr, "\n", &saveptr);
	error(text, "text", "failed to read in main.\n");

	//read line by line and count the chars in each line.
	while((text != NULL))
	{
		ucase(text);
		len += freqanalysis(text, freq);
		text = strtok_r(NULL, "\n", &saveptr);
	}//end while loop

	chi = chisqr(freq, len);

	kappa = kappaTest(freq, len);

	printf("Chi Square: %f\n", chi);

	printf("Index of Coincidence: %f\n", kappa);
	
	close(fd);
	free(text);
	free(freq);

	return 0;
}//end main


