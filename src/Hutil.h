// Function prototypes of the utility functions defined in Uutil.cpp.
// Returns the absolute value of a float.
float absfloat(float numero);
// Copies the int matrix star into s (nlinhas x ncolunas).
void atualizamatrizint(int **s,int **star,int nlinhas,int ncolunas);
// Copies the first tam elements of the float vector star into s.
void atualizavetorfloat(float *s,float *star,int tam);
// Copies the first tam elements of the int vector star into s.
void atualizavetorint(int *s,int *star,int tam);
// Allocates an int matrix of nlinhas x ncolunas (array of row pointers). Free with liberamatriz.
int **criamatriz(int nlinhas, int ncolunas);
// Allocates a float matrix of nlinhas x ncolunas. Free with liberamatrizfloat.
float **criamatrizfloat(int nlinhas, int ncolunas);
// Allocates an int vector of tam elements. Free with liberavetor.
int *criavetor(int tam);
// Allocates a float vector of tam elements. Free with liberavetorfloat.
float *criavetorfloat(int tam);
// Shuffles the int vector by performing ntrocas+1 random pairwise swaps.
void embaralhevetor(int *vetor, int tam, int ntrocas);
// Prints texto, then terminates the program with exit(1).
void Erro(const char *texto);
// Writes an int matrix to file nomearq (overwritten) as "row, column, value" lines.
void imprimematriz(char nomearq[], int nlinhas, int ncolunas, int **matriz);
// Writes a float matrix to file nomearq (overwritten) as "row, column, value" lines.
void imprimematrizfloat(char nomearq[], int nlinhas, int ncolunas, float **matriz);
// Sets every element of the npilhas x ncolunas float matrix c to valor.
void iniciamatrizfloat(float **c,int npilhas,int ncolunas,float valor );
// Sets every element of the npilhas x ncolunas int matrix c to valor.
void iniciamatrizint(int **c,int npilhas,int ncolunas,int valor );
// Sets the first tam elements of the float vector to valor.
void iniciavetorfloat(float *vetor,int tam,float valor );
// Sets the first tam elements of the int vector to valor.
void iniciavetorint(int *vetor,int tam,int valor );
// Reads an int matrix from nomearq ("row, column, value" records) into the allocated matriz.
void learqmatriz(char nomearq[], int **matriz);
// Reads a float matrix from nomearq ("pile,variable,value" records) into the allocated matriz.
void learqmatrizfloat(char nomearq[], float **matriz);
// Reads an int vector from nomearq ("index,value" records) into vetor[index].
void learqvetor(char nomearq[], int *vetor);
// Reads a float vector from nomearq ("index,value" records) into vetor[index].
void learqvetorfloat(char nomearq[], float *vetor);
// Frees an int matrix allocated by criamatriz (nlinhas row arrays, then the pointer array).
void liberamatriz(int **matriz, int nlinhas);
// Frees a float matrix allocated by criamatrizfloat.
void liberamatrizfloat(float **matriz, int nlinhas);
// Frees an int vector allocated by criavetor.
void liberavetor(int *vetor);
// Frees a float vector allocated by criavetorfloat.
void liberavetorfloat(float *vetor);
// Returns the largest of the first tam float values.
float maiorfloat(float *vetor,int tam);
// Returns the smallest of the first tam float values.
float menorfloat(float *vetor,int tam);
// Returns a random integer in the inclusive range [min, max].
int randint(int min, int max);
// Returns a random double in [min, max), based on rand().
double randomico(double min, double max);
// Reads "seed,method" records from nomearq and keeps the values of the last record.
void learqsemente(char nomearq[],unsigned long *semente, int *metodo);
// Appends one tab-separated line (time, iteration, Fo) to file nomearq.
void imprimevalordefo(char nomearq[], float tempo, int fo, int iteracao);
// Appends one line (instance, method, time, Fo) to the file relatorio.rel.
void imprimeRel(float fomin, int metodo, float tempo, char *inst);
// Copies an int matrix of linhas x colunas from src to dest, row by row.
void copiamatriz(int **dest, int **src, int linhas, int colunas);
// Copies size alocarga structs from src to dest.
void copiavetoralocarga(struct alocarga *dest, struct alocarga *src, int tamanho);
