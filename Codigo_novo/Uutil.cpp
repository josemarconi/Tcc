// #include <vcl.h>
// #include <condefs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <limits.h>
#include <ctype.h>
#include <time.h>
#include <float.h>
#include <errno.h>
#include <math.h>
//----------------------------------------------------------------------
#include "Hutil.h"
#include "Hdefine.h"
#include "hstruct.h"
//----------------------------------------------------------------------
float absfloat(float numero)
{
  if (numero >= 0)
    return numero;
  return -1 * numero;
}
//----------------------------------------------------------------------

void atualizamatrizint(int **s, int **star, int nlinhas, int ncolunas)
{
  for (int i = 0; i < nlinhas; i++)
    for (int j = 0; j < ncolunas; j++)
      s[i][j] = star[i][j];
}
//----------------------------------------------------------------------
void atualizavetorfloat(float *s, float *star, int tam)
{
  for (int i = 0; i < tam; i++)
    s[i] = star[i];
}
//----------------------------------------------------------------------
void atualizavetorint(int *s, int *star, int tam)
{
  for (int i = 0; i < tam; i++)
    s[i] = star[i];
}
//----------------------------------------------------------------------
/* Cria matriz de ponteiros para inteiros com nlinhas e ncolunas */
int **criamatriz(int nlinhas, int ncolunas)
{
  register int i;
  int **matriz;

  matriz = (int **)malloc(nlinhas * sizeof(int *));
  if (!matriz)
    Erro("Falta memoria para alocar a matriz de ponteiros");
  for (i = 0; i < nlinhas; i++)
  {
    matriz[i] = (int *)malloc(ncolunas * sizeof(int));
    if (!matriz[i])
      printf("Falta memoria para alocar a matriz de ponteiros.\n");
  }
  return matriz;
}
//----------------------------------------------------------------------

float **criamatrizfloat(int nlinhas, int ncolunas)
{
  register int i;
  float **matriz;

  matriz = (float **)malloc(nlinhas * sizeof(float *));
  if (!matriz)
    Erro("Falta memoria para alocar a matriz de ponteiros");
  for (i = 0; i < nlinhas; i++)
  {
    matriz[i] = (float *)malloc(ncolunas * sizeof(float));
    if (!matriz[i])
      printf("Falta memoria para alocar a matriz de ponteiros.\n");
  }
  return matriz;
}

//----------------------------------------------------------------------
int *criavetor(int tam)
{
  int *vetor;

  vetor = (int *)malloc(tam * sizeof(int));
  if (!vetor)
    Erro("Falta memoria para alocar o vetor de ponteiros");
  return vetor;
}
//----------------------------------------------------------------------
float *criavetorfloat(int tam)
{
  float *vetor;

  vetor = (float *)malloc(tam * sizeof(float));
  if (!vetor)
    Erro("Falta memoria para alocar o vetor de ponteiros");
  return vetor;
}
//----------------------------------------------------------------------
void embaralhevetor(int *vetor, int tam, int ntrocas)
{
  int k, k1, k2, cont;

  cont = 0;
  do
  {
    k1 = randint(0, tam - 1);
    k2 = randint(0, tam - 1);
    k = vetor[k1];
    vetor[k1] = vetor[k2];
    vetor[k2] = k;
    cont++;
  } while (cont <= ntrocas);
}
//----------------------------------------------------------------------
void Erro(const char *texto)
{
  printf("\n ****** %s ******\n", texto);
  printf("... Saindo do sistema ...\n");
  exit(1);
}
//----------------------------------------------------------------------

void imprimematriz(char nomearq[], int nlinhas, int ncolunas, int **matriz)

{
  register int i, j;
  FILE *arquivo;
  arquivo = fopen(nomearq, "w");
  if (!arquivo)
    printf("O arquivo %s nao pode ser aberto", nomearq);
  for (i = 0; i < nlinhas; i++)
  {
    for (j = 0; j < ncolunas; j++)
    {
      fprintf(arquivo, "%d, %d, %d \n", i, j, matriz[i][j]);
    }
  }
  fclose(arquivo);
}
//----------------------------------------------------------------------
void imprimematrizfloat(char nomearq[], int nlinhas, int ncolunas, float **matriz)

{
  register int i, j;
  FILE *arquivo;
  arquivo = fopen(nomearq, "w");
  if (!arquivo)
    printf("O arquivo %s nao pode ser aberto", nomearq);
  for (i = 0; i < nlinhas; i++)
  {
    for (j = 0; j < ncolunas; j++)
    {
      fprintf(arquivo, "%d, %d, %f \n", i, j, matriz[i][j]);
    }
  }
  fclose(arquivo);
}

//----------------------------------------------------------------------
void iniciamatrizfloat(float **c, int npilhas, int ncolunas, float valor)
{

  for (int i = 0; i < npilhas; i++)
  {
    for (int j = 0; j < ncolunas; j++)
      c[i][j] = valor;
  }
}

//----------------------------------------------------------------------
void iniciamatrizint(int **c, int npilhas, int ncolunas, int valor)
{

  for (int i = 0; i < npilhas; i++)
  {
    for (int j = 0; j < ncolunas; j++)
      c[i][j] = valor;
  }
}

