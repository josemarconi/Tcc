#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <time.h>
#include <float.h>
#include <errno.h>
#include <math.h>
#include <chrono>

// Project headers: utilities, the GRASP construction (Hutilcarga.h), the evaluation function,
// VNS, the genetic algorithm (Hag.h) and the shared alocarga struct.
#include "Hutil.h"
#include "Hdefine.h"
#include "Hutilcarga.h"
#include "Hfavaliacao.h"
#include "Hmovimento.h"
#include "Hsa.h"
#include "Hvnd.h"
#include "Hvns.h"
#include "hstruct.h"
#include "Hils.h"
#include "Hag.h"

using namespace std;

// Returns m from the "m/e frentes" field of the "Mina <id>:" line in ../dados/minas.txt
// (the number before the '/'), used as npminerio. Returns -1 if the file or the mine line is missing.
int descobrir_npminerio(int id_mina)
{
    // Open ../dados/minas.txt (one line per mine).
    FILE *f = fopen("../dados/minas.txt", "r");
    if (!f) return -1;

    char linha[512];
    char busca[512];
    // Build the search key "Mina <id>:"; the colon stops "Mina 1" from matching "Mina 10".
    sprintf(busca, "Mina %d:", id_mina);

    // Read the file line by line until the line of this mine is found.
    while (fgets(linha, sizeof(linha), f))
    {
        if (strstr(linha, busca))
        {
            int m, e;
            // Find the first '/' on the matching line; the number before it is m.
            char *ptr = strchr(linha, '/');
            if (ptr)
            {
                // Walk back over the digits in front of the '/' to find where that number starts.
                char *start = ptr - 1;
                while (start > linha && isdigit(*(start - 1))) start--;
                // Parse "m/e" from that position; only m is kept.
                sscanf(start, "%d/%d", &m, &e);
                fclose(f);
                return m;
            }
        }
    }
    // Reached the end of the file without a match.
    fclose(f);
    return -1;
}

// Returns the number of non-empty lines of a file. Used to get the problem sizes from the DAT files.
// Returns 0 if the file cannot be opened.
int get_num_linhas(char *nomearq)
{
    FILE *arquivo = fopen(nomearq, "r");
    if (!arquivo) return 0;

    int linhas = 0;
    char ch;
    int conteudo = 0;

    // Read one character at a time; a line counts only if it has at least one character before its newline.
    while ((ch = fgetc(arquivo)) != EOF)
    {
        if (ch != '\n') conteudo = 1;
        if (ch == '\n')
        {
            if (conteudo) linhas++;
            conteudo = 0;
        }
    }
    // A last line without a trailing newline also counts.
    if (conteudo) linhas++;
    fclose(arquivo);
    return linhas;
}

// Reads prod_<id>.DAT: line 1 is the REM values (waste-to-ore ratio) as min,rec,max;
// line 2 is the production values as min,rec,max. Values may be comma- or space-separated.
// Exits if the file cannot be opened.
void ler_arquivo_producao(char *nomearq, float *remmin, float *remrec, float *remmax, float *prodmin, float *prodrec, float *prodmax)
{
    FILE *f = fopen(nomearq, "r");
    if (!f) { printf("Erro ao abrir %s\n", nomearq); exit(1); }

    // Try the comma-separated format for the REM line; if it does not match, rewind and read it as space-separated.
    if (fscanf(f, "%f,%f,%f", remmin, remrec, remmax) != 3) {
        rewind(f);
        fscanf(f, "%f %f %f", remmin, remrec, remmax);
    }
    // Read the production line the same way, with the space-separated fallback.
    if (fscanf(f, "%f,%f,%f", prodmin, prodrec, prodmax) != 3)
        fscanf(f, "%f %f %f", prodmin, prodrec, prodmax);

    fclose(f);
}

// Reads tciclo_<id>.DAT, records "truck,pile,ignored,cycle time", into tciclo[pile][truck].
// The matrix must already be allocated (npilhas x ncam). Exits if the file cannot be opened.
void ler_tciclo_matriz(char *nomearq, float **tciclo, int npilhas, int ncam)
{
    FILE *f = fopen(nomearq, "r");
    if (!f) { printf("Erro ao abrir %s\n", nomearq); exit(1); }

    // Zero the whole matrix so that pile/truck pairs without a record have cycle time 0.
    iniciamatrizfloat(tciclo, npilhas, ncam, 0.0);
    int cam, pilha, aux;
    float val;

    // One record per iteration (4 fields); the loop ends at the first line that does not match.
    while (fscanf(f, "%d,%d,%d,%f", &cam, &pilha, &aux, &val) == 4) {
        // Store the cycle time only if it is below the 9000 placeholder limit (99999 in the data is skipped).
        if (val < 9000) tciclo[pilha][cam] = val;
    }
    fclose(f);
}

