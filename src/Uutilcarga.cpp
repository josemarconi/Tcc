// #include <vcl.h>
// #include <condefs.h>
// Standard C library headers: I/O, memory, strings, limits, character classes, time, floats, errors, math.
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
// Project headers: Hutil.h (vector and matrix helpers, randint, absfloat), Hdefine.h (constants:
// disponivel = -1 means "no loader", compativel = 1, estdin), Hutilcarga.h (prototypes of this file),
// Hfavaliacao.h (objective function favaliacao and its fa_* terms).
#include "Hutil.h"
#include "Hdefine.h"
#include "Hutilcarga.h"
#include "Hfavaliacao.h"
#include "Hsa.h"
#include "Hmovimento.h"

// Defines struct alocarga: equip (loader id of the pile, disponivel when none) and op (pile is operational).
#include "hstruct.h"

//----------------------------------------------------------------------
// Allocates a vector of tam struct alocarga entries with malloc; stops with an error if the allocation fails.
// The caller releases it with liberavetoralocarga.
struct alocarga *criavetoralocarga(int tam)
{
        struct alocarga *vetor;

        vetor = (struct alocarga *)malloc(tam * sizeof(struct alocarga));
        if (!vetor)
                Erro("Falta memoria para alocar o vetor de ponteiros");
        return vetor;
}
//----------------------------------------------------------------------
// Recomputes the production vector s: s[i] is the production of pile i, given by producaofrente.
void carregavetors(float *s, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, int ncam, int npilhas)
{
        for (int i = 0; i < npilhas; i++)
        {
                s[i] = producaofrente(nvisitas, carga, capcam, compatibilidade, ncam, i);
        }
}

//----------------------------------------------------------------------
// Builds a random initial loader assignment. Each loader i is first placed on pile i (equip = i),
// then random swaps are applied (see below) and the production vector s is recomputed.
void constroisolucaoinicialcarga(struct alocarga *carga, float *s, int ncarga, int npilhas, int **nvisitas, float *capcam, int **compatibilidade, int ncam)
{
        //       printf("Construindo solucao inicial aleatoria para a aloca��o de equipamentos de carga...\n");

        // Place loader i on pile i for every loader.
        for (int i = 0; i < ncarga; i++)
        {
                carga[i].equip = i;
        }

        //        for(int i=0;i<npilhas;i++){
        //                printf("%d\n",carga[i].equip);
        //        }getchar();

        // Random shuffle: for each pile i, mark it operational and swap two random piles p1 and p2 with trocar,
        // which moves the loader, flag, production and visit counts of the two piles together.
        for (int i = 0; i < npilhas; i++)
        {
                int p1 = randint(0, npilhas - 1);
                int p2 = randint(0, npilhas - 1);
                carga[i].op = true;
                trocar(carga, s, p1, p2, nvisitas, capcam, compatibilidade, ncam);
        }

        //        for(int i=0;i<npilhas;i++){
        //                printf("%d\n",carga[i].equip);
        //        }getchar();

        // Recompute the production vector s after the shuffle.
        carregavetors(s, nvisitas, carga, capcam, compatibilidade, ncam, npilhas);
}
//----------------------------------------------------------------------
// Random initial visit counts: nvisitas[i][j] is a random integer from 0 to nviagem[i][j], or 0 when
// nviagem[i][j] is not positive. nviagem is taken per pile and per truck. Then s is recomputed.
void constroisolucaoinicialvisitasale(float *s, struct alocarga *carga, float *capcam, int **compatibilidade, int **nvisitas, int **nviagem, int npilhas, int ncam)
{
    // Visit counts are drawn for every pile i and every truck j.
    for(int i=0; i<npilhas; i++) {
        for(int j=0; j<ncam; j++) {
            // Now nviagem is specific per truck [i][j]
            // Draw only when at least one trip is possible for this pile and truck.
            if(nviagem[i][j] > 0)
                nvisitas[i][j] = randint(0, nviagem[i][j]);
            else
                nvisitas[i][j] = 0;
        }
    }
    carregavetors(s, nvisitas, carga, capcam, compatibilidade, ncam, npilhas);
}
// Experimental variant of the visit construction with a budget of nviagemmax trips per truck.
// It prints the loader and production of each pile and waits for a key press (debug output),
// draws the visits, recomputes s and prints and waits again. nvisitas is not cleared first.
void constroisolucaoinicialvisitas02(float *s, struct alocarga *carga, float *capcam, int **compatibilidade, int **nvisitas, int nviagemmax, int npilhas, int ncam)
{

        // Print the loader id and the production of every pile (debug output).
        for (int i = 0; i < npilhas; i++)
        {
                printf("%d - %f\n", carga[i].equip, s[i]);
        }
        // Pause until a key is pressed (debug).
        getchar();

        // Trips still available for the current truck.
        int nvdisp = 0;
        // For each truck j, distribute up to nviagemmax trips among the piles.
        for (int j = 0; j < ncam; j++)
        {
                nvdisp = nviagemmax;
                for (int i = 0; i < npilhas; i++)
                {
                        // Candidate piles: those with a loader (not disponivel) that are operational.
                        if (carga[i].equip != disponivel && carga[i].op == true)
                        {
                                // Only piles whose loader is compatible with truck j.
                                if (compatibilidade[j][carga[i].equip] == compativel)
                                {
                                        // Random number of trips from 0 to the remaining budget, taken from the budget.
                                        nvisitas[i][j] = randint(0, nvdisp);
                                        nvdisp -= nvisitas[i][j];
                                        // Stop distributing for this truck when its budget is used up.
                                        if (nvdisp == 0)
                                                break;
                                }
                        }
                }
        }

        // Recompute the production vector s.
        carregavetors(s, nvisitas, carga, capcam, compatibilidade, ncam, npilhas);

        // Print the production of every pile again (debug output).
        for (int i = 0; i < npilhas; i++)
        {
                printf("%d - %f\n", carga[i].equip, s[i]);
        }
        // Pause until a key is pressed (debug).
        getchar();
}

//----------------------------------------------------------------------
// Random initial visits: for each truck j one pile index escolha is drawn; if that pile has a loader and
// is operational it receives nviagemax trips for truck j. The ok flag is set after the if, so one draw
// is made per truck. Then the production vector s is recomputed.
void constroisolucaoinicialvisitas(float *s, struct alocarga *carga, float *capcam, int **compatibilidade, int **nvisitas, int *nviagem, int npilhas, int ncam, int nviagemax)
{
        int escolha;
        bool ok = false;
        for (int j = 0; j < ncam; j++)
        {
                // Draw a random pile index and check it.
                do
                {
                        escolha = randint(0, npilhas - 1);
                        ok = false;
                        // Only piles with a loader (not disponivel) that are operational can receive visits.
                        if (carga[escolha].equip != disponivel && carga[escolha].op == true)
                                nvisitas[escolha][j] = nviagemax;
                        ok = true;
                } while (!ok);
        }
        // Recompute the production vector s from the new visit counts.
        carregavetors(s, nvisitas, carga, capcam, compatibilidade, ncam, npilhas);
}
//----------------------------------------------------------------------
//----------------------------------------------------------------------

