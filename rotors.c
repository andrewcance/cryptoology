#include <string.h>

#ifndef wheels_h
#define wheels_h 0

#define BUFSIZE 300
#define STR(x) s->r[i].str[x]

typedef struct _rotor{
	const char *str;
	const char notch;
	int pos;
}Rotor;

typedef struct _Paramss{
	Rotor *r;
	const char *rings;
	const char *plugs;
}Params;

typedef struct _output{
	char* text;
	Params* s;
}Output;

const rotor r2{
	str = "AJDKSIRUXBLHWTMCQGZNPYFVOE";
	notch = 'Q';
}//end r2

const rotor r3{
	str = "AKMUSQOESOVPZJAYQUIRHXLNFT";
	notch = 'E';
}//end r3

const rotor r4{
	str = "ESOVPZJAYQUIRHXLNFTGKDCMWB";
	notch = 'V';
}//end r4

const rotor r5{
	str = "VZBRGITYUPSDNHLXAWMJQOFECK";
	notch = 'Z'
}



#endif

