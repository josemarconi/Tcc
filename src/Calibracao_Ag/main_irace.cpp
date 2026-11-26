// Calibration driver for the genetic algorithm, run by irace (see run_irace.R and target-runner).
// One run loads a mine instance from ../dados/Minas/Mina_<id>, builds an initial solution with GRASP,
// runs AlgoritmoGenetico (method 11 or 12) and prints the objective value on stdout.
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
#include <iostream>
#include <string>

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

// Returns m, the number of ore piles of mine id_mina, read from the "m/e" text on its "Mina <id>:" line in ../dados/minas.txt.
// Returns -1 if the file or the line is not found.
int descobrir_npminerio(int id_mina)
{
    FILE *f = fopen("../dados/minas.txt", "r");
    if (!f) return -1;

    char linha[512];
    char busca[512];
    sprintf(busca, "Mina %d:", id_mina);

    // Scan the file line by line until the line of this mine is found.
    while (fgets(linha, sizeof(linha), f))
    {
        if (strstr(linha, busca))
        {
            int m, e;
            char *ptr = strchr(linha, '/');
            if (ptr)
            {
                char *start = ptr - 1;
                // Move start back to the first digit of the number that precedes the '/'.
                while (start > linha && isdigit(*(start - 1))) start--;
                // Read m (ore piles) and e from the "m/e" text.
                sscanf(start, "%d/%d", &m, &e);
                fclose(f);
                return m;
            }
        }
    }
    fclose(f);
    return -1;
}

// Returns the number of non-empty lines of a file (number of piles, trucks, loaders or variables). Returns 0 if the file cannot be opened.
int get_num_linhas(char *nomearq)
{
    FILE *arquivo = fopen(nomearq, "r");
    if (!arquivo) return 0;

    int linhas = 0;
    char ch;
    int conteudo = 0;

    // Scan the file character by character; a line counts only if it has something before its newline.
    while ((ch = fgetc(arquivo)) != EOF)
    {
        if (ch != '\n') conteudo = 1;
        if (ch == '\n')
        {
            if (conteudo) linhas++;
            conteudo = 0;
        }
    }
    if (conteudo) linhas++;
    fclose(arquivo);
    return linhas;
}

// Reads the production limits file: line 1 holds REM min, rec, max; line 2 holds production min, rec, max.
// Accepts comma or space separators. Exits the program if the file cannot be opened.
void ler_arquivo_producao(char *nomearq, float *remmin, float *remrec, float *remmax, float *prodmin, float *prodrec, float *prodmax)
{
    FILE *f = fopen(nomearq, "r");
    if (!f) exit(1);

    if (fscanf(f, "%f,%f,%f", remmin, remrec, remmax) != 3)
    {
        rewind(f);
        fscanf(f, "%f %f %f", remmin, remrec, remmax);
    }
    if (fscanf(f, "%f,%f,%f", prodmin, prodrec, prodmax) != 3)
        fscanf(f, "%f %f %f", prodmin, prodrec, prodmax);

    fclose(f);
}

// Reads the cycle-time file (lines "cam,pilha,aux,value") into tciclo[pilha][cam].
// Values of 9000 or more are ignored and stay 0. Exits the program if the file cannot be opened.
void ler_tciclo_matriz(char *nomearq, float **tciclo, int npilhas, int ncam)
{
    FILE *f = fopen(nomearq, "r");
    if (!f) exit(1);

    iniciamatrizfloat(tciclo, npilhas, ncam, 0.0);
    int cam, pilha, aux;
    float val;

    // Each line gives the truck cam, the pile pilha, an unused field aux and the cycle time val.
    while (fscanf(f, "%d,%d,%d,%f", &cam, &pilha, &aux, &val) == 4)
    {
        if (val < 9000) tciclo[pilha][cam] = val;
    }
    fclose(f);
}