// Returns the average grade of parameter varj over the ore piles (i < npminerio), weighted by production:
// sum(teores[i][varj] * s[i]) / sum(s[i]). Returns 0 when the ore production is zero. Changes no array.
float encontrado(float *s, float **teores, int npminerio, int varj)
{
        float soma1 = 0;
        float soma2 = 0;

        // Accumulate the weighted grade (soma1) and the total ore production (soma2) over the ore piles.
        for (int i = 0; i < npminerio; i++)
        {
                soma1 += teores[i][varj] * s[i];
                soma2 += s[i];
        }
        // No ore produced: return 0 to avoid a division by zero.
        if (soma2 == 0)
                return 0;

        return soma1 / soma2;
}

//----------------------------------------------------------------------
// Sets equip = valor1 (converted to int) and op = valor2 for the first tam entries of vetor.
void iniciavetoralocarga(struct alocarga *vetor, int tam, float valor1, bool valor2)
{
        for (int i = 0; i < tam; i++)
        {
                vetor[i].equip = valor1;
                vetor[i].op = valor2;
        }
}

//----------------------------------------------------------------------
// Frees a vector created with criavetoralocarga.
void liberavetoralocarga(struct alocarga *vetor)

{
        free(vetor);
}
//----------------------------------------------------------------------
// Returns the time-use fraction of truck cam: sum over operational piles with a compatible loader of
// nvisitas[i][cam] * tciclo[i][cam], divided by tcontrole. Returns 0 when the sum or tcontrole is zero.
float produtcam(int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int npilhas, int cam)
{
    float soma = 0.0;
    // Accumulate trips times cycle time over the piles served by truck cam.
    for(int i=0; i<npilhas; i++) {
        // Skip piles without a loader or that are not operational.
        if(carga[i].equip != disponivel && carga[i].op == true) {
            // Only piles whose loader is compatible with truck cam.
            if(compatibilidade[cam][carga[i].equip]) {
                // CHANGED HERE: tciclo[i][cam]
                soma += nvisitas[i][cam] * tciclo[i][cam];
            }
        }
    }
    // No trips for this truck: return 0.
    if(soma == 0) return 0.0;
    // Control time is zero: return 0 to avoid a division by zero.
    if(tcontrole == 0) return 0.0;

    // Fraction of the control time used by the trips of truck cam.
    return soma / tcontrole;
}
//----------------------------------------------------------------------
// Returns the number of operational piles with a compatible loader that truck cam visits (nvisitas > 0),
// minus one. Returns 0 when no pile is visited.
int dinamico(int **nvisitas, struct alocarga *carga, int **compatibilidade, int npilhas, int cam)
{
        int soma = 0;
        // Count the piles visited by truck cam.
        for (int i = 0; i < npilhas; i++)
        {
                if (carga[i].equip != disponivel && carga[i].op == true)
                        // Only piles whose loader is compatible with truck cam.
                        if (compatibilidade[cam][carga[i].equip] == compativel)
                        {
                                // The pile receives at least one trip from truck cam.
                                if (nvisitas[i][cam] > 0)
                                {
                                        soma++;
                                }
                        }
        }
        // At least one visited pile: return the count minus one.
        if (soma > 0)
                return (soma - 1);
        else
                return 0;
}
//----------------------------------------------------------------------
// Returns the production of pile frente: sum over trucks k compatible with its loader of
// nvisitas[frente][k] * capcam[k]. Returns 0 when the pile is not operational or has no loader.
float producaofrente(int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, int ncam, int frente)
{
        float soma = 0.0;

        // Non-operational pile: no production.
        if (carga[frente].op == false)
                return 0.0;

        // Pile without a loader: no production.
        if (carga[frente].equip == disponivel)
                return 0.0;
        else
        {
                // Sum trips times truck capacity over the trucks compatible with this pile's loader.
                for (int k = 0; k < ncam; k++)
                        if (compatibilidade[k][carga[frente].equip] == compativel)
                                soma += nvisitas[frente][k] * capcam[k];
        }
        return soma;
}
//----------------------------------------------------------------------
// Swaps piles pilha1 and pilha2: loader id, operation flag, production s and the visit counts of every truck.
// The visit counts of pilha1 are kept in a temporary vector that is freed at the end.
void trocar(struct alocarga *carga, float *s, int pilha1, int pilha2, int **nvisitas, float *capcam, int **compatibilidade, int ncam)
{

        // Save the values of pilha1 in temporaries.
        int aux = carga[pilha1].equip;
        int auxop = carga[pilha1].op;
        int auxs = s[pilha1];
        int *vetor = criavetor(ncam);
        for (int k = 0; k < ncam; k++)
                vetor[k] = nvisitas[pilha1][k];

        // Copy pilha2 into pilha1.
        carga[pilha1].equip = carga[pilha2].equip;
        carga[pilha1].op = carga[pilha2].op;
        s[pilha1] = s[pilha2];
        for (int k = 0; k < ncam; k++)
                nvisitas[pilha1][k] = nvisitas[pilha2][k];

        // Copy the saved values of pilha1 into pilha2.
        carga[pilha2].equip = aux;
        carga[pilha2].op = auxop;
        s[pilha2] = auxs;
        for (int k = 0; k < ncam; k++)
                nvisitas[pilha2][k] = vetor[k];

        // Free the temporary visit vector.
        liberavetor(vetor);
}
//----------------------------------------------------------------------
// Adds (tipomov == false) or removes (tipomov == true) passo trips of truck caminhao at pile pilha when the
// result stays in range. Adding is limited by nviagem[pilha]; removing must not go below zero.
// *ok is set to true when the change was made and to false otherwise.
void trocarnvisitas(bool *ok, int **nvisitas, int *nviagem, int passo, bool tipomov, int pilha, int caminhao)
{
        // Assume failure until the change is checked.
        *ok = false;
        // Add the trips only if the total does not exceed nviagem[pilha].
        if (tipomov == false)
        {
                if (nvisitas[pilha][caminhao] + passo <= nviagem[pilha])
                {
                        nvisitas[pilha][caminhao] += passo;
                        *ok = true;
                }
        }
        else
        {
                // Remove the trips only if at least passo trips remain.
                if (nvisitas[pilha][caminhao] - passo >= 0)
                {
                        nvisitas[pilha][caminhao] += -passo;
                        *ok = true;
                }
        }
}
//----------------------------------------------------------------------
// Toggles the operation flag of pile pilha, then recomputes the production vector s.
void trocaroperacao(struct alocarga *carga, float *s, int pilha, int **nvisitas, float *capcam, int **compatibilidade, int ncam, int npilhas)
{

        if (carga[pilha].op == false)
                carga[pilha].op = true;
        else
                carga[pilha].op = false;

        // Recompute s after the change of the flag.
        carregavetors(s, nvisitas, carga, capcam, compatibilidade, ncam, npilhas);
}
//----------------------------------------------------------------------
// Moves passo trips at pile pilha from truck caminhao2 to truck caminhao1. No range check is made.
void trocarviagemcaminhaofrente(int **nvisitas, int pilha, int caminhao1, int caminhao2, int passo)
{
        nvisitas[pilha][caminhao1] += passo;
        nvisitas[pilha][caminhao2] -= passo;
}
//----------------------------------------------------------------------
// Moves passo trips of truck cam from pile pilha2 to pile pilha1. No range check is made.
void trocarviagemcaminhaocam(int **nvisitas, int cam, int pilha1, int pilha2, int passo)
{
        nvisitas[pilha1][cam] += passo;
        nvisitas[pilha2][cam] -= passo;
}
//----------------------------------------------------------------------
// Copies the loader assignment (equip and op) of the first tam entries of star into s.
// Production and visit counts are not copied.
void atualizamelhorsolucaoalocarga(struct alocarga *s, struct alocarga *star, int tam)
{
        for (int i = 0; i < tam; i++)
        {
                s[i].equip = star[i].equip;
                s[i].op = star[i].op;
        }
}
//----------------------------------------------------------------------
// Returns the number of trucks k compatible with the loader of pile pilha that make at least one trip to it
// (nvisitas != 0). Returns 0 for a pile without a loader. The nummax argument is not used.
int numcamfrente(int **nvisitas, struct alocarga *carga, int **compatibilidade, int ncam, int pilha, int *nummax)
{
        int soma = 0;
        // Pile without a loader: no trucks.
        if (carga[pilha].equip == disponivel)
                return 0;
        // Count the compatible trucks that visit this pile.
        for (int k = 0; k < ncam; k++)
        {
                if (compatibilidade[k][carga[pilha].equip] == compativel && nvisitas[pilha][k] != 0)
                {
                        soma++;
                }
        }
        return soma;
}
//----------------------------------------------------------------------
// Writes the report of one solution to the text file nomearq, created or overwritten. Sections: method
// (VND when metodo == 1, otherwise VNS), static or dynamic allocation (estdin), run time tempo, objective
// value fo (from favaliacao), ore, waste and REM summary, objective components, loader table and
// grade parameter table. The seed argument is not written. The file is closed before returning.
void imprimerelatorio(char nomearq[], int metodo, unsigned long seed, float tempo, float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem)
{
        register int i, j, k;
        FILE *arquivo;
        // Per-truck array; used only by the disabled truck table below and freed at the end.
        int *camutil = criavetor(ncam);
        // Local variables: viavel is assigned but never read; soma1, soma2, utilizacao, totalcam and auxcam
        // are not used by the active code.
        bool viavel;
        float enc = 0.0;
        float auxcam = 0.0;
        int totalcam = 0;
        float soma1 = 0.00000001;
        float soma2 = 0;
        float utilizacao;
        // Open the output file in write mode (creates or truncates it).
        arquivo = fopen(nomearq, "w");
        // Stop with an error if the file cannot be opened.
        if (!arquivo)
                Erro("O arquivo nome nao pode ser aberto");
        // Ore production (piles 0..npminerio-1) and total production (all piles).
        float producaomin = prodtotal(s, npminerio);
        float producaototal = prodtotal(s, npilhas);
        float rem = 0.0;
        long long fo = 0;
        float soma3 = 0.0;
        float soma4 = 0.0;

        // Evaluate the objective value fo of this solution with favaliacao.
        fo = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

        // soma4: for each grade parameter j, accumulates the absolute value of the sum over ore piles of
        // (teores[i][j] - rec[j]) * s[i]. It is computed but not printed.
        for (j = 0; j < nvar; j++)
        {
                for (i = 0; i < npminerio; i++)
                {
                        soma3 += ((teores[i][j] - rec[j]) * s[i]);
                }
                soma4 += absfloat(soma3);
                soma3 = 0;
        }

        // Header line: static allocation (estdin == 1) or dynamic allocation.
        if (estdin == 1)
                fprintf(arquivo, "Relat�rio para a alocacao ESTATICA de caminh�es utilizando o metodo ");
        else
                fprintf(arquivo, "Relat�rio para a alocacao DINAMICA de caminh�es utilizando o metodo ");

        // Method name: VND when metodo == 1, otherwise VNS.
        if (metodo == 1)
                fprintf(arquivo, "VND-Referencia");
        else
                fprintf(arquivo, "VNS");

        // fprintf(arquivo,"Seed: %d\nAbs: %d\nSoma: %f\nTempo: %f\nFo: %d",seed,abs(producaomin-prodrec),soma4,tempo,fo);
        // fprintf(arquivo,"\n\n\nSeed: %d\n",seed;
        // fprintf(arquivo,"Abs: %d",seed,abs(producaomin-prodrec),soma4,tempo,fo);
        // fprintf(arquivo,"Soma: %f\n",seed,abs(producaomin-prodrec),soma4,tempo,fo);
        // Print the run time and the objective value fo.
        fprintf(arquivo, "\n\n\nTempo: %f\n", tempo);
        fprintf(arquivo, "Fo: %lld\n\n\n", fo);

        // Ore and waste summary, printed only when some ore was produced (REM divides by the ore production).
        if (producaomin != 0)
        {
                fprintf(arquivo, "Producao de Minerio %8.2f  [%8.1f -%8.1f -%8.1f]\n", producaomin, prodmin, prodrec, prodmax);
                fprintf(arquivo, "Producao de Esteril %8.2f\n", producaototal - producaomin);
                // REM: waste production divided by ore production.
                rem = (producaototal - producaomin) / producaomin;
                fprintf(arquivo, "                REM %8.2f  [%8.2f -%8.2f -%8.2f]\n", rem, remmin, remrec, remmax);
        }
        /*else{
                    fprintf(arquivo,"N�o foi produzido minerio....\n");
                    liberavetor(camutil);
                    fclose(arquivo);
                    exit(1);
        }*/

        fprintf(arquivo, " \nComponentes da Funcao de Avaliacao\n\n\n");

        fprintf(arquivo, "\t Favaliacao  %12lld\n", fo);
        fprintf(arquivo, "\t--------------------------\n");

        // Objective components: each line below prints one fa_* term of the evaluation for this solution.
        fprintf(arquivo, "\t fa_producao %12lld\n", fa_producao(producaomin, prodmax, prodmin, prodrec, wfproducaoinv, wfproducaoqualp, wfproducaoqualn));
        // Grade term: fa_teor summed over all grade parameters, using the weighted average grade of each one.
        long long auxteor = 0;
        for (int j = 0; j < nvar; j++)
        {
                auxteor += fa_teor(encontrado(s, teores, npminerio, j), ls[j], li[j], rec[j], penalidadeteorinv[j], penalidadesup[j], penalidadeinf[j]);
        }
        fprintf(arquivo, "\t fa_teor     %12lld\n", auxteor);

        fprintf(arquivo, "\t fa_rem      %12lld\n", fa_rem(rem, remmax, remmin, remrec, wfrem, wfremqualp, wfremqualn));

        // Truck time-use term: fa_produtcam for each truck l, with produtcam(l) as value, 1.0 and 0.0 as the
        // max and min limits and dispmec[l] as the target.
        long long auxpcam = 0;
        for (int l = 0; l < ncam; l++)
        {
                auxpcam += fa_produtcam(produtcam(nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, npilhas, l), 1.0, 0.0, dispmec[l], wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn);
        }
        fprintf(arquivo, "\t fa_produtcam%12lld\n", auxpcam);

        // Loader production term: fa_prodcarga for each pile, with the max and min production of its loader.
        long long auxcarga = 0;
        for (int i = 0; i < npilhas; i++)
        {

                auxcarga += fa_prodcarga(s[i], cargamax[carga[i].equip], cargamin[carga[i].equip], wfprodcargainv, wfprodcargaqual);
        }
        fprintf(arquivo, "\t fa_prodcarga%12lld\n", auxcarga);
        // Number of trucks term (fa_numcam).
        fprintf(arquivo, "\t fa_totalcam %12lld\n", fa_numcam(nvisitas, carga, compatibilidade, npilhas, ncam, wfnumcamutilizados));

        // Static allocation term (fa_estatico).
        fprintf(arquivo, "\t fa_estatico %12lld\n", fa_estatico(nvisitas, carga, compatibilidade, npilhas, ncam, wfestatico));
        // Trucks per pile term (fa_numcamfrente), using nummax.
        fprintf(arquivo, "\t fa_numcamfre%12lld\n", fa_numcamfrente(nvisitas, carga, compatibilidade, npilhas, ncam, wfnumcam, nummax));
        fprintf(arquivo, "\t--------------------------\n");

        //******************************************************************************
        fprintf(arquivo, "\n\n\nEquipamentos de Carga\n\n\n");

        fprintf(arquivo, "\t N�    Frente   [Prod Min - Prod Max]   Ritmo de Lavra     Pontuacao   STATUS\n");
        fprintf(arquivo, "\t------------------------------------------------------------------------------\n");

        // Loader table: one line per pile that has a loader and nonzero production.
        for (i = 0; i < npilhas; i++)
        {
                if (carga[i].equip != disponivel && s[i] != 0)
                {
                        fprintf(arquivo, "\t %2d       %2d    [%8.1f  -%8.1f]     %8.1f    %10lld", carga[i].equip, i, cargamin[carga[i].equip], cargamax[carga[i].equip], s[i], fa_prodcarga(s[i], cargamax[carga[i].equip], cargamin[carga[i].equip], wfprodcargainv, wfprodcargaqual));
                        // Loader range check: ok when cargamin <= s[i] <= cargamax, otherwise INV.
                        if (s[i] >= cargamin[carga[i].equip] && s[i] <= cargamax[carga[i].equip])
                                fprintf(arquivo, "         ok!\n");
                        else
                        {
                                // Mark the solution as infeasible (viavel).
                                viavel = false;
                                fprintf(arquivo, "         INV\n");
                        }
                }
        }
        fprintf(arquivo, "\t------------------------------------------------------------------------------\n");

        //******************************************************************************
        fprintf(arquivo, "Par�metros de Controle\n\n\n");
        fprintf(arquivo, "\t Parametro [  Minimo -   Meta  -  Maximo]   Encontrado    Diferen�a    Pontuacao  STATUS\n");
        fprintf(arquivo, "\t-----------------------------------------------------------------------------------------\n");

        // Grade parameter table: one line per parameter j with min, target, max, weighted average, difference and term.
        for (j = 0; j < nvar; j++)
        {
                // Weighted average grade of parameter j over the ore piles.
                enc = encontrado(s, teores, npminerio, j);
                fprintf(arquivo, "\t  %2d       [%8.4f -%8.4f -%8.4f]   %8.4f     %8.4f %10lld", j, li[j], rec[j], ls[j], enc, enc - rec[j], fa_teor(encontrado(s, teores, npminerio, j), ls[j], li[j], rec[j], penalidadeteorinv[j], penalidadesup[j], penalidadeinf[j]));
                // Grade range check: ok when li[j] <= enc <= ls[j], otherwise INV.
                if (enc >= li[j] && enc <= ls[j])
                        fprintf(arquivo, "        ok!\n");
                else
                {
                        viavel = false;
                        fprintf(arquivo, "        INV\n");
                }
        }
        fprintf(arquivo, "\t-----------------------------------------------------------------------------------------\n");

        // From here to the closing line of this block comment is disabled code (the truck trip table).
        // It is inside a C block comment and is not compiled.
        /******************************************************************************
        fprintf(arquivo,"\n\n\nN�mero de viagens realizados �s frentes pelos caminh�es\n\n\n");

        for(int l = 0; l< ncam; l++ )
                camutil[l]=camutilizados(nvisitas,carga,compatibilidade, npilhas, l);



        fprintf(arquivo,"\n\tCaminhao |");

        for(int i=0;i<npilhas;i++){
                if(carga[i].equip!=disponivel && carga[i].op != false)
                if(s[i] != 0){
                        fprintf(arquivo,"%4d",i);
                }
        }
        fprintf(arquivo," |  Utilizacao  disponibilidade  Pontuacao  STATUS");

        fprintf(arquivo,"\n\t----------");
        for(int i=0;i<npilhas;i++){
                if(carga[i].equip!=disponivel && carga[i].op != false)
                if(s[i] != 0){
                        fprintf(arquivo,"----");
                }
        }
        fprintf(arquivo,"---------------------------------------------------");
        fprintf(arquivo,"\n");
        for(int k=0;k<ncam;k++){
                totalcam += camutil[k];
        }

        for(int k=0;k<ncam;k++){
                if(camutil[k]!=0){
                        fprintf(arquivo,"\t%4d     |", k);
                        for(int i=0;i<npilhas;i++){
                                if(carga[i].equip!=disponivel && carga[i].op != false)
                                if(s[i] != 0){
                                        if(compatibilidade[k][carga[i].equip]!=compativel ){
                                                fprintf(arquivo,"%4s", ".");
                                        }else{
                                                if(nvisitas[i][k]==0 ){
                                                        fprintf(arquivo,"%4s", ".");
                                                }else{
                                                        fprintf(arquivo,"%4d", nvisitas[i][k]);}
                                                }
                                        }
                                }
                                auxcam = produtcam(nvisitas,carga, capcam,compatibilidade, tciclo, tcontrole, npilhas, k);
                                fprintf(arquivo," |    %5.1f",100*auxcam);
                                fprintf(arquivo,"         %5.1f",100*dispmec[k]);
                                fprintf(arquivo,"     %8d",fa_produtcam(auxcam,1.0, 0.0, dispmec[k], wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn));

                               if(auxcam <= 1.0)
                                        fprintf(arquivo,"        ok!");
                               else{
                                        viavel = false;
                                        fprintf(arquivo,"        INV");
                                }

                                fprintf(arquivo," \n");

                }
        }
        fprintf(arquivo,"\t----------");
        for(int i=0;i<npilhas;i++){
                if(carga[i].equip!=disponivel && carga[i].op != false)
                if(s[i] != 0){
                        fprintf(arquivo,"----");
                }
        }
        fprintf(arquivo,"---------------------------------------------------");
        fprintf(arquivo,"\n");
        fprintf(arquivo,"\t Total de caminh�es %8d\n",totalcam);

        **********************************************************************/

        // Free the per-truck array.
        liberavetor(camutil);

        // Close the report file.
        fclose(arquivo);
}
//----------------------------------------------------------------------
// Returns 1 if truck cam makes at least one trip (nvisitas > 0) to an operational pile whose loader is
// compatible with it, otherwise 0.
int camutilizados(int **nvisitas, struct alocarga *carga, int **compatibilidade, int npilhas, int cam)
{
        // Stop at the first pile served by truck cam.
        for (int i = 0; i < npilhas; i++)
        {
                if (carga[i].equip != disponivel && carga[i].op == true)
                        if (compatibilidade[cam][carga[i].equip] == compativel)
                        {
                                if (nvisitas[i][cam] > 0)
                                {
                                        return 1;
                                }
                        }
        }
        return 0;
}
//----------------------------------------------------------------------
// Cycle time of each pile: tciclo[i] = tdv[i] + tdc[i] + tcarga[i] + tbasc[i] (sum of the four time arrays).
void carregatempociclo(int npilhas, float *tciclo, float *tdv, float *tdc, float *tcarga, float *tbasc)
{
        for (int i = 0; i < npilhas; i++)
        {
                tciclo[i] = tdv[i] + tdc[i] + tcarga[i] + tbasc[i];
        }
}

