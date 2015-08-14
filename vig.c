#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 500

typedef unsigned char bool;

char* readfile(int fd);
char* rempunc(char* str);
char crypt(char text, char key, bool encode);
char encrypt(char text, char key);
char decrypt(char text, char key);
void error(void* ptr, const char* msg);
void println(const char* str);

int main(int argc, char const *argv[])
{
	int fd;
	char* key = strdup(argv[2]);
	bool encode = atoi(argv[3]);
	char* text;
	int klen, sz, plen;
	int i = 0, j = 0, ndx = 0;
	char* ptext;

	fd = open(argv[1], O_RDONLY);

	klen = strlen(key);

	text = readfile(fd);
	sz = strlen(text);

	ptext = rempunc(text);
	plen = strlen(ptext);
	
	for(i = 0; i < sz; i++){		

		ptext[i] = crypt( ptext[i], key[ndx], encode);			
		ndx++;
		ndx %= klen;

	}//end encypher loop

	//print loop
	for(i = 0; i < plen; i++){

		if((i % 30 == 0)){
			printf("\n");
		}

		printf("%c", ptext[i]);

	}//end print loop

	close(fd);
	free(key);
	free(ptext);

	return 0;
}//end main

char crypt(char text, char key, bool encode)
{

	//encrypt
	if(encode){
		return encrypt(text, key);
	}
	else{//decrypt
		return decrypt(text, key);
	}

}//end crypt

char encrypt(char text, char key)
{

	char out;

	out = 'A' + (text - 'A' + key - 'A') % 26;

	return out;

}//end encrypt

char decrypt(char text, char key){

	char out;

	out =  'A' + (text - key + 26) % 26;

	return out;

}//end decrypt

void error(void* ptr, const char* msg){
    if(ptr == '\0'){
        perror(msg);
        exit(1);
    }
}//end error

void println(const char* str){
	fprintf(stdout, "%s\n", str);
}//end println
