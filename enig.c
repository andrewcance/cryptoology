#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
	Defines
*/

#define CHARSIZE sizeof(char)
#define BUFSIZE 300
#define STR(x) s->r[i].str[x]

/*
	Typedefs
*/

typedef struct _rotor{
	const char *str;
	char notch;
	int rtrnum;
	int pos;
}Rotor;

typedef struct _Paramss{
	Rotor *r;
	unsigned int rings[3];
	char plugs[10];
}Params;

typedef struct _output{
	char* text;
	Params* s;
}Output;

typedef unsigned char bool;

/*
	Prototypes
*/

Params* initparams(const char* filename);
char rotor(char c, Params* s, int i, bool forward);
char scrambler(char c, Params* s);
char plugboard(char c, Params* s);
void cycler(Params* s, int i);
Output* enigma(char* ptext, Params* s);
char set(char c);
void ucase(char* text);
char* prompt(const char* msg);
void error(const void* ptr, const char *name, const char* msg);
Params* initrotor(Params *s, FILE *fd, int i);

/*
	Global Variables
*/

const int len = 26;
const int rnum = 3;
const char reflector[26] = "YRUHQSLDPXNGOKMIEBFZCWVJAT";

int main(int argc, char const *argv[])
{
	FILE *pfile, *cfile;
	const char *pfilename, *cfilename;
	Output *r;
	char *text;
	Params *s;
	int i;
	
	pfilename = argv[1];
	cfilename = argv[2];

	pfile = fopen(pfilename, "r");
	cfile = fopen(cfilename, "w");

	//error fopen check
	error(pfile, pfilename, "File does not exist.\n");
	error(cfile, cfilename, "file does not exist.\n");

	text = calloc(BUFSIZE, CHARSIZE);
	error(text, "text", "calloc failed in main.");

	//set up initial parameters
	s = initparams(argv[3]);

	//read one line at a time from the pfile then encrypt and write to the cfile.
	while((fgets(text, BUFSIZE, pfile)
 != NULL)){

		//prepare the text for encrypting assume only alphabetical chars.
		ucase(text);

		//encrypt line return cyphertext and parameters for reuse.
		r = enigma(text, s);
		free(text);//prepare text for use below
				
		//extract Params for reuse
		s = r->s;

		//extract ciphertext and write to ciphertext file.
		text = r->text;

		//print to output file.
		fputs(text, cfile);
		fwrite("\n", 1, CHARSIZE, cfile);

		free(r);
	}//end read/write loop

	//close and free, prep for end of program.
	fclose(pfile);
	fclose(cfile);

	//free loop
	for(i = 0; i < rnum; i++){
		free((void*) s->r[i].str);
	}//end free loop

	free(s->r);
	free(s);
	free(text);

	return 0;
}//end main

Params* initparams(const char* filename){
/* Initialize all parameters for use
	by the enigma.  Get user input 
	for rotor selection, initial 
	position, ring position, and 
	plugboard settings.  Filename 
	passed in contains wheels */

	FILE* fd;
	Params* s;
	int i;

	fd = fopen(filename, "r");

	//error fopen check
	error(fd, filename, "File does not exist in initparams.\n");

	s = calloc(1, sizeof(Params));

	//params calloc error check
	error(s, "s", "calloc failed in initparams.\n");

	s->r = calloc(rnum, sizeof(Rotor));

	//rotor calloc error check
	error(s->r, "r", "calloc failed in initparams.\n");
	
	//populate rotors
	for(i = 0; i < rnum; i++){
		s = initrotor(s, fd, i);
	}//end populate loop

	//set initial ring positions
	s->rings[0] = atoi(prompt("Enter ring 1: ")) - 'A';
	s->rings[1] =atoi(prompt("Enter ring 2: ")) - 'A';
	s->rings[2] = atoi(prompt("Enter ring 3: ")) - 'A';

	//initialize plugboard
	s->plugs[0] = 'A';
	s->plugs[1] = 'B';
	s->plugs[2] = 'C';
	s->plugs[3] = 'D';
	s->plugs[4] = 'E';
	s->plugs[5] = 'F';
	s->plugs[6] = 'G';
	s->plugs[7] = 'H';
	s->plugs[8] = 'I';
	s->plugs[9] = 'J';

	fclose(fd);

	return s;

}//end initparams

char rotor(char c, Params* s, int i, bool forward){
/*
	Scramble one char through the ith rotor wheel.
*/
	int j = 0;

	c += s->r[i].pos;
	c = set(c);

	/*After the algorithm is done,
	rewrite the rings element to 
	turn it into an int so I don't 
	have to subtract 'A' each loop*/
	c -= s->rings[i];
	c = set(c);

	//forward or revverse direction
	if(forward){
		c = STR(c - 'A');
	}
	else
	{
		while(s->r[i].str[j] != c){
			j++;
			j %= 26;
		}//end while loop

		c = j + 'A';

	}//end forward/reverse if/else

	c += s->rings[i];
	c = set(c);

	c -= s->r[i].pos;
	c = set(c);

	return c;

}//end rotor

