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

void VNS(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv,
         int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio,
         float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam,
         int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin,
         int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem,
         int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn,
         int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga,
         int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem)
{
    
    int max_iter_vns = 100; 
    long long fo_melhor = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax,
                                     prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole,
                                     ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn,
                                     wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga,
                                     wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

    // Alocação de memória para Backup
    int **nvisitas_best = criamatriz(npilhas, ncam);
    struct alocarga *carga_best = (struct alocarga*)malloc(npilhas * sizeof(struct alocarga));
    float *s_best = (float*)malloc(npilhas * sizeof(float));

    // Salva estado inicial
    copiamatriz(nvisitas_best, nvisitas, npilhas, ncam);
    copiavetoralocarga(carga_best, carga, npilhas);
    memcpy(s_best, s, npilhas * sizeof(float));

    for (int iter = 0; iter < max_iter_vns; iter++)
    {
        int k = 1;
        while (k <= 8)
        {
            // --- 1. SHAKING (Perturbação Aleatória) ---
            for (int i = 0; i < k; i++)
            {
                int mov_aleatorio = rand() % 8 + 1; // Escolhe 1 dos 8 tipos
                int f1 = rand() % npilhas; int f2 = rand() % npilhas;
                int c1 = rand() % ncam;    int c2 = rand() % ncam;

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

            // --- 2. BUSCA LOCAL (VND exaustivo) ---
            VND(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax,
                prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole,
                ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn,
                wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga,
                wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

            /*VND_Aleatorio(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax,
                prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole,
                ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn,
                wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga,
                wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);*/

            long long fo_nova = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax,
                                     prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole,
                                     ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn,
                                     wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga,
                                     wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem); 

            // --- 3. ACEITAÇÃO ---
            if (fo_nova < fo_melhor)
            {
                fo_melhor = fo_nova;
                copiamatriz(nvisitas_best, nvisitas, npilhas, ncam);
                copiavetoralocarga(carga_best, carga, npilhas);
                memcpy(s_best, s, npilhas * sizeof(float));
                k = 1;
                //printf("Mina: Iter %d | Nova Melhor FO: %lld\n", iter, fo_melhor);
            }
            else
            {
                // ROLLBACK: Restaura a partir do melhor
                copiamatriz(nvisitas, nvisitas_best, npilhas, ncam);
                copiavetoralocarga(carga, carga_best, npilhas);
                memcpy(s, s_best, npilhas * sizeof(float));
                k++;
            }
        }
    }
    
    // LIMPEZA DE MEMÓRIA (Obrigatório)
    liberamatriz(nvisitas_best, npilhas);
    free(carga_best);
    free(s_best);
}