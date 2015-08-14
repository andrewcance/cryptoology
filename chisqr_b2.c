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
#define SHIFT(a, b) ((a + b) % 26)

typedef struct _expected{
	char ltr;
	float freq;
}expected;

typedef struct _FileData{
	char* text;
	long sz;
	unsigned int* freq;
	float kpa;
}FileData;

typedef struct _Str{
	char* text;
	long sz;
	unsigned int *freq;
	unsigned int shift;
	float chi;
}Str;

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

float chisqr(unsigned int* obs, const int len, int shift)
{
	int i;
	float output = 0.0;
	float *exp = calloc(SIZE, sizeof(float));
	error(exp, "exp", "calloc failed in chisqr.\n");

	//prep alphabet for use in eqution
	for(i = 0; i < SIZE; i++){
		exp[i] = alphabet[i].freq * len;
	}

	//calculate chi square
	for(i = 0; i <= SIZE; i++){
		//printf("obs[%d]: %d, exp[%d]: %f.\n", i, obs[i], SHIFT(i, shift), exp[SHIFT(i, shift)]);
		output += (obs[i] - exp[SHIFT(i, shift)]) / exp[SHIFT(i, shift)];
	}//end chi square loop

	printf("Shift [%d]: %f\n", shift, output);

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

int freqanalysis(FileData* file){
//this module returns an array containing the total number of each char in the string sent.
	uint ch;
	long tndx;
	long len = 0;

	//cycle through the text
	for(tndx = 0; tndx <= file->sz; tndx++){

		//extract each char
		ch = (unsigned int) file->text[tndx] - LVAL;

		//check if is alpha
		if(ch >= 0 && ch < 26)
		{
			//add one to that array element
			file->freq[ch]++;
			len++;
		}//end check if is alpha

	}//end counter loop

	return len;
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

FileData* readFile(int fd){

	long sz;
	char* addr;
	FileData* file = calloc(1, sizeof(FileData));
	error(file, "file","failed to calloc in readfile.\n");

	//initialize the frequency count array
	file->freq = calloc(SIZE, FLTSIZE);
	error(file->freq, "freq", "failed to calloc in readfile.\n");

	sz = lseek(fd, 0L, SEEK_END);
	lseek(fd, 0L, SEEK_SET);

	addr = mmap(0, sz, PROT_READ, MAP_SHARED, fd, 0L);
	error(addr, "addr", "failed to mmap in readFile.\n");

	//put FileData into struct to return to user
	file->text = addr;
	file->sz = sz;

	return file;

}//end readfile

Str* shiftTxt(FileData* file, unsigned int len){
/*
	Test each ceaser shift and find 
	the lowest chi square value.
*/

	Str* csr;
	int i, shift = 0;
	float best = 10000.0;
	unsigned int chi;

	csr = calloc(1, file->sz);
	error(csr, "csr", "failed to calloc in shiftTxt.\n");

	csr->freq = calloc(1, sizeof(file->freq));
	error(csr->freq,"csr.freq","failed to calloc in shiftText.\n");

	//cycle through each shift and find the smallest chisqr
	for(i = 0; i < 26; i++){
		chi = chisqr(file->freq, len, i);

		//see if the chisqr is less than the current best shift
		if(chi < best){
			best = chi;
			shift = i;
		}//end check loop

	}//end for loop

	//copy data from file to decrypt
	csr->text = strdup(file->text);
	csr->sz = file->sz;
	csr->chi = chi;
	csr->shift = shift;

	//cycle through each char and shift the letters.
	for(i = 0; i < file->sz; i++){

		//if char is alpha, shift to decrypted position
		if(isalpha(csr->text[i])){
			csr->text[i] = SHIFT(csr->text[i], shift);
		}//end test if isalpha
	}//end copy loop

	return csr;

}//end shiftText

int main(int argc, char const *argv[])
{
	unsigned int len = 0;
	float chi, kappa;
	int fd;
	FileData* file;
	Str* output;

	fd = open(argv[1], O_RDONLY);
	if(fd == -1){
		error((void*) NULL, "fd", "failed to open in readFile.\n");
	}//end open error check

	file = readFile(fd);

	setup();

	len = freqanalysis(file);

	output = shiftTxt(file, len);

	kappa = kappaTest(file->freq, len);

	printf("Chi Square: %f\n", output->chi);

	printf("Index of Coincidence: %f\n", kappa);
	
	close(fd);
	free(file->freq);
	free(file);
	
	return 0;
}//end main