// Entry point: parses the iRace arguments, loads the instance, builds the GRASP solution,
// runs the genetic algorithm and prints the objective value.
int main(int argc, char *argv[])
{
    // Default values for an individual run or test
    int id_mina = 1;
    unsigned int seed = 12345;
    int metodo = 12;
    int tam_pop = populacao_padrao_ag;
    float alpha = alpha_padrao_ag;

    // Hyperparameters and weights configurable by iRace
    // Weights (wf*) of each objective term with their defaults; the iRace arguments below override them.
    int wfproducaoinv = 1000000;
    int wfproducaoqualp = 1000;
    int wfproducaoqualn = 1000;
    int wfteor = 500000;
    int wfrem = 100000;
    int wfremqualp = 1;
    int wfremqualn = 1;
    int wfprodutcaminv = 1000;
    int wfprodutcamqualp = 1;
    int wfprodutcamqualn = 1;
    int wfprodcargainv = 1000;
    int wfprodcargaqual = 1;
    int wfproducaocamtotal = 1;
    int wfproducaocarga = 1;
    int wfnumcamutilizados = 1;
    int wfestatico = 10000;
    int wfnumcam = 10000;
    int wfdist = 1;

    // Read the arguments supplied by iRace / target-runner
    // Each option is a name followed by its value; unknown arguments are ignored.
    for (int i = 1; i < argc; i++)
    {
        if ((strcmp(argv[i], "--instance") == 0 || strcmp(argv[i], "-i") == 0) && i + 1 < argc) { id_mina = atoi(argv[++i]); }
        else if ((strcmp(argv[i], "--seed") == 0 || strcmp(argv[i], "-s") == 0) && i + 1 < argc) { seed = (unsigned int)stoul(argv[++i]); }
        else if (strcmp(argv[i], "--metodo") == 0 && i + 1 < argc) { metodo = atoi(argv[++i]); }
        else if (strcmp(argv[i], "--pop") == 0)
        {
            char *fim;
            errno = 0;
            const char *valor = i + 1 < argc ? argv[++i] : "";
            const long populacao = strtol(valor, &fim, 10);
            if (errno != 0 || fim == valor || *fim != '\0' || populacao < 2 || populacao > INT_MAX / 2)
            {
                fprintf(stderr, "--pop requer um inteiro entre 2 e %d\n", INT_MAX / 2);
                return 2;
            }
            tam_pop = static_cast<int>(populacao);
        }
        else if (strcmp(argv[i], "--alpha") == 0)
        {
            char *fim;
            const char *valor = i + 1 < argc ? argv[++i] : "";
            alpha = strtof(valor, &fim);
            if (fim == valor || *fim != '\0' || !(alpha >= 0.0f && alpha <= 1.0f))
            {
                fprintf(stderr, "--alpha requer um numero entre 0 e 1\n");
                return 2;
            }
        }
        else if (strcmp(argv[i], "--wfteor") == 0 && i + 1 < argc) { wfteor = atoi(argv[++i]); }
        else if (strcmp(argv[i], "--wfrem") == 0 && i + 1 < argc) { wfrem = atoi(argv[++i]); }
        else if (strcmp(argv[i], "--wfproducaoinv") == 0 && i + 1 < argc) { wfproducaoinv = atoi(argv[++i]); }
        else if (strcmp(argv[i], "--wfproducaoqualp") == 0 && i + 1 < argc) { wfproducaoqualp = atoi(argv[++i]); }
        else if (strcmp(argv[i], "--wfproducaoqualn") == 0 && i + 1 < argc) { wfproducaoqualn = atoi(argv[++i]); }
        else if (strcmp(argv[i], "--wfremqualp") == 0 && i + 1 < argc) { wfremqualp = atoi(argv[++i]); }
        else if (strcmp(argv[i], "--wfremqualn") == 0 && i + 1 < argc) { wfremqualn = atoi(argv[++i]); }
        else if (strcmp(argv[i], "--wfprodutcaminv") == 0 && i + 1 < argc) { wfprodutcaminv = atoi(argv[++i]); }
        else if (strcmp(argv[i], "--wfprodutcamqualp") == 0 && i + 1 < argc) { wfprodutcamqualp = atoi(argv[++i]); }
        else if (strcmp(argv[i], "--wfprodutcamqualn") == 0 && i + 1 < argc) { wfprodutcamqualn = atoi(argv[++i]); }
        else if (strcmp(argv[i], "--wfprodcargainv") == 0 && i + 1 < argc) { wfprodcargainv = atoi(argv[++i]); }
        else if (strcmp(argv[i], "--wfprodcargaqual") == 0 && i + 1 < argc) { wfprodcargaqual = atoi(argv[++i]); }
        else if (strcmp(argv[i], "--wfproducaocamtotal") == 0 && i + 1 < argc) { wfproducaocamtotal = atoi(argv[++i]); }
        else if (strcmp(argv[i], "--wfproducaocarga") == 0 && i + 1 < argc) { wfproducaocarga = atoi(argv[++i]); }
        else if (strcmp(argv[i], "--wfnumcamutilizados") == 0 && i + 1 < argc) { wfnumcamutilizados = atoi(argv[++i]); }
        else if (strcmp(argv[i], "--wfestatico") == 0 && i + 1 < argc) { wfestatico = atoi(argv[++i]); }
        else if (strcmp(argv[i], "--wfnumcam") == 0 && i + 1 < argc) { wfnumcam = atoi(argv[++i]); }
        else if (strcmp(argv[i], "--wfdist") == 0 && i + 1 < argc) { wfdist = atoi(argv[++i]); }
    }

    // Fix the received seed for reproducibility
    // Seed the C random generator so that runs are reproducible.
    srand(seed);

    // Base folder of the mine data, relative to the working directory.
    char path_base[] = "../dados/Minas/";
    char buffer[512];

    // Check whether the instance exists
    // Check that the instance exists (its quant file). If not, print 1e12 as a large penalty and exit with status 1.
    sprintf(buffer, "%sMina_%d/quant_%d.DAT", path_base, id_mina, id_mina);
    FILE *teste = fopen(buffer, "r");
    if (!teste)
    {
        // If the mine fails to load, send a large penalty value to iRace
        printf("1e12\n");
        return 1;
    }
    fclose(teste);

    // Problem sizes from the line counts of the data files: npilhas piles, ncam trucks, ncarga loaders, nvar quality variables.
    int npilhas = get_num_linhas(buffer);

    sprintf(buffer, "%sMina_%d/capcam_est_%d.DAT", path_base, id_mina, id_mina);
    int ncam = get_num_linhas(buffer);

    sprintf(buffer, "%sMina_%d/cargamax_%d.DAT", path_base, id_mina, id_mina);
    int ncarga = get_num_linhas(buffer);

    sprintf(buffer, "%sMina_%d/cmax_%d.DAT", path_base, id_mina, id_mina);
    int nvar = get_num_linhas(buffer);

    // Ore pile count from minas.txt; if it is not found, every pile is treated as ore.
    int npminerio = descobrir_npminerio(id_mina);
    if (npminerio == -1) npminerio = npilhas;

    // Allocation of structures
    // Allocate the solution, mine-data and bookkeeping arrays.
    float *QTDisp = criavetorfloat(npilhas);
    float **teores = criamatrizfloat(npilhas, nvar);
    float *s = criavetorfloat(npilhas);
    float *ls = criavetorfloat(nvar);
    float *li = criavetorfloat(nvar);
    float *rec = criavetorfloat(nvar);

    int *penalidadesup = criavetor(nvar);
    int *penalidadeinf = criavetor(nvar);
    int *penalidadeteorinv = criavetor(nvar);

    struct alocarga *carga = criavetoralocarga(npilhas);
    float *cargamax = criavetorfloat(ncarga);
    float *cargamin = criavetorfloat(ncarga);

    float **tciclo = criamatrizfloat(npilhas, ncam);
    int **nviagem = criamatriz(npilhas, ncam);
    int *nummax = criavetor(npilhas);
    int **compatibilidade = criamatriz(ncam, ncarga);
    int **nvisitas = criamatriz(npilhas, ncam);

    float *capcam = criavetorfloat(ncam);
    float *dispmec = criavetorfloat(ncam);
    float remrec, remmax, remmin, prodmax, prodmin, prodrec;

    // Reading of the matrices and vectors from the mine data files
    // Each data file of this instance is read into the matching array allocated above.
    sprintf(buffer, "%sMina_%d/quant_%d.DAT", path_base, id_mina, id_mina);
    learqvetorfloat(buffer, QTDisp);

    sprintf(buffer, "%sMina_%d/content_%d.DAT", path_base, id_mina, id_mina);
    learqmatrizfloat(buffer, teores);

    sprintf(buffer, "%sMina_%d/cmin_%d.DAT", path_base, id_mina, id_mina);
    learqvetorfloat(buffer, li);

    sprintf(buffer, "%sMina_%d/cmax_%d.DAT", path_base, id_mina, id_mina);
    learqvetorfloat(buffer, ls);

    sprintf(buffer, "%sMina_%d/crec_%d.DAT", path_base, id_mina, id_mina);
    learqvetorfloat(buffer, rec);

    sprintf(buffer, "%sMina_%d/cargamin_%d.DAT", path_base, id_mina, id_mina);
    learqvetorfloat(buffer, cargamin);

    sprintf(buffer, "%sMina_%d/cargamax_%d.DAT", path_base, id_mina, id_mina);
    learqvetorfloat(buffer, cargamax);

    sprintf(buffer, "%sMina_%d/capcam_est_%d.DAT", path_base, id_mina, id_mina);
    learqvetorfloat(buffer, capcam);

    sprintf(buffer, "%sMina_%d/comp_%d.DAT", path_base, id_mina, id_mina);
    learqmatriz(buffer, compatibilidade);

    // Production limits (REM min, rec, max and production min, rec, max) from prod_<id>.DAT.
    sprintf(buffer, "%sMina_%d/prod_%d.DAT", path_base, id_mina, id_mina);
    ler_arquivo_producao(buffer, &remmin, &remrec, &remmax, &prodmin, &prodrec, &prodmax);

    // Cycle time of each truck for each pile.
    sprintf(buffer, "%sMina_%d/tciclo_%d.DAT", path_base, id_mina, id_mina);
    ler_tciclo_matriz(buffer, tciclo, npilhas, ncam);

    // Time horizon and mechanical availability determine the trip limits.
    const float tcontrole = horizonte_planejamento_minutos;
    iniciavetorfloat(dispmec, ncam, disponibilidade_mecanica_padrao);
    carreganviagem(npilhas, ncam, nviagem, nummax, tcontrole, tciclo, dispmec);

    // Initial state: zero production and visits and default loader assignments.
    iniciavetorfloat(s, npilhas, 0.0);
    iniciamatrizint(nvisitas, npilhas, ncam, 0);
    iniciavetoralocarga(carga, npilhas, 0, true);

    // Per-variable penalty weights from the grade range (ls - li). A tiny range uses 5% of rec.
    // The inverse weight is 100/(range + 0.001); the upper and lower weights are 10 times that.
    for (int j_v = 0; j_v < nvar; j_v++)
    {
        float amplitude = ls[j_v] - li[j_v];
        if (amplitude < 0.00001) amplitude = rec[j_v] * 0.05;
        penalidadeteorinv[j_v] = (int)(100.0 / (amplitude + 0.001));
        penalidadesup[j_v] = penalidadeteorinv[j_v] * 10;
        penalidadeinf[j_v] = penalidadeteorinv[j_v] * 10;
    }

    // Method 11 uses fixed VND and method 12 uses VND-RLP, as in the main experiment.
    if (metodo == 11 || metodo == 12)
    {
        AlgoritmoGenetico(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv,
                         npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, QTDisp, ncarga,
                         remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole,
                         ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn,
                         wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn,
                         wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados,
                         wfestatico, wfnumcam, wfdist, dispmec, nviagem, metodo == 11 ? 1 : 2, tam_pop, alpha);
    }
    else
    {
        constroisolucaoGRASP(s, carga, capcam, compatibilidade, nvisitas, nviagem,
                             npilhas, ncam, nvar, teores, rec, npminerio, QTDisp, cargamax, cargamin,
                             ncarga, prodrec * horasDoHorizonte(tcontrole), remrec, alpha);
        ajustarTempoCaminhoes(nvisitas, carga, compatibilidade, tciclo, tcontrole, npilhas, ncam, dispmec);
    }

    // Final evaluation of the obtained solution
    // Final evaluation of the search result with the same objective function (favaliacao).
    long long fo_final = favaliacao(
        s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv,
        npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio,
        remrec, remmax, remmin, nvisitas, carga, capcam,
        compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax,
        wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn,
        wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual,
        wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist,
        dispmec, nviagem);

    // MANDATORY OUTPUT: iRace captures exactly this line as the cost of the candidate
    // Print the objective value on stdout; irace reads this single line as the cost of the candidate.
    printf("%lld\n", fo_final);

    // Dynamic release of memory
    // Free all arrays allocated above.
    liberavetorfloat(QTDisp);
    liberamatrizfloat(teores, npilhas);
    liberavetorfloat(s);
    liberavetorfloat(ls);
    liberavetorfloat(li);
    liberavetorfloat(rec);
    liberavetor(penalidadesup);
    liberavetor(penalidadeinf);
    liberavetor(penalidadeteorinv);
    liberavetoralocarga(carga);
    liberavetorfloat(cargamax);
    liberavetorfloat(cargamin);
    liberamatrizfloat(tciclo, npilhas);
    liberamatriz(nviagem, npilhas);
    liberavetor(nummax);
    liberamatriz(compatibilidade, ncam);
    liberamatriz(nvisitas, npilhas);
    liberavetorfloat(capcam);
    liberavetorfloat(dispmec);

    return 0;
}
