// Declarations for the genetic algorithm implemented in Uag.cpp: the Individuo structure,
// memory and selection helpers, the crossover, repair and mutation operators, and the AlgoritmoGenetico driver.
#ifndef HAG_H
#define HAG_H

// hstruct.h defines struct alocarga (the loader assigned to each front and its operation flag).
#include "hstruct.h"

// Structure encapsulating an individual of the population
// Individual of the population: s = production per front, nvisitas = trips per front and truck,
// carga = loader and operation status per front, fo = objective value (lower is better).
struct Individuo {
    // Production of each front (npilhas values).
    float *s;
    // Trips per front and truck (npilhas x ncam matrix).
    int **nvisitas;
    // Loader and operation status of each front (npilhas values).
    struct alocarga *carga;
    // Objective value. criar_individuo sets it to LLONG_MAX until the individual is evaluated.
    long long fo;
};

// -----------------------------------------------------------------------------
// MEMORY MANAGEMENT AND SELECTION FUNCTIONS
// -----------------------------------------------------------------------------
// Allocates an individual for npilhas fronts and ncam trucks, with all fronts unassigned and no trips.
Individuo criar_individuo(int npilhas, int ncam);
// Frees the memory of an individual (s, nvisitas and carga). npilhas is the number of rows of nvisitas.
void liberar_individuo(Individuo ind, int npilhas);
// Deep-copies src into dest (s, nvisitas, loader/operation status and fo). dest must already be allocated.
void copiar_individuo(Individuo &dest, Individuo &src, int npilhas, int ncam);
// Binary tournament: returns the index of the better of two random individuals of pop (lower fo wins).
int torneio(Individuo *pop, int tam_pop);
// Sorts the first tamanho individuals of pop by fo, ascending (best first).
void ordenar_populacao(Individuo *pop, int tamanho);

// -----------------------------------------------------------------------------
// GENETIC OPERATORS AND REPAIR
// -----------------------------------------------------------------------------
// Crossover: builds filho from parents p1 and p2, then repairs duplicate loaders, recomputes production
// and enforces each truck's time limit by removing trips.
void crossover_carga_viagens(Individuo p1, Individuo p2, Individuo &filho, int npilhas, int ncam, int ncarga,
                             float *capcam, int **compatibilidade, float **tciclo, float tcontrole, float *dispmec);

// Repair operator: raises the ore production to prodrec by giving free loaders to ore fronts and adding trips.
// Updates s, nvisitas and carga of ind.
void reparar_producao_rapido(Individuo &ind, int npilhas, int npminerio, int ncam, int ncarga, 
                             float *capcam, int **compatibilidade, float **tciclo, float tcontrole, 
                             float *dispmec, int **nviagem, float prodrec, float *QTDisp);

// Light mutation: applies 1 to 3 random moves to ind, chosen according to whether production is below prodrec.
void mutacao_dirigida(Individuo &ind, int npilhas, int ncam, float *capcam, int **compatibilidade, int **nviagem, float prodrec);

// Heavy mutation: destroys 10% to 20% of the ore fronts and rebuilds them with a GRASP-style construction.
// ind is passed by value, but its arrays are shared with the caller.
void mutacao_GRASP_minerio(Individuo ind, int npilhas, int npminerio, int ncam, int ncarga, int nvar, 
                           float *capcam, int **compatibilidade, float **teores, float *rec, 
                           float *QTDisp, float *cargamax, int **nviagem, float prodrec, float alpha);

// -----------------------------------------------------------------------------
// MASTER FUNCTION OF THE GENETIC ALGORITHM
// -----------------------------------------------------------------------------
// AlgoritmoGenetico: runs the genetic algorithm and writes the best individual to s_main, nvis_main and carga_main.
// forma_vnd == 1 selects VND as local search, otherwise VND-RLP. Returns the number of generations executed.
int AlgoritmoGenetico(
         float *s_main, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv,
         int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float *QTDisp, int ncarga,
         float remrec, float remmax, float remmin, int **nvis_main, struct alocarga *carga_main, float *capcam,
         int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin,
         int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem,
         int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn,
         int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga,
         int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem, int forma_vnd);

#endif // HAG_H