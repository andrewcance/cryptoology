#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "andy.h"
#include "cryptolib.h"

typedef struct _Paramss{
	char order[3];
	char rings[3];
	char pos[3];
	char plugs[19];
}Params;

#define WNUM 5	
#define RNUM 3
#define ABETLEN 26

char scramble(char c, Params* s);
void rotsetup(Params* s);
char cycler(char c, Params* s);
char plugboard(char c, Params* s);
char reflector(char c, Params* s);
char* enigma(char* ptext, Params* s);

const char wheels[WNUM][ABETLEN];

const char ref[26]={
	"YRUHQSLDPXNGOKMIEBFZCWVJAT";
}//end ref

const char notch[5] = "ABCDE";


char scramble(char c, Params s){
//assume only alphas and uppercase

	int i, j, flag = 0;

	//first rotor
	s->pos[0]++;
	s->pos[0] %= ABETLEN;

	//is second rotor at last notch?
	if(flag){
		//if yes, set up second and third rotors
		rotsetup(s);

		//reset flag
		flag = 0
	}//end first check

	//set up rotors if first rotor reached notch
	if(s->pos[0] == notch[s->ordor[0] - 1]){
		rotsetup(s);

		/* Set flag if second rotor reached notch */
		if(s->pos[1] == notch[s->order[1] - 1])
			{flag = 1;}

	}//end second setup

	//run char through the plugboard
	c = plugboard(c, s);

	//cycle the rotors
	c = cycler(c, s);

}//end scramble

void rotsetup(Params* s){
	int i;

	//cycle through all remaining rotors and set them up
	for(i = 1; i < RNUM; i++){
		s->pos[i]++;
		s->pos[i] %= ABETLEN;
	}//end setup loop

}//end rotsetup

char plugboard(char c, Params* s){
/*  Swap pairs of letters on the plugboard */

	int i;

	for (i = 0; s->plug[i]; i += 2)
	{
		if(c == s->plug[i])
			c = s->plug[i + 1];
		else if(c == s->plug[i + 1])
			c = s->plug[i];
	}//end plugboard loop

	return c;
}//end plugboard

char cycler(char c, Params* s){
//this module cycles the rotors forward

	int i;

	for(i = 1; i < RNUM; i++){
		c += s->pos[i] - 'A';
		c %= ABETLEN;

		c -= s->rings[i] - 'A';
		c += ABETLEN;
		c %= ABETLEN;

	}//end cycle rotor

	return c;
}//end cycler

char reflector(char c, Params* s){
/* Reflect rotor*/

	int i;

	for(i = RNUM; i > 0; i--){
		c += s->pos[i - 1] - 'A';
		c %= ABETLEN;

		c -= s->rings[i - 1 ] -'A';
		c %= 26;

	}//end reflect loop

}//end reflector

char* enigma(char* ptext, Params* s){
//take in plaintext and return cryptext

	char* ctext;
	int i;
	int len = strlen(ptext);

	ctext = malloc(len);

	//scramble the letters in the ptext
	for(i = 0; i < len; i++){
		ctext[i] = scramble(ptext[i], s);
	}//end scramble loop

	ctext[i] = '\0';

	return ctext;

}//end enigma