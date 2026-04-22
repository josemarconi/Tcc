#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <time.h>
#include <float.h>
#include <errno.h>
#include <math.h>
#include <chrono> // Adicionado para medição de tempo

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

using namespace std;

int descobrir_npminerio(int id_mina)
{
    FILE *f = fopen("../codigo_gerar_arquivos/minas.txt", "r");
    if (!f)
        return -1;

    char linha[512];
    char busca[512];

    sprintf(busca, "Mina %d:", id_mina);

    while (fgets(linha, sizeof(linha), f))
    {
        if (strstr(linha, busca))
        {
            int m, e;
            char *ptr = strchr(linha, '/');
            if (ptr)
            {
                char *start = ptr - 1;
                while (start > linha && isdigit(*(start - 1)))
                    start--;
                sscanf(start, "%d/%d", &m, &e);
                fclose(f);
                return m;
            }
        }
    }

    fclose(f);
    return -1;
}

int get_num_linhas(char *nomearq)
{
    FILE *arquivo = fopen(nomearq, "r");
    if (!arquivo)
        return 0;

    int linhas = 0;
    char ch;
    int conteudo = 0;

    while ((ch = fgetc(arquivo)) != EOF)
    {
        if (ch != '\n')
            conteudo = 1;

        if (ch == '\n')
        {
            if (conteudo)
                linhas++;
            conteudo = 0;
        }
    }

    if (conteudo)
        linhas++;

    fclose(arquivo);
    return linhas;
}

void ler_arquivo_producao(char *nomearq,
                          float *remmin, float *remrec, float *remmax,
                          float *prodmin, float *prodrec, float *prodmax)
{
    FILE *f = fopen(nomearq, "r");

    if (!f)
    {
        printf("Erro ao abrir %s\n", nomearq);
        exit(1);
    }

    if (fscanf(f, "%f,%f,%f", remmin, remrec, remmax) != 3)
    {
        rewind(f);
        fscanf(f, "%f %f %f", remmin, remrec, remmax);
    }

    if (fscanf(f, "%f,%f,%f", prodmin, prodrec, prodmax) != 3)
        fscanf(f, "%f %f %f", prodmin, prodrec, prodmax);

    fclose(f);
}

void ler_tciclo_matriz(char *nomearq, float **tciclo, int npilhas, int ncam)
{
    FILE *f = fopen(nomearq, "r");

    if (!f)
    {
        printf("Erro ao abrir %s\n", nomearq);
        exit(1);
    }

    iniciamatrizfloat(tciclo, npilhas, ncam, 0.0);

    int cam, pilha, aux;
    float val;

    while (fscanf(f, "%d,%d,%d,%f", &cam, &pilha, &aux, &val) == 4)
    {
        if (val < 9000)
            tciclo[pilha][cam] = val;
    }

    fclose(f);
}