//----------------------------------------------------------------------
// nummax[i] = floor(tciclo[i] / tcarga[i]): number of loading times that fit in the cycle time, as an int.
void carreganummax(int npilhas, int *nummax, float *tciclo, float *tcarga)
{
        for (int i = 0; i < npilhas; i++)
                nummax[i] = (int)(floor(tciclo[i] / tcarga[i]));
}
//----------------------------------------------------------------------
// nviagem[i] = floor(tcontrole / tciclo[i]): number of cycles of pile i that fit in the control time, as an int.
void carreganviagem(int npilhas, int *nviagem, float tcontrole, float *tciclo)
{
        for (int i = 0; i < npilhas; i++)
                nviagem[i] = (int)(floor(tcontrole / tciclo[i]));
}
//----------------------------------------------------------------------

// Greedy initial solution (not called from other files in src). It works in two phases:
// 1) waste piles (npminerio..npilhas-1), largest available mass first, smallest loaders first, until the
//    waste target meta_esteril = prodrec * remrec is reached;
// 2) ore piles (0..npminerio-1), closest to the target grades first, largest loaders first, until the ore
//    target prodrec is reached. Trucks go from largest to smallest in phase 1 and from smallest to largest in phase 2.
// nvisitas entries are assigned, not accumulated. Temporary arrays are freed at the end.
void constroisolucaogulosa(float *s, struct alocarga *carga, float *capcam, int **compatibilidade, int **nvisitas, int **nviagem, int npilhas, int ncam, int nvar, float **teores, float *rec, int npminerio, float *QTDisp, float *cargamax, float *cargamin, int ncarga, float prodrec, float remrec)
{
    // Flags of the loaders already assigned to a pile; calloc sets them all to false.
    bool *loader_used = (bool*)calloc(ncarga, sizeof(bool));
    
    // 1. Sorting of loaders (smallest first)
    // Loader ids, sorted below by cargamax.
    int *sorted_loaders = (int*)malloc(ncarga * sizeof(int));

    float prod_minerio_atual = 0.0;
    float prod_esteril_atual = 0.0;
    // Waste target: meta_esteril = prodrec * remrec, since REM is waste divided by ore.
    float meta_esteril = prodrec * remrec; // Ideal waste proportion based on the REM

    // Start with loader ids 0..ncarga-1.
    for(int i=0; i<ncarga; i++){sorted_loaders[i] = i;}
    // Exchange sort of the loader ids by cargamax, ascending.
    for(int i=0; i<ncarga-1; i++)
    {
        for(int j=i+1; j<ncarga; j++) 
        {
            if(cargamax[sorted_loaders[i]] > cargamax[sorted_loaders[j]]) 
            {
                int temp = sorted_loaders[i];
                sorted_loaders[i] = sorted_loaders[j];
                sorted_loaders[j] = temp;
            }
        }
    }

    // 2. Sorting of trucks (largest first)
    // Truck ids, sorted below by capacity.
    int *sorted_trucks = (int*)malloc(ncam * sizeof(int));
    for(int i=0; i<ncam; i++) {sorted_trucks[i] = i;}
    // Exchange sort of the truck ids by capacity, descending.
    for(int i=0; i<ncam-1; i++)
    {
        for(int j=i+1; j<ncam; j++) 
        {
            if(capcam[sorted_trucks[i]] < capcam[sorted_trucks[j]]) 
            {
                int temp = sorted_trucks[i];
                sorted_trucks[i] = sorted_trucks[j];
                sorted_trucks[j] = temp;
            }
        }
    }

    // --- 1: WASTE (piles from npminerio to npilhas-1) ---
    // Number of waste piles (indices npminerio..npilhas-1).
    int num_est = npilhas - npminerio;
    if (num_est > 0) 
    {
        // Array of waste pile indices.
        int *est_faces = (int*)malloc(num_est * sizeof(int));
        for(int i=0; i<num_est; i++){est_faces[i] = npminerio + i;}
        
        // Sort the waste piles by available mass (QTDisp), descending
        for(int i=0; i<num_est-1; i++) 
        {
            for(int j=i+1; j<num_est; j++) 
            {
                if(QTDisp[est_faces[i]] < QTDisp[est_faces[j]]) 
                {
                    int temp = est_faces[i]; est_faces[i] = est_faces[j]; est_faces[j] = temp;
                }
            }
        }

        int l_idx = 0; // Smallest loader
        // Visit the waste piles in order of decreasing available mass.
        for(int i=0; i<num_est; i++) 
        {
            if (prod_esteril_atual >= meta_esteril) break; // Enough waste for the REM

            // Waste pile to fill next.
            int f = est_faces[i];
            // Advance to the next loader that is not yet used.
            while(l_idx < ncarga && loader_used[sorted_loaders[l_idx]]) {l_idx++;}
            // If a free loader exists, place it on pile f and assign trucks to it.
            if(l_idx < ncarga)
            {
                int l_id = sorted_loaders[l_idx];
                carga[f].equip = l_id;
                loader_used[l_id] = true;
                
                // Waste phase: visit with the largest compatible trucks first.
                // Waste: starts with the largest trucks
                for(int j=0; j<ncam; j++) 
                {
                    int c = sorted_trucks[j];
                    // Only trucks compatible with this loader.
                    if(compatibilidade[c][l_id] == compativel) 
                    {
                        // Waste still needed to reach the target.
                        float falta_esteril = meta_esteril - prod_esteril_atual;
                        // Target reached: stop assigning trucks.
                        if (falta_esteril <= 0) break;

                        // Maximum trips for this pile and truck, from the nviagem matrix.
                        int v = nviagem[f][c];
                        // Limit the trips so the pile does not exceed its available mass QTDisp[f].
                        if(s[f] + v*capcam[c] > QTDisp[f]) v = (int)((QTDisp[f] - s[f]) / capcam[c]);
                        // Limit the trips to the waste still needed, rounded up to whole trips.
                        if(v * capcam[c] > falta_esteril) v = (int)ceil(falta_esteril / capcam[c]);

                        // Record the trips and add their production to the pile and to the waste total.
                        nvisitas[f][c] = v;
                        s[f] += v * capcam[c];
                        prod_esteril_atual += v * capcam[c];
                    }
                }
            }
        }
        // Free the waste pile index array.
        free(est_faces);
    }

    // --- 2: ORE (piles from 0 to npminerio-1) ---
    // Ore phase (skipped when there are no ore piles).
    if (npminerio > 0) 
    {
        // Ore pile indices and their grade deviations.
        int *ore_faces = (int*)malloc(npminerio * sizeof(int));
        float *deviations = (float*)malloc(npminerio * sizeof(float));
        // Deviation of each ore pile: sum over grade parameters j of |teores[i][j] - rec[j]|.
        for(int i=0; i<npminerio; i++) 
        {
            ore_faces[i] = i;
            deviations[i] = 0;
            for(int j=0; j<nvar; j++){deviations[i] += absfloat(teores[i][j] - rec[j]);}
        }
        
        // Sort by smallest deviation
        // Sort the ore piles by ascending deviation (exchange sort).
        for(int i=0; i<npminerio-1; i++) 
        {
            for(int j=i+1; j<npminerio; j++) 
            {
                if(deviations[i] > deviations[j]) 
                {
                    float td = deviations[i]; deviations[i] = deviations[j]; deviations[j] = td;
                    int tf = ore_faces[i]; ore_faces[i] = ore_faces[j]; ore_faces[j] = tf;
                }
            }
        }

        int l_idx = ncarga - 1; // Largest loader
        // Visit the ore piles in order of ascending deviation.
        for(int i=0; i<npminerio; i++) 
        {
            // Ore target reached: stop.
            if (prod_minerio_atual >= prodrec) break; // Ore target reached

            int f = ore_faces[i];
            // Move from the largest loader downward to the next unused one.
            while(l_idx >= 0 && loader_used[sorted_loaders[l_idx]]) {l_idx--;}
            if(l_idx >= 0) 
            {
                int l_id = sorted_loaders[l_idx];
                carga[f].equip = l_id;
                loader_used[l_id] = true;
                
                // Ore: truck of smallest capacity
                // Ore phase: visit with the smallest compatible trucks first.
                for(int j=ncam-1; j>=0; j--) 
                {
                    int c = sorted_trucks[j];
                    if(compatibilidade[c][l_id] == compativel) 
                    {
                        // Ore still needed to reach the target.
                        float falta_para_meta = prodrec - prod_minerio_atual;
                        // Target reached: stop assigning trucks.
                        if (falta_para_meta <= 0) break;

                        // Maximum trips for this pile and truck, from the nviagem matrix.
                        int v = nviagem[f][c];
                        // Limit by the mass of the pile
                        if(s[f] + v*capcam[c] > QTDisp[f]) v = (int)((QTDisp[f] - s[f]) / capcam[c]);
                        // Limit by the production target
                        if(v * capcam[c] > falta_para_meta) v = (int)ceil(falta_para_meta / capcam[c]);

                        // Record the trips and update the pile production and the ore total.
                        nvisitas[f][c] = v;
                        s[f] += v * capcam[c];
                        prod_minerio_atual += v * capcam[c];
                    }
                }
            }
        }
        // Free the ore arrays.
        free(ore_faces);
        free(deviations);
    }

    // Recompute the production vector s from the visit counts.
    carregavetors(s, nvisitas, carga, capcam, compatibilidade, ncam, npilhas);

    // Free the temporary arrays.
    free(loader_used); 
    free(sorted_loaders); 
    free(sorted_trucks);
}