// Program entry point. For each of 20 test runs, for each of 3 methods and for each mine id 1..290 it:
// reads the instance from ../dados/Minas/Mina_<id>/, builds an initial solution with GRASP, improves it
// (VNS for methods 1 and 2, genetic algorithm for method 3), evaluates it and appends one row to the
// method's CSV. Paths are relative, so the program must be run from the src folder.
int main()
{
    // Outer loop: Iterations (Tests 1 to 5)
    // Outer loop over the 20 test runs; j is the run number used in the output names and seeds.
    for (int j = 1; j <= 20; j++)
    {
        int numero_da_execucao = j;

        // Inner loop: Methods (1, 2 and 3)
        // Inner loop over the methods: 1 and 2 use VNS (output folders VNS-Referencia and VNS-RLP), 3 uses the genetic algorithm.
        for (int metodo = 1; metodo <= 3; metodo++)
        {
            // Base folder of the instance data; each mine is a Mina_<id> subfolder.
            char path_base[] = "../dados/Minas/";
            char buffer[512];
            char arquivo_name[512];

            // Objective weights (wf...) for the evaluation terms; the same values are passed to every method.
            int wfproducaoinv = 1000000, wfproducaoqualp = 1000, wfproducaoqualn = 1000;
            int wfteor = 500000, wfrem = 100000, wfremqualp = 1, wfremqualn = 1;
            int wfprodutcaminv = 1000, wfprodutcamqualp = 1, wfprodutcamqualn = 1;
            int wfprodcargainv = 1000, wfprodcargaqual = 1, wfproducaocamtotal = 1;
            int wfproducaocarga = 1, wfnumcamutilizados = 1, wfestatico = 10000;
            int wfnumcam = 10000, wfdist = 1;
            
            const float alpha_vns = 0.3741f;

            // Choose the output CSV of this method.
            switch (metodo)
            {
                case 1:
                    sprintf(arquivo_name, "Relatorios_Finais/VNS-Referencia/Resumo_Geral_Minas_VNS-Referencia_Teste_%d.csv", numero_da_execucao);
                    break;
                case 2:
                    sprintf(arquivo_name, "Relatorios_Finais/VNS-RLP/Resumo_Geral_Minas_VNS-RLP_Teste_%d.csv", numero_da_execucao);
                    break;
                case 3:
                    sprintf(arquivo_name, "Relatorios_Finais/AG-RLP/Resumo_Geral_Minas_AG-RLP_Teste_%d.csv", numero_da_execucao);
                    break;
            }

            // Create or overwrite the method's CSV and write its header line; one row per mine is appended below.
            FILE *f_resumo = fopen(arquivo_name, "w");
            // Stop with return 1 if the output folder does not exist.
            if (!f_resumo) {
                printf("Erro: Certifique-se de que a estrutura de pastas 'Relatorios_Finais/...' existe.\n");
                return 1;
            }
            fprintf(f_resumo, "Mina;Fo;Tempo(s);Iter\n");
            fclose(f_resumo);

            // Loop over mine ids 1 to 290.
            for (int id_mina = 1; id_mina <= 290; id_mina++)
            {
                printf("\n[Teste %d/20] Metodo %d | Processando Mina %d...\n", numero_da_execucao, metodo, id_mina);

                // Mines without a quant_<id>.DAT file are skipped and produce no row.
                sprintf(buffer, "%sMina_%d/quant_%d.DAT", path_base, id_mina, id_mina);
                FILE *teste = fopen(buffer, "r");
                if (!teste) continue;
                fclose(teste);

                // Problem sizes, from the number of lines of each file: npilhas (piles), ncam (trucks),
                // ncarga (loaders), nvar (quality variables).
                int npilhas = get_num_linhas(buffer);
                sprintf(buffer, "%sMina_%d/capcam_est_%d.DAT", path_base, id_mina, id_mina);
                int ncam = get_num_linhas(buffer);

                sprintf(buffer, "%sMina_%d/cargamax_%d.DAT", path_base, id_mina, id_mina);
                int ncarga = get_num_linhas(buffer);

                sprintf(buffer, "%sMina_%d/cmax_%d.DAT", path_base, id_mina, id_mina);
                int nvar = get_num_linhas(buffer);

                // Ore-related pile count from minas.txt; if the mine is not listed, fall back to npilhas.
                int npminerio = descobrir_npminerio(id_mina);
                if (npminerio == -1) npminerio = npilhas;

                // Allocate the instance arrays for this mine (they are all freed at the end of the loop body).
                float *QTDisp = criavetorfloat(npilhas);
                float **teores = criamatrizfloat(npilhas, nvar);
                float *s = criavetorfloat(npilhas);
                float *ls = criavetorfloat(nvar), *li = criavetorfloat(nvar), *rec = criavetorfloat(nvar);

                // Penalty arrays, one entry per quality variable.
                int *penalidadesup = criavetor(nvar), *penalidadeinf = criavetor(nvar), *penalidadeteorinv = criavetor(nvar);
                // Pile-to-loader assignment array (struct alocarga, see hstruct.h).
                struct alocarga *carga = criavetoralocarga(npilhas);
                float *cargamax = criavetorfloat(ncarga), *cargamin = criavetorfloat(ncarga);
                // tciclo[pile][truck]: cycle time per pile and truck, read from tciclo_<id>.DAT below.
                float **tciclo = criamatrizfloat(npilhas, ncam);
                // nviagem[pile][truck]: maximum number of trips; nummax[pile]: number of trucks with at least one trip.
                int **nviagem = criamatriz(npilhas, ncam), *nummax = criavetor(npilhas);
                // compatibilidade[truck][loader]: compatibility flags; nvisitas[pile][truck]: visit counts of the solution.
                int **compatibilidade = criamatriz(ncam, ncarga), **nvisitas = criamatriz(npilhas, ncam);
                // capcam: capacity per truck; dispmec: available fraction of the horizon per truck.
                float *capcam = criavetorfloat(ncam), *dispmec = criavetorfloat(ncam);
                // REM and production limits (min, rec, max), filled from prod_<id>.DAT.
                float remrec, remmax, remmin, prodmax, prodmin, prodrec;

                // Instance files of Mina_<id>/ (each name ends in _<id>.DAT). Record formats expected by the readers:
                // quant: "pile,quantity" -> QTDisp (its line count also gives npilhas)
                // content: "pile,variable,grade" -> teores[pile][variable]
                // cmin: "variable,value" -> li (lower band limit); cmax: "variable,value" -> ls (its line count gives nvar)
                // crec: "variable,value" -> rec (target value per variable)
                // cargamin and cargamax: "loader,production in t/h" (cargamax line count gives ncarga).
                // capcam_est: "truck,value" -> capcam (its line count gives ncam)
                // comp: "truck,loader,flag" -> compatibilidade (1 = compatible)
                // prod: REM min,rec,max on line 1 and production min,rec,max in t/h on line 2.
                // tciclo: "truck,pile,ignored,cycle time" -> tciclo[pile][truck] (see ler_tciclo_matriz)
                sprintf(buffer, "%sMina_%d/quant_%d.DAT", path_base, id_mina, id_mina); learqvetorfloat(buffer, QTDisp);
                sprintf(buffer, "%sMina_%d/content_%d.DAT", path_base, id_mina, id_mina); learqmatrizfloat(buffer, teores);
                sprintf(buffer, "%sMina_%d/cmin_%d.DAT", path_base, id_mina, id_mina); learqvetorfloat(buffer, li);
                sprintf(buffer, "%sMina_%d/cmax_%d.DAT", path_base, id_mina, id_mina); learqvetorfloat(buffer, ls);
                sprintf(buffer, "%sMina_%d/crec_%d.DAT", path_base, id_mina, id_mina); learqvetorfloat(buffer, rec);
                sprintf(buffer, "%sMina_%d/cargamin_%d.DAT", path_base, id_mina, id_mina); learqvetorfloat(buffer, cargamin);
                sprintf(buffer, "%sMina_%d/cargamax_%d.DAT", path_base, id_mina, id_mina); learqvetorfloat(buffer, cargamax);
                sprintf(buffer, "%sMina_%d/capcam_est_%d.DAT", path_base, id_mina, id_mina); learqvetorfloat(buffer, capcam);
                sprintf(buffer, "%sMina_%d/comp_%d.DAT", path_base, id_mina, id_mina); learqmatriz(buffer, compatibilidade);
                sprintf(buffer, "%sMina_%d/prod_%d.DAT", path_base, id_mina, id_mina); ler_arquivo_producao(buffer, &remmin, &remrec, &remmax, &prodmin, &prodrec, &prodmax);
                sprintf(buffer, "%sMina_%d/tciclo_%d.DAT", path_base, id_mina, id_mina); ler_tciclo_matriz(buffer, tciclo, npilhas, ncam);

                // Time horizon and mechanical availability determine the trip limits.
                const float tcontrole = horizonte_planejamento_minutos;
                iniciavetorfloat(dispmec, ncam, disponibilidade_mecanica_padrao);
                carreganviagem(npilhas, ncam, nviagem, nummax, tcontrole, tciclo, dispmec);

                // Seed perfectly diversified per run, mine and method
                // Seed the C random generator from run, mine and method, so every combination gets its own seed.
                srand(numero_da_execucao * 997 + id_mina * 31 + metodo * 101);

                // Reset the solution, visit counts and loader assignments.
                iniciavetorfloat(s, npilhas, 0.0);
                iniciamatrizint(nvisitas, npilhas, ncam, 0);
                iniciavetoralocarga(carga, npilhas, 0, true);

                // Penalty weights per quality variable: the narrower the tolerance band (ls - li), the larger the penalty.
                for (int j_var = 0; j_var < nvar; j_var++) {
                    // Band width; if it is essentially zero, use 5 percent of the target value rec instead.
                    float amplitude = ls[j_var] - li[j_var];
                    if (amplitude < 0.00001) amplitude = rec[j_var] * 0.05;
                    // Base penalty, inversely proportional to the band width (0.001 avoids division by zero).
                    penalidadeteorinv[j_var] = (int)(100.0 / (amplitude + 0.001));
                    // Upper and lower penalties are both 10 times the base penalty.
                    penalidadesup[j_var] = penalidadeteorinv[j_var] * 10;
                    penalidadeinf[j_var] = penalidadeteorinv[j_var] * 10;
                }

                // Start the timer; it covers the construction and the improvement phase only, not the final evaluation.
                auto start = chrono::high_resolution_clock::now();

                // VNS starts from one GRASP solution. The GA builds its own GRASP population.
                if (metodo != 3) {
                    constroisolucaoGRASP(s, carga, capcam, compatibilidade, nvisitas, nviagem,
                                         npilhas, ncam, nvar, teores, rec, npminerio,
                                         QTDisp, cargamax, cargamin, ncarga, prodrec * horasDoHorizonte(tcontrole), remrec, alpha_vns);
                    ajustarTempoCaminhoes(nvisitas, carga, compatibilidade, tciclo, tcontrole, npilhas, ncam, dispmec);
                    carregavetors(s, nvisitas, carga, capcam, compatibilidade, ncam, npilhas);
                }

                // Iterations reported by the improvement phase; stays 0 if no method branch runs.
                int iteracoes_reais = 0;

                // Methods 1 and 2: VNS improvement. The last argument (1 or 2) is passed through unchanged.
                if (metodo == 1) {
                    iteracoes_reais = VNS(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv,
                                         npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio,
                                         remrec, remmax, remmin, nvisitas, carga, capcam,
                                         compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax,
                                         wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn,
                                         wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual,
                                         wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist,
                                         dispmec, nviagem, 1);
                } else if (metodo == 2) {
                    iteracoes_reais = VNS(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv,
                                         npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio,
                                         remrec, remmax, remmin, nvisitas, carga, capcam,
                                         compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax,
                                         wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn,
                                         wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual,
                                         wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist,
                                         dispmec, nviagem, 2);
                // Method 3: AG-RLP with the calibrated population size and alpha defaults.
                } else if (metodo == 3) {
                    iteracoes_reais = AlgoritmoGenetico(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv,
                                                        npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, QTDisp, ncarga,
                                                        remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole,
                                                        ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn,
                                                        wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn,
                                                        wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados,
                                                        wfestatico, wfnumcam, wfdist, dispmec, nviagem, 2);
                }

                // Stop the timer.
                auto end = chrono::high_resolution_clock::now();
                chrono::duration<double> elapsed = end - start;

                // Phase 3: evaluate the final solution s with the objective function (Fo).
                long long fo_final = favaliacao(
                    s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv,
                    npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio,
                    remrec, remmax, remmin, nvisitas, carga, capcam,
                    compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax,
                    wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn,
                    wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual,
                    wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist,
                    dispmec, nviagem);

                // Progress line on the console.
                printf("FO final = %lld | Tempo = %.4f s | Iteracoes Reais: %d\n", fo_final, elapsed.count(), iteracoes_reais);

                // Append one row (mine id; Fo; time in seconds; iterations) to the method's CSV.
                f_resumo = fopen(arquivo_name, "a");
                if (f_resumo) {
                    fprintf(f_resumo, "%d;%lld;%.4f;%d\n", id_mina, fo_final, elapsed.count(), iteracoes_reais);
                    fclose(f_resumo);
                }

                // Free all arrays allocated for this mine before moving to the next mine.
                liberavetorfloat(QTDisp); liberamatrizfloat(teores, npilhas); liberavetorfloat(s);
                liberavetorfloat(ls); liberavetorfloat(li); liberavetorfloat(rec);
                liberavetor(penalidadesup); liberavetor(penalidadeinf); liberavetor(penalidadeteorinv);
                liberavetoralocarga(carga); liberavetorfloat(cargamax); liberavetorfloat(cargamin);
                liberamatrizfloat(tciclo, npilhas); liberamatriz(nviagem, npilhas); liberavetor(nummax);
                liberamatriz(compatibilidade, ncam); liberamatriz(nvisitas, npilhas);
                liberavetorfloat(capcam); liberavetorfloat(dispmec);
            }
        }
    }

    // Printed after all runs and methods have finished.
    printf("\n--- EXECUÇÃO CONCLUÍDA ---\n");
    return 0;
}