//----------------------------------------------------------------------
void iniciavetorfloat(float *vetor, int tam, float valor)
{
  for (int i = 0; i < tam; i++)
  {
    vetor[i] = valor;
  }
}
//----------------------------------------------------------------------
void iniciavetorint(int *vetor, int tam, int valor)
{
  for (int i = 0; i < tam; i++)
  {

    vetor[i] = valor;
  }
}
//----------------------------------------------------------------------
/* Le uma matriz no formato: linha, coluna, valor */
void learqmatriz(char nomearq[], int **matriz)
{
  int i,        /* horarios */
      j,        /* sala */
      numturma; /* numero da turma*/
  FILE *arquivo;

  arquivo = fopen(nomearq, "r");
  if (!arquivo)
    printf("O Arquivo %s nao pode ser aberto.\n", nomearq);
  while (!feof(arquivo))
  {
    fscanf(arquivo, "%d, %d, %d", &i, &j, &numturma);
    matriz[i][j] = numturma;
  }
  fclose(arquivo);
}
//----------------------------------------------------------------------
void learqmatrizfloat(char nomearq[], float **matriz)
{
  int p,      /* pilha */
      v;      /* variavel */
  float teor; /* teor*/
  FILE *arquivo;

  arquivo = fopen(nomearq, "r");

  if (!arquivo)
  {
    printf("O Arquivo %s nao pode ser aberto.\n", nomearq);
    getchar();
    exit(1);
  }
  while (!feof(arquivo))
  {

    fscanf(arquivo, "%d,%d,%f", &p, &v, &teor);

    matriz[p][v] = teor;
  }
  fclose(arquivo);
}
//----------------------------------------------------------------------
void learqvetor(char nomearq[], int *vetor)
{
  int j,
      valor;
  FILE *arquivo;

  arquivo = fopen(nomearq, "r");
  if (!arquivo)
  {
    printf("O Arquivo %s nao pode ser aberto.\n", nomearq);
    getchar();
    exit(1);
  }

  while (!feof(arquivo))
  {
    fscanf(arquivo, "%d,%d", &j, &valor);
    vetor[j] = valor;
  }
  fclose(arquivo);
}
//----------------------------------------------------------------------
void learqvetorfloat(char nomearq[], float *vetor)
{
  int j;
  float valor;
  FILE *arquivo;

  arquivo = fopen(nomearq, "r");
  if (!arquivo)
  {
    printf("O Arquivo %s nao pode ser aberto.\n", nomearq);
    getchar();
    exit(1);
  }

  while (!feof(arquivo))
  {
    fscanf(arquivo, "%d,%f", &j, &valor);

    vetor[j] = valor;
  }

  fclose(arquivo);
}
//----------------------------------------------------------------------
void liberamatriz(int **matriz, int nlinhas)
{
  register int i;

  for (i = nlinhas - 1; i >= 0; i--)
    free((int *)matriz[i]);
  free((int *)matriz);
}
//----------------------------------------------------------------------
void liberamatrizfloat(float **matriz, int nlinhas)
{
  register int i;

  for (i = nlinhas - 1; i >= 0; i--)
    free((float *)matriz[i]);
  free((float *)matriz);
}
//----------------------------------------------------------------------
void liberavetor(int *vetor)
{

  free(vetor);
}
//----------------------------------------------------------------------
void liberavetorfloat(float *vetor)

{

  free(vetor);
}
//----------------------------------------------------------------------
float maiorfloat(float *vetor, int tam)
{
  float aux = (-1) * (float)(INT_MAX);
  for (int i = 0; i < tam; i++)
  {
    if (vetor[i] > aux)
      aux = vetor[i];
  }
  return aux;
}
//----------------------------------------------------------------------
float menorfloat(float *vetor, int tam)
{
  float aux = (float)(INT_MAX);
  for (int i = 0; i < tam; i++)
  {
    if (vetor[i] < aux)
      aux = vetor[i];
  }
  return aux;
}
//----------------------------------------------------------------------
/* Gera inteiro aleatorio entre min e max */
int randint(int min, int max)
{
  if (min == max)
    return min;
  return (int)(randomico(0, max - min + 1.0)) + min;
}
//----------------------------------------------------------------------

double randomico(double min, double max)
{
  if (min == max)
    return min;
  return ((double)(rand() % 10000 / 10000.0) * (max - min) + min);
}
//----------------------------------------------------------------------
void learqsemente(char nomearq[], unsigned long *semente, int *metodo)
{
  FILE *arquivo;
  int b;
  unsigned long a;
  arquivo = fopen(nomearq, "r");
  if (!arquivo)
  {
    printf("O Arquivo %s nao pode ser aberto.\n", nomearq);
    getchar();
    exit(1);
  }

  while (!feof(arquivo))
  {
    fscanf(arquivo, "%lu,%d", &a, &b);
    *semente = a;
    *metodo = b;
  }
  fclose(arquivo);
}
//***********************************************************************

void imprimevalordefo(char nomearq[], float tempo, int fo, int iteracao)
{

  FILE *arquivo;
  arquivo = fopen(nomearq, "a+");

  if (!arquivo)
  {
    printf("O arquivo %s nao pode ser aberto", nomearq);
    exit(1);
  }
  fprintf(arquivo, "%4.2f\t  %12d\t  %12d\n", tempo, iteracao, fo);
  fclose(arquivo);
}
//**************************************************************************
void imprimeRel(float fomin, int metodo, float tempo, char *inst)
{

  FILE *arquivo;
  arquivo = fopen("relatorio.rel", "a+");
  if (!arquivo)
  {
    printf("O arquivo %s nao pode ser aberto", "relatorio.rel");
    exit(1);
  }

  fprintf(arquivo, "%s; %d; %10.0f; %10.0f\n", inst, metodo, tempo, fomin);
  fclose(arquivo);
}

void copiamatriz(int **dest, int **src, int linhas, int colunas) {
    for (int i = 0; i < linhas; i++) {
        memcpy(dest[i], src[i], colunas * sizeof(int));
    }
}

void copiavetoralocarga(struct alocarga *dest, struct alocarga *src, int tamanho) {
    memcpy(dest, src, tamanho * sizeof(struct alocarga));
}