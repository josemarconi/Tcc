// Variable Neighbourhood Search (VNS) for the truck dispatch solution.
// Each outer iteration applies k random shaking moves to the current solution (k goes from 1 up
// to a maximum), runs a local search (VND variants from Uvnd.cpp), and accepts the result only if
// the objective decreases; otherwise the best saved solution is restored.
// Solution arrays: s[] = production per front, nvisitas = trips per front and truck,
// carga = loader and operating flag per front. The objective comes from favaliacao() (minimised).
#include "Hvns.h"
#include "Hvnd.h"
#include "Hmovimento.h"
#include "Hfavaliacao.h"
#include "Hutil.h"
#include <stdlib.h>
#include "Hdefine.h"
#include "hstruct.h"
#include "Hutilcarga.h"
#include "Hsa.h"
#include <string.h>

using namespace std;

// VNS: runs Variable Neighbourhood Search on the solution arrays, in place.
// Keeps copies of the best solution (trip counts, loaders, production, objective).
// forma selects the local search: 1 = VND (fixed order), 2 = VND-RLP; any other value
// runs only the shaking moves. Returns the number of outer iterations (fixed limit of 1800).
int VNS(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv,
         int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio,
         float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam,
         int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin,
         int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem,
         int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn,
         int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga,
         int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem, int forma)
{
    // Outer iteration limit (fixed at 1800 in this function).
    int max_iter_vns = 1800; 
    // Count of outer iterations performed; returned to the caller.
    int contador_iteracoes = 0;
    
    // Objective of the starting solution, the reference for all improvements.
    long long fo_melhor = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax,
                                     prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole,
                                     ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn,
                                     wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga,
                                     wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

    // Allocate the best-solution copies: a trip-count matrix (npilhas x ncam), a loader array and
    // a production array.
    int **nvisitas_best = criamatriz(npilhas, ncam);
    struct alocarga *carga_best = (struct alocarga*)malloc(npilhas * sizeof(struct alocarga));
    float *s_best = (float*)malloc(npilhas * sizeof(float));

    // Store the starting solution as the initial best solution.
    copiamatriz(nvisitas_best, nvisitas, npilhas, ncam);
    copiavetoralocarga(carga_best, carga, npilhas);
    memcpy(s_best, s, npilhas * sizeof(float));

    // Outer loop: one shaking and local search step per iteration.
    for (int iter = 0; iter < max_iter_vns; iter++)
    {
        // Count this outer iteration.
        contador_iteracoes++;
        // Shaking strength k: number of random moves applied in each step. Starts at 1 and grows to 4.
        int k = 1;
        while (k <= 4)
        {
            // Apply k random moves (shaking) to the current solution.
            for (int i = 0; i < k; i++)
            {
                // Pick a random move type 1..8 and random front indices f1, f2 and truck indices c1, c2.
                int mov_aleatorio = rand() % 8 + 1;
                int f1 = rand() % npilhas; int f2 = rand() % npilhas;
                int c1 = rand() % ncam;    int c2 = rand() % ncam;

                // Move types: 1 NumberTrips, 2 Load, 3 RelocateTripTruck, 4 RelocateTripPit, 5 PitOperation,
                // 6 TruckOperation, 7 SwapTrips, 8 SwapShovels. Each move* function changes the state in place.
                switch(mov_aleatorio) {
                    case 1: movimentoNumberTrips(nvisitas, nviagem, s, capcam, npilhas, ncam, carga); break;
                    case 2: movimentoLoad(carga, npilhas, ncam, compatibilidade, nvisitas, s, capcam, nviagem); break;
                    case 3: movimentoRelocateTripTruck(nvisitas, nviagem, f1, f2, c1, s, capcam, compatibilidade, carga); break;
                    case 4: movimentoRelocateTripPit(nvisitas, nviagem, f1, c1, c2, s, capcam, compatibilidade, carga); break;
                    case 5: movimentoPitOperation(carga, f1, nvisitas, s, ncam); break;
                    case 6: movimentoTruckOperation(nvisitas, f1, c1, s, capcam); break;
                    case 7: movimentoSwapTrips(nvisitas, f1, f2, c1, c2, s, capcam, compatibilidade, carga, nviagem); break;
                    case 8: movimentoSwapShovels(carga, f1, f2, nvisitas, ncam, compatibilidade, s, capcam); break;
                }
            }

            // Local search selected by forma: 1 = VND (fixed neighbourhood order).
            if(forma == 1) {
                VND_Referencia(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax,
                    prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole,
                    ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn,
                    wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga,
                    wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);
            }
            // 2 = VND-RLP (random neighbourhood order). Any other forma skips the local search.
            else if (forma == 2) {
                VND_RLP(s, li, ls, rec,
                            penalidadesup, penalidadeinf, penalidadeteorinv,
                            npilhas, nvar, teores,
                            prodmax, prodmin, prodrec,
                            npminerio,
                            remrec, remmax, remmin,
                            nvisitas, carga, capcam,
                            compatibilidade, tciclo, tcontrole,
                            ncam, cargamax, cargamin, nummax,
                            wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn,
                            wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual,
                            wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist,
                            dispmec, nviagem);
            }

            // Evaluate the solution after shaking and local search.
            long long fo_nova = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax,
                                     prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole,
                                     ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn,
                                     wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga,
                                     wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem); 

            // Improvement: save the current solution as the new best and restart with k = 1.
            if (fo_nova < fo_melhor)
            {
                fo_melhor = fo_nova;
                copiamatriz(nvisitas_best, nvisitas, npilhas, ncam);
                copiavetoralocarga(carga_best, carga, npilhas);
                memcpy(s_best, s, npilhas * sizeof(float));
                k = 1;
            }
            else
            {
                // No improvement: restore the current solution from the best copy and increase the shaking strength.
                copiamatriz(nvisitas, nvisitas_best, npilhas, ncam);
                copiavetoralocarga(carga, carga_best, npilhas);
                memcpy(s, s_best, npilhas * sizeof(float));
                // Increase k for the next attempt.
                k++;
            }
        }
    }
    
    // Free the best-solution copies.
    liberamatriz(nvisitas_best, npilhas);
    free(carga_best);
    free(s_best);

    return contador_iteracoes;
}