// GRASP initial solution: a randomized version of constroisolucaogulosa. Each choice is drawn at random from a
// restricted candidate list (RCL) made of the first ceil(remaining * alpha) candidates, at least one.
// alpha = 1 gives the full list; small alpha behaves almost greedily. Waste is handled first, then ore.
// Uses rand(), so the caller must seed it with srand. Clears s and nvisitas first; temporary arrays are freed.
void constroisolucaoGRASP(float *s, struct alocarga *carga, float *capcam, int **compatibilidade, int **nvisitas, int **nviagem, int npilhas, int ncam, int nvar, float **teores, float *rec, int npminerio, float *QTDisp, float *cargamax, float *cargamin, int ncarga, float prodrec, float remrec, float alpha)
{
    // 1. Cleanup and initialization
    // Clear the production vector s and the visit matrix nvisitas.
    iniciavetorfloat(s, npilhas, 0.0);
    iniciamatrizint(nvisitas, npilhas, ncam, 0);
    // Flags of the loaders already assigned; all false at the start.
    bool *loader_used = (bool*)calloc(ncarga, sizeof(bool));
    // Running totals of ore and waste production, and the waste target.
    float prod_minerio_atual = 0.0;
    float prod_esteril_atual = 0.0;
    float meta_esteril = prodrec * remrec;

    // 2. Sorting of loaders (smallest first)
    // Loader ids, sorted below by cargamax.
    int *sorted_loaders = (int*)malloc(ncarga * sizeof(int));
    for(int i=0; i<ncarga; i++){sorted_loaders[i] = i;}
    // Exchange sort of the loader ids by cargamax, ascending.
    for(int i=0; i<ncarga-1; i++) {
        for(int j=i+1; j<ncarga; j++) {
            if(cargamax[sorted_loaders[i]] > cargamax[sorted_loaders[j]]) {
                int temp = sorted_loaders[i];
                sorted_loaders[i] = sorted_loaders[j];
                sorted_loaders[j] = temp;
            }
        }
    }

    // 3. Sorting of trucks (largest first)
    // Truck ids, sorted below by capacity.
    int *sorted_trucks = (int*)malloc(ncam * sizeof(int));
    for(int i=0; i<ncam; i++) {sorted_trucks[i] = i;}
    // Exchange sort of the truck ids by capacity, descending.
    for(int i=0; i<ncam-1; i++) {
        for(int j=i+1; j<ncam; j++) {
            if(capcam[sorted_trucks[i]] < capcam[sorted_trucks[j]]) {
                int temp = sorted_trucks[i];
                sorted_trucks[i] = sorted_trucks[j];
                sorted_trucks[j] = temp;
            }
        }
    }

    // --- GRASP PHASE: WASTE ---
    // Number of waste piles.
    int num_est = npilhas - npminerio;
    if (num_est > 0) 
    {
        // Array of waste pile indices, initially npminerio..npilhas-1.
        int *est_faces = (int*)malloc(num_est * sizeof(int));
        for(int i=0; i<num_est; i++){est_faces[i] = npminerio + i;}
        
        // Sort the waste piles by available mass (QTDisp), descending
        for(int i=0; i<num_est-1; i++) {
            for(int j=i+1; j<num_est; j++) {
                if(QTDisp[est_faces[i]] < QTDisp[est_faces[j]]) {
                    int temp = est_faces[i]; est_faces[i] = est_faces[j]; est_faces[j] = temp;
                }
            }
        }

        // Index of the smallest loader not yet used.
        int l_idx = 0; 
        for(int i=0; i<num_est; i++) 
        {
            // Waste target reached: stop.
            if (prod_esteril_atual >= meta_esteril) break;

            // GRASP APPLICATION: random draw from the RCL of waste piles
            // GRASP choice: the candidate list holds the first range_lrc unassigned waste piles, where
            // range_lrc = ceil(remaining * alpha), at least 1. One is picked at random with rand().
            int restante = num_est - i;
            int range_lrc = (int)ceil(restante * alpha);
            if (range_lrc < 1) range_lrc = 1;
            int escolhido_idx = i + (rand() % range_lrc);
            
            int f = est_faces[escolhido_idx];
            // Swap so that the used pile is kept out of the next draw
            // Move the chosen pile to position i so it is not picked again.
            int temp_f = est_faces[i]; est_faces[i] = est_faces[escolhido_idx]; est_faces[escolhido_idx] = temp_f;

            // Advance to the next unused loader, in ascending cargamax order.
            while(l_idx < ncarga && loader_used[sorted_loaders[l_idx]]) {l_idx++;}
            if(l_idx < ncarga)
            {
                // Place loader l_id on pile f and mark it used.
                int l_id = sorted_loaders[l_idx];
                carga[f].equip = l_id;
                loader_used[l_id] = true;
                
                // Assign trucks from the largest capacity to the smallest.
                for(int j=0; j<ncam; j++) 
                {
                    int c = sorted_trucks[j];
                    // Only trucks compatible with this loader (compatibilidade value 1, the same as compativel).
                    if(compatibilidade[c][l_id] == 1) 
                    {
                        // Waste still needed to reach the target.
                        float falta_esteril = meta_esteril - prod_esteril_atual;
                        // Target reached: stop assigning trucks.
                        if (falta_esteril <= 0) break;

                        // Maximum trips for this pile and truck, from the nviagem matrix.
                        int v = nviagem[f][c];
                        // Limit the trips by the available mass of the pile.
                        if(s[f] + v*capcam[c] > QTDisp[f]) v = (int)((QTDisp[f] - s[f]) / capcam[c]);
                        // Limit the trips to the waste still needed, rounded up to whole trips.
                        if(v * capcam[c] > falta_esteril) v = (int)ceil(falta_esteril / capcam[c]);

                        // Record the trips and update the production totals.
                        nvisitas[f][c] = v;
                        s[f] += v * capcam[c];
                        prod_esteril_atual += v * capcam[c];
                    }
                }
            }
        }
        // Free the waste temporaries.
        free(est_faces);
    }

    // --- GRASP PHASE: ORE ---
    // Ore phase (skipped when there are no ore piles).
    if (npminerio > 0) 
    {
        // Ore pile indices and their grade deviations.
        int *ore_faces = (int*)malloc(npminerio * sizeof(int));
        float *deviations = (float*)malloc(npminerio * sizeof(float));
        // Deviation of each ore pile: sum over grade parameters j of |teores[i][j] - rec[j]|.
        for(int i=0; i<npminerio; i++) 
        {
            ore_faces[i] = i;
            deviations[i] = 0;
            for(int j=0; j<nvar; j++){deviations[i] += absfloat(teores[i][j] - rec[j]);}
        }
        
        // Sort by smallest deviation (greedy)
        // Sort the ore piles by ascending deviation.
        for(int i=0; i<npminerio-1; i++) {
            for(int j=i+1; j<npminerio; j++) {
                if(deviations[i] > deviations[j]) {
                    float td = deviations[i]; deviations[i] = deviations[j]; deviations[j] = td;
                    int tf = ore_faces[i]; ore_faces[i] = ore_faces[j]; ore_faces[j] = tf;
                }
            }
        }

        int l_idx = ncarga - 1; // Largest loader
        // Visit the ore piles in deviation order, with a GRASP choice at each step.
        for(int i=0; i<npminerio; i++) 
        {
            // Ore target reached: stop.
            if (prod_minerio_atual >= prodrec) break;

            // GRASP APPLICATION: random draw from the RCL of ore piles
            // GRASP choice among the remaining ore piles, same rule as in the waste phase.
            int restante = npminerio - i;
            int range_lrc = (int)ceil(restante * alpha);
            if (range_lrc < 1) range_lrc = 1;
            int escolhido_idx = i + (rand() % range_lrc);
            
            int f = ore_faces[escolhido_idx];
            // Move the chosen pile to position i.
            int temp_f = ore_faces[i]; ore_faces[i] = ore_faces[escolhido_idx]; ore_faces[escolhido_idx] = temp_f;

            // Move from the largest loader downward to the next unused one.
            while(l_idx >= 0 && loader_used[sorted_loaders[l_idx]]) {l_idx--;}
            if(l_idx >= 0) 
            {
                int l_id = sorted_loaders[l_idx];
                // Place loader l_id on pile f and mark it used.
                carga[f].equip = l_id;
                loader_used[l_id] = true;
                
                // Assign trucks from the smallest capacity to the largest.
                for(int j=ncam-1; j>=0; j--) 
                {
                    int c = sorted_trucks[j];
                    // Only trucks compatible with this loader (value 1, the same as compativel).
                    if(compatibilidade[c][l_id] == 1) 
                    {
                        // Ore still needed to reach the target.
                        float falta_para_meta = prodrec - prod_minerio_atual;
                        // Target reached: stop assigning trucks.
                        if (falta_para_meta <= 0) break;

                        // Maximum trips for this pile and truck, from the nviagem matrix.
                        int v = nviagem[f][c];
                        // Limit the trips by the available mass of the pile.
                        if(s[f] + v*capcam[c] > QTDisp[f]) v = (int)((QTDisp[f] - s[f]) / capcam[c]);
                        // Limit the trips to the ore still needed, rounded up to whole trips.
                        if(v * capcam[c] > falta_para_meta) v = (int)ceil(falta_para_meta / capcam[c]);

                        // Record the trips and update the production totals.
                        nvisitas[f][c] = v;
                        s[f] += v * capcam[c];
                        prod_minerio_atual += v * capcam[c];
                    }
                }
            }
        }
        // Free the ore temporaries.
        free(ore_faces);
        free(deviations);
    }

    // Free the remaining temporaries.
    free(loader_used); 
    free(sorted_loaders); 
    free(sorted_trucks);
}