char scrambler(char c, Params *s){
/* Operate the enigma cypher, assume 	only uppercase alphabetical chars.
*/

	int i;

	/*Cycle the rotors*/
	cycler(s, 0);

	//run char through the plugboard
	c = plugboard(c, s);
	
	//pass forward through the rotors
	for(i = 0; i < rnum; i++)
	{
		c = rotor(c, s, i, 1);
	}//end rotor loop

	/*Reflector*/
	c = reflector[c - 'A'];

	//pass backward  through rotors
	for(i = rnum - 1; i >= 0; i--)
	{
		c = rotor(c, s, i, 0);
	}//end rotor loop

	//second pass through plugboard
	c = plugboard(c, s);
	
	return c;

}//end scrambler

char plugboard(char c, Params* s){
/*  Swap pairs of letters on the plugboard */

	int i;

	for (i = 0; i < 10; i += 2)
	{
		if(c == s->plugs[i])
			c = s->plugs[i + 1];
		else if(c == s->plugs[i + 1])
			c = s->plugs[i];
	}//end plugboard loop

	return c;
}//end plugboard

void cycler(Params* s, int i){
/*Cycle the rotors to the next 
  position before each char has 
  passed throught the rotors. */

	s->r[i].pos++;
	s->r[i].pos %= len;
	
	if((i < rnum) && (STR(s->r[i].pos) == s->r[i].notch))
	{
		cycler(s, ++i);
	}

}//end cycler

Output* enigma(char* ptext, Params* s){
//take in plaintext and return cryptext

	char* ctext;
	int i;
	const int tlen = strlen(ptext);

	Output* r = calloc(1, sizeof(Output));
	
	//Output calloc error check
	error(r, "r", "calloc failed in enigma.\n");

	ctext = calloc(tlen + 1, CHARSIZE);

	//crypttext calloc error check
	error(ctext, "ctext", "calloc failed in enigma.\n");

	//scramble the letters in the ptext
	for(i = 0; i < tlen; i++){
		//if charactor isn't a letter, skip
		//if(isalpha(ptext[i])){
			ctext[i] = scrambler(ptext[i], s);
		//}//end check if is alpha
	}//end scramble loop

	ctext[i] = '\0';

	//prepare for ciphertext and params to be passed out.
	r->text = ctext;
	r->s = s;

	return r;

}//end enigma

char set(char c){
/* Check if the char c is out of
   range of uppercase letters.  If
   so, move it back into the range. */

	//if above the alphabet, substract 26 to return it to the alphabet
	if(c > 'Z'){
		c -= 26;
	}//end if greater

	//if below the alphabet, add 26 to return it to the alphabet
	else if(c < 'A'){
		c += 26;
	}//end if less

	return c;
}//end set

void ucase(char* text){
//Make the line uppercase and remove line terminators.

	int i;
	const int len = strlen(text);

	//cycle through and make uppercase the lower case letters
	for(i = 0; i < len; i++){
		if(islower(text[i]) && isalpha(text[i]))
		{
			text[i] += ('A' - 'a');
		}//end check if lower
		else if(!isalpha(text[i])){
			text[i] = '\0';//remove line terminator
		}//end check if not alpha
	}//end to upper loop

}//end ucase

char* prompt(const char* msg){
    char* output = calloc(100, CHARSIZE);

    /*
       This function takes a prompt from the caller and displays it.  Then it returns a user input.
    */

    fprintf(stdout, "%s", msg);
    fgets(output, 100, stdin);
    return output;
    
}//end prompt

void error(const void* ptr, const char *name, const char* msg){
    if(ptr == '\0'){
        fprintf(stderr, "%s ", name);
        perror(msg);
        exit(1);
    }
}//end error

Params* initrotor(Params *s, FILE *fd, int i){
/*Get one line from the file and populate the rotors being used.*/
	
	char *buffer;
	char startpos;
	char *line, *saveptr;

	buffer = calloc(35, CHARSIZE);
	//buffer calloc error check
	error(buffer, "buffer", "calloc error in initrotor.\n");

	//copy rotor wheel string from file
	fgets(buffer, 35, fd);
	buffer[35] = '\0';

	//get the rotors number
	line = strtok_r(buffer, ",", &saveptr);
	s->r[i].rtrnum = atoi(line);

	printf("Enter the initial position for rotor %d: ", rnum - i);

	startpos = getc(stdin);

	//copy the string into the rotor struct		
	line = strtok_r(NULL, ",", &saveptr);

	s->r[i].str = strdup(line);

	//find the initial start position
	while(startpos != s->r->str[s->r[i].pos]){
		s->r[i].pos++;
		s->r[i].pos %= 26;
	}//end set pos

	//get the notch position
	line = strtok_r(NULL, ",", &saveptr);	
	s->r->notch = line[0];

	free(buffer);
	free(line);

	return s;

}//end initrotor