int main()
{
    // SELETOR DE MÉTODO:
    // 1 = ILS Normal | 2 = ILS Aleatorio | 3 = Smart ILS Normal | 4 = Smart ILS Aleatorio
    int metodo = 4;
    char path_base[] = "../codigo_gerar_arquivos/Minas/";
    char buffer[512];

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
    float alpha = 0.2;
    // --- NOVA ADIÇÃO: Inicialização do Relatório Geral ---
    char arq_resumo_geral[] = "Relatorios/Resumo_Geral_Minas_GRASP_VND(Normal)1.csv";
    FILE *f_resumo = fopen(arq_resumo_geral, "w");
    if (!f_resumo) {
        printf("Erro: Pasta 'Relatorios' nao encontrada.\n");
        return 1;
    }
    fprintf(f_resumo, "Mina;Fo;Tempo(s);Iter\n");
    fclose(f_resumo);

    for (int id_mina = 1; id_mina <= 290; id_mina++)
    {

        printf("\n=================================================\n");
        printf("Processando Mina %d\n", id_mina);
        printf("=================================================\n");

        sprintf(buffer, "%sMina_%d/quant_%d.DAT", path_base, id_mina, id_mina);

        FILE *teste = fopen(buffer, "r");

        if (!teste)
        {
            printf("Mina %d nao encontrada\n", id_mina);
            continue; // Mudado para continue para não parar o loop das 290
        }

        fclose(teste);

        printf("[INFO] Detectando dimensoes...\n");

        int npilhas = get_num_linhas(buffer);

        sprintf(buffer, "%sMina_%d/capcam_est_%d.DAT", path_base, id_mina, id_mina);
        int ncam = get_num_linhas(buffer);

        sprintf(buffer, "%sMina_%d/cargamax_%d.DAT", path_base, id_mina, id_mina);
        int ncarga = get_num_linhas(buffer);

        sprintf(buffer, "%sMina_%d/cmax_%d.DAT", path_base, id_mina, id_mina);
        int nvar = get_num_linhas(buffer);

        printf("[INFO] npilhas = %d\n", npilhas);
        printf("[INFO] ncam    = %d\n", ncam);
        printf("[INFO] ncarga  = %d\n", ncarga);
        printf("[INFO] nvar    = %d\n", nvar);

        int npminerio = descobrir_npminerio(id_mina);
        if (npminerio == -1)
            npminerio = npilhas;

        printf("[INFO] npminerio = %d\n", npminerio);

        printf("[INFO] Alocando memoria...\n");

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

        float remrec, remmax, remmin;
        float prodmax, prodmin, prodrec;

        printf("[INFO] Lendo arquivos...\n");

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

        sprintf(buffer, "%sMina_%d/prod_%d.DAT", path_base, id_mina, id_mina);
        ler_arquivo_producao(buffer, &remmin, &remrec, &remmax, &prodmin, &prodrec, &prodmax);

        sprintf(buffer, "%sMina_%d/tciclo_%d.DAT", path_base, id_mina, id_mina);
        ler_tciclo_matriz(buffer, tciclo, npilhas, ncam);

        printf("[INFO] Calculando limite de viagens...\n");

        float tcontrole = 360.0;

        for (int i = 0; i < npilhas; i++)
        {
            int cont = 0;

            for (int j = 0; j < ncam; j++)
            {
                if (tciclo[i][j] > 0.01)
                {
                    nviagem[i][j] = (int)floor(tcontrole / tciclo[i][j]);

                    if (nviagem[i][j] > 0)
                        cont++;
                }
                else
                    nviagem[i][j] = 0;
            }

            nummax[i] = cont;
        }

        srand(1234 + id_mina);

        iniciavetorfloat(s, npilhas, 0.0);
        iniciamatrizint(nvisitas, npilhas, ncam, 0);
        iniciavetoralocarga(carga, npilhas, 0, true);
        iniciavetorfloat(dispmec, ncam, 0.0);

        for (int j = 0; j < nvar; j++)
        {
            float amplitude = ls[j] - li[j];
            if (amplitude < 0.00001)
                amplitude = rec[j] * 0.05;

            penalidadeteorinv[j] = (int)(100.0 / (amplitude + 0.001));
            penalidadesup[j] = penalidadeteorinv[j] * 10;
            penalidadeinf[j] = penalidadeteorinv[j] * 10;
        }

        //printf("[INFO] Construindo solucao inicial (Gulosa)...\n");

        // --- NOVA ADIÇÃO: Início da medição de tempo ---
        auto start = chrono::high_resolution_clock::now();

        /*constroisolucaogulosa(
            s, carga, capcam, compatibilidade, nvisitas, nviagem,
            npilhas, ncam, nvar, teores, rec, npminerio,
            QTDisp, cargamax, cargamin, ncarga, prodrec, remrec);*/

        printf("[INFO] Construindo solucao inicial (GRASP)...\n");
        constroisolucaoGRASP(s, carga, capcam, compatibilidade, nvisitas, nviagem, 
                             npilhas, ncam, nvar, teores, rec, npminerio, 
                             QTDisp, cargamax, cargamin, ncarga, prodrec, remrec, alpha);    

        /*
        if (metodo == 1) {
            ILS_Normal(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);
        } 
        else if (metodo == 2) {
            ILS_Aleatorio(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);
        }
        else if (metodo == 3) {
            Smart_ILS_Normal(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);
        }
        else if (metodo == 4) {
            Smart_ILS_Aleatorio(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);
        }*/

        
        printf("[INFO] Executando VNS...\n");   

        VNS(s, li, ls, rec,
            penalidadesup, penalidadeinf, penalidadeteorinv,
            npilhas, nvar, teores,
            prodmax, prodmin, prodrec,
            npminerio,
            remrec, remmax, remmin,
            nvisitas, carga, capcam,
            compatibilidade, tciclo, tcontrole,
            ncam, cargamax, cargamin, nummax,
            wfproducaoinv, wfproducaoqualp, wfproducaoqualn,
            wfteor, wfrem, wfremqualp, wfremqualn,
            wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn,
            wfprodcargainv, wfprodcargaqual,
            wfproducaocamtotal, wfproducaocarga,
            wfnumcamutilizados,
            wfestatico, wfnumcam, wfdist,
            dispmec, nviagem);

        // --- NOVA ADIÇÃO: Fim da medição de tempo ---
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;

        long long fo_final = favaliacao(
            s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv,
            npilhas, nvar, teores,
            prodmax, prodmin, prodrec,
            npminerio,
            remrec, remmax, remmin,
            nvisitas, carga, capcam,
            compatibilidade, tciclo, tcontrole,
            ncam, cargamax, cargamin, nummax,
            wfproducaoinv, wfproducaoqualp, wfproducaoqualn,
            wfteor, wfrem, wfremqualp, wfremqualn,
            wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn,
            wfprodcargainv, wfprodcargaqual,
            wfproducaocamtotal, wfproducaocarga,
            wfnumcamutilizados,
            wfestatico, wfnumcam, wfdist,
            dispmec, nviagem);

        printf("[INFO] FO final = %lld | Tempo = %.4f s\n", fo_final, elapsed.count());

        char rel_detalhado[512];
        sprintf(rel_detalhado, "Solucoes/Solucao_Mina_%d.txt", id_mina);

        imprimerelatorio(rel_detalhado, 0, 1234, (float)elapsed.count(),
                         s, li, ls, rec,
                         penalidadesup, penalidadeinf, penalidadeteorinv,
                         npilhas, nvar, teores,
                         prodmax, prodmin, prodrec,
                         npminerio,
                         remrec, remmax, remmin,
                         nvisitas, carga, capcam,
                         compatibilidade, tciclo, tcontrole,
                         ncam, cargamax, cargamin, nummax,
                         wfproducaoinv, wfproducaoqualp, wfproducaoqualn,
                         wfteor, wfrem, wfremqualp, wfremqualn,
                         wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn,
                         wfprodcargainv, wfprodcargaqual,
                         wfproducaocamtotal, wfproducaocarga,
                         wfnumcamutilizados,
                         wfestatico, wfnumcam, wfdist,
                         dispmec, nviagem);

        // --- NOVA ADIÇÃO: Escrita no Relatório Geral Consolidade ---
        f_resumo = fopen(arq_resumo_geral, "a");
        if (f_resumo) {
            fprintf(f_resumo, "%d;%lld;%.4f;%d\n", id_mina, fo_final, elapsed.count(), 1000); // 1000 iters exemplo
            fclose(f_resumo);
        }

        // --- LIMPEZA DE MEMÓRIA (Essencial no loop de 290 minas) ---
        liberavetorfloat(QTDisp); liberamatrizfloat(teores, npilhas); liberavetorfloat(s);
        liberavetorfloat(ls); liberavetorfloat(li); liberavetorfloat(rec);
        liberavetor(penalidadesup); liberavetor(penalidadeinf); liberavetor(penalidadeteorinv);
        liberavetoralocarga(carga); liberavetorfloat(cargamax); liberavetorfloat(cargamin);
        liberamatrizfloat(tciclo, npilhas); liberamatriz(nviagem, npilhas);
        liberavetor(nummax); liberamatriz(compatibilidade, ncam);
        liberamatriz(nvisitas, npilhas); liberavetorfloat(capcam); liberavetorfloat(dispmec);

        printf("[OK] Mina %d finalizada\n", id_mina);
    }

    printf("\n=================================================\n");
    printf("PROCESSAMENTO FINALIZADO\n");
    printf("=================================================\n");

    return 0;
}