// Partial GRASP rebuild. If reconstruir_apenas_minerio is true, only the ore piles (0..npminerio-1) are rebuilt
// and the waste piles keep their current loaders and production. Otherwise the whole solution is rebuilt,
// as in constroisolucaoGRASP. The ore piles are always rebuilt. Temporary arrays are freed at the end.
void constroisolucaoGRASP_Parcial(float *s, struct alocarga *carga, float *capcam, int **compatibilidade, int **nvisitas, int **nviagem, int npilhas, int ncam, int nvar, float **teores, float *rec, int npminerio, float *QTDisp, float *cargamax, float *cargamin, int ncarga, float prodrec, float remrec, float alpha, bool reconstruir_apenas_minerio)
{
    // Flags of the loaders already assigned; all false at the start.
    bool *loader_used = (bool*)calloc(ncarga, sizeof(bool));
    // Sorted loader and truck id lists, filled below.
    int *sorted_loaders = (int*)malloc(ncarga * sizeof(int));
    int *sorted_trucks = (int*)malloc(ncam * sizeof(int));
    
    // If only the ore is being rebuilt, the waste loaders must be marked as used
    // Partial mode: the loaders of the waste piles are marked as used, so the ore phase cannot take them.
    if(reconstruir_apenas_minerio) {
        for(int i = npminerio; i < npilhas; i++) {
            if(carga[i].equip != -1) loader_used[carga[i].equip] = true;
        }
    } else {
        // Full rebuild: clear the production vector and the visit matrix.
        iniciavetorfloat(s, npilhas, 0.0);
        iniciamatrizint(nvisitas, npilhas, ncam, 0);
    }

    // --- Base sorts ---
    // Sort the loader ids by cargamax (ascending) and the truck ids by capacity (descending).
    for(int i=0; i<ncarga; i++){sorted_loaders[i] = i;}
    for(int i=0; i<ncarga-1; i++) {
        for(int j=i+1; j<ncarga; j++) {
            if(cargamax[sorted_loaders[i]] > cargamax[sorted_loaders[j]]) {
                int temp = sorted_loaders[i]; sorted_loaders[i] = sorted_loaders[j]; sorted_loaders[j] = temp;
            }
        }
    }
    for(int i=0; i<ncam; i++) {sorted_trucks[i] = i;}
    for(int i=0; i<ncam-1; i++) {
        for(int j=i+1; j<ncam; j++) {
            if(capcam[sorted_trucks[i]] < capcam[sorted_trucks[j]]) {
                int temp = sorted_trucks[i]; sorted_trucks[i] = sorted_trucks[j]; sorted_trucks[j] = temp;
            }
        }
    }

    // --- WASTE BLOCK (ONLY RUNS IF NOT A PARTIAL REBUILD) ---
    // Waste phase: runs only in full rebuild mode.
    if (!reconstruir_apenas_minerio) {
        // Waste target and running waste production.
        float prod_esteril_atual = 0.0;
        float meta_esteril = prodrec * remrec;
        int num_est = npilhas - npminerio;
        if (num_est > 0) {
            // Waste pile indices npminerio..npilhas-1.
            int *est_faces = (int*)malloc(num_est * sizeof(int));
            for(int i=0; i<num_est; i++){est_faces[i] = npminerio + i;}
            // Sort by descending mass
            for(int i=0; i<num_est-1; i++) {
                for(int j=i+1; j<num_est; j++) {
                    if(QTDisp[est_faces[i]] < QTDisp[est_faces[j]]) {
                        int temp = est_faces[i]; est_faces[i] = est_faces[j]; est_faces[j] = temp;
                    }
                }
            }
            // Index of the smallest loader not yet used.
            int l_idx = 0; 
            for(int i=0; i<num_est; i++) {
                // Waste target reached: stop.
                if (prod_esteril_atual >= meta_esteril) break;
                // Size of the candidate list: ceil(remaining * alpha), at least 1.
                int r_lrc = (int)ceil((num_est - i) * alpha); if (r_lrc < 1) r_lrc = 1;
                // Random pick within the candidate list.
                int f_idx = i + (rand() % r_lrc);
                int f = est_faces[f_idx];
                // Move the chosen pile to position i.
                int temp_f = est_faces[i]; est_faces[i] = est_faces[f_idx]; est_faces[f_idx] = temp_f;

                // Advance to the next unused loader.
                while(l_idx < ncarga && loader_used[sorted_loaders[l_idx]]) {l_idx++;}
                if(l_idx < ncarga) {
                    // Place loader l_id on pile f and mark it used.
                    int l_id = sorted_loaders[l_idx];
                    carga[f].equip = l_id; loader_used[l_id] = true;
                    // Visit with the trucks from largest to smallest capacity, compatible ones only.
                    for(int j=0; j<ncam; j++) {
                        int c = sorted_trucks[j];
                        if(compatibilidade[c][l_id] == 1) {
                            // Waste still needed to reach the target.
                            float falta = meta_esteril - prod_esteril_atual;
                            // Target reached: stop assigning trucks.
                            if (falta <= 0) break;
                            // Start from the maximum trips for this pile and truck; they are limited below by mass and by the target.
                            int v = nviagem[f][c];
                            // Limit the trips by the available mass of the pile.
                            if(s[f] + v*capcam[c] > QTDisp[f]) v = (int)((QTDisp[f] - s[f]) / capcam[c]);
                            // Limit the trips to the waste still needed, rounded up to whole trips.
                            if(v * capcam[c] > falta) v = (int)ceil(falta / capcam[c]);
                            // Record the trips and update the production totals.
                            nvisitas[f][c] = v; s[f] += v * capcam[c]; prod_esteril_atual += v * capcam[c];
                        }
                    }
                }
            }
            // Free the waste temporaries.
            free(est_faces);
        }
    }

    // --- ORE BLOCK (ALWAYS REBUILT) ---
    // Ore phase, always rebuilt. Running total of ore production.
    float prod_min_atual = 0.0;
    // In partial mode, only the ore piles need to be reset
    // Reset the ore piles: production, visit counts and loader (-1 means no loader).
    for(int i=0; i < npminerio; i++) {
        s[i] = 0;
        for(int j=0; j < ncam; j++) nvisitas[i][j] = 0;
        carga[i].equip = -1;
    }

    // Ore pile indices and their grade deviations.
    int *ore_faces = (int*)malloc(npminerio * sizeof(int));
    float *deviations = (float*)malloc(npminerio * sizeof(float));
    // Deviation of each ore pile: sum over grade parameters j of |teores[i][j] - rec[j]|.
    for(int i=0; i<npminerio; i++) {
        ore_faces[i] = i; deviations[i] = 0;
        for(int j=0; j<nvar; j++){ deviations[i] += absfloat(teores[i][j] - rec[j]); }
    }
    // Sort by smallest deviation
    for(int i=0; i<npminerio-1; i++) {
        for(int j=i+1; j<npminerio; j++) {
            if(deviations[i] > deviations[j]) {
                float td = deviations[i]; deviations[i] = deviations[j]; deviations[j] = td;
                int tf = ore_faces[i]; ore_faces[i] = ore_faces[j]; ore_faces[j] = tf;
            }
        }
    }
    // Index of the largest loader not yet used.
    int l_idx_ore = ncarga - 1;
    // Assign the ore piles in order of ascending deviation, with a GRASP choice at each step.
    for(int i=0; i<npminerio; i++) {
        // Ore target reached: stop.
        if (prod_min_atual >= prodrec) break;
        // Size of the candidate list: ceil(remaining * alpha), at least 1.
        int r_lrc = (int)ceil((npminerio - i) * alpha); if (r_lrc < 1) r_lrc = 1;
        // Random pick within the candidate list.
        int f_idx = i + (rand() % r_lrc);
        int f = ore_faces[f_idx];
        // Move the chosen pile to position i.
        int temp_f = ore_faces[i]; ore_faces[i] = ore_faces[f_idx]; ore_faces[f_idx] = temp_f;

        // Move from the largest loader downward to the next unused one.
        while(l_idx_ore >= 0 && loader_used[sorted_loaders[l_idx_ore]]) {l_idx_ore--;}
        if(l_idx_ore >= 0) {
            int l_id = sorted_loaders[l_idx_ore];
            // Place loader l_id on pile f and mark it used.
            carga[f].equip = l_id; loader_used[l_id] = true;
            // Assign trucks from the smallest capacity to the largest, compatible ones only.
            for(int j=ncam-1; j>=0; j--) {
                int c = sorted_trucks[j];
                if(compatibilidade[c][l_id] == 1) {
                    // Ore still needed to reach the target.
                    float falta = prodrec - prod_min_atual;
                    // Target reached: stop assigning trucks.
                    if (falta <= 0) break;
                    // Maximum trips for this pile and truck, from the nviagem matrix.
                    int v = nviagem[f][c];
                    // Limit the trips by the available mass of the pile.
                    if(s[f] + v*capcam[c] > QTDisp[f]) v = (int)((QTDisp[f] - s[f]) / capcam[c]);
                    // Limit the trips to the ore still needed, rounded up to whole trips.
                    if(v * capcam[c] > falta) v = (int)ceil(falta / capcam[c]);
                    // Record the trips and update the production totals.
                    nvisitas[f][c] = v; s[f] += v * capcam[c]; prod_min_atual += v * capcam[c];
                }
            }
        }
    }
    // Free all temporary arrays.
    free(ore_faces); free(deviations); free(loader_used); free(sorted_loaders); free(sorted_trucks);
}