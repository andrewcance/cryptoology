typedef struct _values{
//hold the values of expected english plaintext
	const unsigned int *freq;
	const unsigned int **digraph;
	const unsigned int ***trigraph;
	const unsigned float *ExpChiSqr;
	const unsigned float *ndxofcoinc;
}values;

typedef struct _ceaser{
	char* text;
	const unsigned int len;
	unsigned int shift;
	unsigned int *freq;
	unsigned float *chisqr;
}ceaser;

typedef struct _vig{
	ceaser *csr;
	char *ptext;
	char *key;
	unsigned int *freq;
	const unsigned int len;
	unsigned float *chisqr;
	unsigned float *ndxofcoinc; 
}vig;

typedef struct _ciphertext{
	char *text;
	const unsigned int len;
	unsigned int *freq;
	unsigned int **digraph;
	unsigned int ***trigraphs;
	unsigned float *chisqr;
	unsigned float *ndxofcoinc;
	unsigned float chisqrsum;
	
}ciphertext;