// ------------------------------------------------------------------------------------------------------------------------ //
/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <time.h>
#include <float.h>
#include <errno.h>
#include <math.h>
#include <chrono>

#include "Hutil.h"
#include "Hdefine.h"
#include "Hutilcarga.h"
#include "Hfavaliacao.h"
#include "Hmovimento.h"
#include "Hsa.h"
#include "Hvnd.h"
#include "Hvns.h"
#include "hstruct.h"

using namespace std;

int descobrir_npminerio(int id_mina)
{
    FILE *f = fopen("../codigo_gerar_arquivos/minas.txt", "r");
    if (!f)
        return -1;

    char linha[1024];
    char busca[50];

    sprintf(busca, "Mina %d:", id_mina);

    while (fgets(linha, sizeof(linha), f))
    {
        if (strstr(linha, busca))
        {
            int m, e;
            char *ptr = strchr(linha, '/');
            if (ptr)
            {
                char *start = ptr - 1;
                while (start > linha && isdigit(*(start - 1)))
                    start--;
                sscanf(start, "%d/%d", &m, &e);
                fclose(f);
                return m;
            }
        }
    }

    fclose(f);
    return -1;
}

int get_num_linhas(char *nomearq)
{
    FILE *arquivo = fopen(nomearq, "r");
    if (!arquivo)
        return 0;

    int linhas = 0;
    char ch;
    int conteudo = 0;

    while ((ch = fgetc(arquivo)) != EOF)
    {
        if (ch != '\n')
            conteudo = 1;

        if (ch == '\n')
        {
            if (conteudo)
                linhas++;
            conteudo = 0;
        }
    }

    if (conteudo)
        linhas++;

    fclose(arquivo);
    return linhas;
}