// VNS_Parametrizado: same scheme as VNS, but the local search is fixed to VND-RLP and the
// limits come from the caller: max_iter_vns (outer iterations) and max_k (maximum shaking strength).
// Returns nothing; on return the arrays hold the best solution found.
void VNS_Parametrizado(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv,
         int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio,
         float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam,
         int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin,
         int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem,
         int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn,
         int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga,
         int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem, 
         int max_iter_vns, int max_k)
{
    // Objective of the starting solution, the reference for all improvements.
    long long fo_melhor = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax,
                                     prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole,
                                     ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn,
                                     wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga,
                                     wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

    // Allocate the best-solution copies (same layout as in VNS).
    int **nvisitas_best = criamatriz(npilhas, ncam);
    struct alocarga *carga_best = (struct alocarga*)malloc(npilhas * sizeof(struct alocarga));
    float *s_best = (float*)malloc(npilhas * sizeof(float));

    // Store the starting solution as the initial best solution.
    copiamatriz(nvisitas_best, nvisitas, npilhas, ncam);
    copiavetoralocarga(carga_best, carga, npilhas);
    memcpy(s_best, s, npilhas * sizeof(float));

    // Outer loop: max_iter_vns iterations.
    for (int iter = 0; iter < max_iter_vns; iter++)
    {
        // Shaking strength k: starts at 1 and grows up to max_k.
        int k = 1;
        while (k <= max_k)
        {
            // Apply k random moves (shaking) to the current solution.
            for (int i = 0; i < k; i++)
            {
                // Pick a random move type 1..8 and random front and truck indices.
                int mov_aleatorio = rand() % 8 + 1;
                int f1 = rand() % npilhas; int f2 = rand() % npilhas;
                int c1 = rand() % ncam;    int c2 = rand() % ncam;

                // Apply the chosen move.
                switch(mov_aleatorio) {
                    case 1: movimentoNumberTrips(nvisitas, nviagem, s, capcam, npilhas, ncam, carga); break;
                    case 2: movimentoLoad(carga, npilhas, ncam, compatibilidade, nvisitas, s, capcam, nviagem); break;
                    case 3: movimentoRelocateTripTruck(nvisitas, nviagem, f1, f2, c1, s, capcam, compatibilidade, carga); break;
                    case 4: movimentoRelocateTripPit(nvisitas, nviagem, f1, c1, c2, s, capcam, compatibilidade, carga); break;
                    case 5: movimentoPitOperation(carga, f1, nvisitas, s, ncam); break;
                    case 6: movimentoTruckOperation(nvisitas, f1, c1, s, capcam); break;
                    case 7: movimentoSwapTrips(nvisitas, f1, f2, c1, c2, s, capcam, compatibilidade, carga, nviagem); break;
                    case 8: movimentoSwapShovels(carga, f1, f2, nvisitas, ncam, compatibilidade, s, capcam); break;
                }
            }

            // Local search: VND-RLP on the shaken solution.
            VND_RLP(s, li, ls, rec,
                        penalidadesup, penalidadeinf, penalidadeteorinv,
                        npilhas, nvar, teores,
                        prodmax, prodmin, prodrec,
                        npminerio,
                        remrec, remmax, remmin,
                        nvisitas, carga, capcam,
                        compatibilidade, tciclo, tcontrole,
                        ncam, cargamax, cargamin, nummax,
                        wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist,
                        dispmec, nviagem);

            // Evaluate the solution after the local search.
            long long fo_nova = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax,
                                     prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole,
                                     ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn,
                                     wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga,
                                     wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem); 

            // Improvement: store the solution as the new best and restart with k = 1.
            if (fo_nova < fo_melhor)
            {
                fo_melhor = fo_nova;
                copiamatriz(nvisitas_best, nvisitas, npilhas, ncam);
                copiavetoralocarga(carga_best, carga, npilhas);
                memcpy(s_best, s, npilhas * sizeof(float));
                k = 1;
            }
            else
            {
                // No improvement: restore the best solution and increase the shaking strength k.
                copiamatriz(nvisitas, nvisitas_best, npilhas, ncam);
                copiavetoralocarga(carga, carga_best, npilhas);
                memcpy(s, s_best, npilhas * sizeof(float));
                k++;
            }
        }
    }
    // Free the best-solution copies.
    liberamatriz(nvisitas_best, npilhas);
    free(carga_best);
    free(s_best);
}