void ler_arquivo_producao(char *nomearq,
                          float *remmin, float *remrec, float *remmax,
                          float *prodmin, float *prodrec, float *prodmax)
{
    FILE *f = fopen(nomearq, "r");

    if (!f)
    {
        printf("Erro ao abrir %s\n", nomearq);
        exit(1);
    }

    if (fscanf(f, "%f,%f,%f", remmin, remrec, remmax) != 3)
    {
        rewind(f);
        fscanf(f, "%f %f %f", remmin, remrec, remmax);
    }

    if (fscanf(f, "%f,%f,%f", prodmin, prodrec, prodmax) != 3)
        fscanf(f, "%f %f %f", prodmin, prodrec, prodmax);

    fclose(f);
}

void ler_tciclo_matriz(char *nomearq, float **tciclo, int npilhas, int ncam)
{
    FILE *f = fopen(nomearq, "r");

    if (!f)
    {
        printf("Erro ao abrir %s\n", nomearq);
        exit(1);
    }

    iniciamatrizfloat(tciclo, npilhas, ncam, 0.0);

    int cam, pilha, aux;
    float val;

    while (fscanf(f, "%d,%d,%d,%f", &cam, &pilha, &aux, &val) == 4)
    {
        if (val < 9000)
            tciclo[pilha][cam] = val;
    }

    fclose(f);
}

int main()
{

    char path_base[] = "../codigo_gerar_arquivos/Minas/";
    char buffer[512];

    int wfproducaoinv = 100000;
    int wfproducaoqualp = 1000;
    int wfproducaoqualn = 1000;
    int wfteor = 100;
    int wfrem = 10000;
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
    int wfestatico = 900000;
    int wfnumcam = 900000;
    int wfdist = 1;

    for (int id_mina = 1; id_mina <= 290; id_mina++)
    {

        printf("\n=================================================\n");
        printf("Processando Mina %d\n", id_mina);
        printf("=================================================\n");

        sprintf(buffer, "%sMina_%d/quant_%d.DAT", path_base, id_mina, id_mina);

        FILE *teste = fopen(buffer, "r");

        if (!teste)
        {
            printf("Mina %d nao encontrada\n", id_mina);
            exit(1);
        }

        fclose(teste);

        printf("[INFO] Detectando dimensoes...\n");

        int npilhas = get_num_linhas(buffer);

        sprintf(buffer, "%sMina_%d/capcam_est_%d.DAT", path_base, id_mina, id_mina);
        int ncam = get_num_linhas(buffer);

        sprintf(buffer, "%sMina_%d/cargamax_%d.DAT", path_base, id_mina, id_mina);
        int ncarga = get_num_linhas(buffer);

        sprintf(buffer, "%sMina_%d/cmax_%d.DAT", path_base, id_mina, id_mina);
        int nvar = get_num_linhas(buffer);

        printf("[INFO] npilhas = %d\n", npilhas);
        printf("[INFO] ncam    = %d\n", ncam);
        printf("[INFO] ncarga  = %d\n", ncarga);
        printf("[INFO] nvar    = %d\n", nvar);

        int npminerio = descobrir_npminerio(id_mina);
        if (npminerio == -1)
            npminerio = npilhas;

        printf("[INFO] npminerio = %d\n", npminerio);

        printf("[INFO] Alocando memoria...\n");

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

        float remrec, remmax, remmin;
        float prodmax, prodmin, prodrec;

        printf("[INFO] Lendo arquivos...\n");

        sprintf(buffer, "%sMina_%d/quant_%d.DAT", path_base, id_mina, id_mina);
        learqvetorfloat(buffer, QTDisp);
        printf("\n[DEBUG] QTDisp:\n");
        for (int i = 0; i < npilhas; i++)
            printf("pilha %d = %.2f\n", i, QTDisp[i]);

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

        sprintf(buffer, "%sMina_%d/prod_%d.DAT", path_base, id_mina, id_mina);
        ler_arquivo_producao(buffer, &remmin, &remrec, &remmax, &prodmin, &prodrec, &prodmax);

        sprintf(buffer, "%sMina_%d/tciclo_%d.DAT", path_base, id_mina, id_mina);
        ler_tciclo_matriz(buffer, tciclo, npilhas, ncam);

        printf("[INFO] Calculando limite de viagens...\n");

        float tcontrole = 360.0;

        for (int i = 0; i < npilhas; i++)
        {
            int cont = 0;

            for (int j = 0; j < ncam; j++)
            {
                if (tciclo[i][j] > 0.01)
                {
                    nviagem[i][j] = (int)floor(tcontrole / tciclo[i][j]);

                    if (nviagem[i][j] > 0)
                        cont++;
                }
                else
                    nviagem[i][j] = 0;
            }

            nummax[i] = cont;
        }

        srand(1234 + id_mina);

        iniciavetorfloat(s, npilhas, 0.0);
        iniciamatrizint(nvisitas, npilhas, ncam, 0);
        iniciavetoralocarga(carga, npilhas, 0, true);
        iniciavetorfloat(dispmec, ncam, 0.0);

        for (int j = 0; j < nvar; j++)
        {
            float amplitude = ls[j] - li[j];
            if (amplitude < 0.00001)
                amplitude = rec[j] * 0.05;

            // Normalização pela amplitude: quanto mais "apertada" a meta, maior o peso
            // Usamos 100.0 como base para não gerar números astronômicos quando multiplicar pelo wfteor global
            penalidadeteorinv[j] = (int)(100.0 / (amplitude + 0.001));

            // Para o status INV (Inviabilidade), o peso deve ser drasticamente maior
            // para que o VNS entenda que "ok, sair da meta dói, mas sair do limite é proibido"
            penalidadesup[j] = penalidadeteorinv[j] * 10;
            penalidadeinf[j] = penalidadeteorinv[j] * 10;
        }

        printf("\n--- VERIFICACAO DE PESOS DE TEOR ---\n");
        for (int j = 0; j < nvar; j++)
        {
            printf("Teor V%d: Inf=%d | Sup=%d | Inv=%d\n",
                   j, penalidadeinf[j], penalidadesup[j], penalidadeteorinv[j]);
        }

        printf("[INFO] Construindo solucao inicial (Gulosa)...\n");

        constroisolucaogulosa(
            s, carga, capcam, compatibilidade, nvisitas, nviagem,
            npilhas, ncam, nvar, teores, rec, npminerio,
            QTDisp, cargamax, cargamin, ncarga, prodrec, remrec);

        long long fo_inicial = favaliacao(
            s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv,
            npilhas, nvar, teores,
            prodmax, prodmin, prodrec,
            npminerio,
            remrec, remmax, remmin,
            nvisitas, carga, capcam,
            compatibilidade, tciclo, tcontrole,
            ncam, cargamax, cargamin, nummax,
            wfproducaoinv, wfproducaoqualp, wfproducaoqualn,
            wfteor, wfrem, wfremqualp, wfremqualn,
            wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn,
            wfprodcargainv, wfprodcargaqual,
            wfproducaocamtotal, wfproducaocarga,
            wfnumcamutilizados,
            wfestatico, wfnumcam, wfdist,
            dispmec, nviagem);

        printf("[INFO] FO inicial = %lld\n", fo_inicial);

        printf("[INFO] Executando VNS...\n");

        VNS(s, li, ls, rec,
            penalidadesup, penalidadeinf, penalidadeteorinv,
            npilhas, nvar, teores,
            prodmax, prodmin, prodrec,
            npminerio,
            remrec, remmax, remmin,
            nvisitas, carga, capcam,
            compatibilidade, tciclo, tcontrole,
            ncam, cargamax, cargamin, nummax,
            wfproducaoinv, wfproducaoqualp, wfproducaoqualn,
            wfteor, wfrem, wfremqualp, wfremqualn,
            wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn,
            wfprodcargainv, wfprodcargaqual,
            wfproducaocamtotal, wfproducaocarga,
            wfnumcamutilizados,
            wfestatico, wfnumcam, wfdist,
            dispmec, nviagem);

        long long fo_final = favaliacao(
            s, li, ls, rec,
            penalidadesup, penalidadeinf, penalidadeteorinv,
            npilhas, nvar, teores,
            prodmax, prodmin, prodrec,
            npminerio,
            remrec, remmax, remmin,
            nvisitas, carga, capcam,
            compatibilidade, tciclo, tcontrole,
            ncam, cargamax, cargamin, nummax,
            wfproducaoinv, wfproducaoqualp, wfproducaoqualn,
            wfteor, wfrem, wfremqualp, wfremqualn,
            wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn,
            wfprodcargainv, wfprodcargaqual,
            wfproducaocamtotal, wfproducaocarga,
            wfnumcamutilizados,
            wfestatico, wfnumcam, wfdist,
            dispmec, nviagem);

        printf("[INFO] FO final = %lld\n", fo_final);

        char relatorio[256];
        sprintf(relatorio, "Solucoes/Solucao_Mina_%d.txt", id_mina);

        printf("[INFO] Gerando relatorio...\n");

        imprimerelatorio(relatorio, 0, 1234, 0.0,
                         s, li, ls, rec,
                         penalidadesup, penalidadeinf, penalidadeteorinv,
                         npilhas, nvar, teores,
                         prodmax, prodmin, prodrec,
                         npminerio,
                         remrec, remmax, remmin,
                         nvisitas, carga, capcam,
                         compatibilidade, tciclo, tcontrole,
                         ncam, cargamax, cargamin, nummax,
                         wfproducaoinv, wfproducaoqualp, wfproducaoqualn,
                         wfteor, wfrem, wfremqualp, wfremqualn,
                         wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn,
                         wfprodcargainv, wfprodcargaqual,
                         wfproducaocamtotal, wfproducaocarga,
                         wfnumcamutilizados,
                         wfestatico, wfnumcam, wfdist,
                         dispmec, nviagem);

        printf("[OK] Mina %d finalizada\n", id_mina);
    }

    printf("\n=================================================\n");
    printf("PROCESSAMENTO FINALIZADO\n");
    printf("=================================================\n");

    return 0;
}
*/