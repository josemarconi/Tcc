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
#include "Hutilcarga.h"
#include "Hfavaliacao.h"
#include "Hsa.h"
#include "Hmovimento.h"

#include "hstruct.h"

//----------------------------------------------------------------------
struct alocarga *criavetoralocarga(int tam)
{
        struct alocarga *vetor;

        vetor = (struct alocarga *)malloc(tam * sizeof(struct alocarga));
        if (!vetor)
                Erro("Falta memoria para alocar o vetor de ponteiros");
        return vetor;
}
//----------------------------------------------------------------------
void carregavetors(float *s, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, int ncam, int npilhas)
{
        for (int i = 0; i < npilhas; i++)
        {
                s[i] = producaofrente(nvisitas, carga, capcam, compatibilidade, ncam, i);
        }
}

//----------------------------------------------------------------------
void constroisolucaoinicialcarga(struct alocarga *carga, float *s, int ncarga, int npilhas, int **nvisitas, float *capcam, int **compatibilidade, int ncam)
{
        //       printf("Construindo solucao inicial aleatoria para a aloca��o de equipamentos de carga...\n");

        for (int i = 0; i < ncarga; i++)
        {
                carga[i].equip = i;
        }

        //        for(int i=0;i<npilhas;i++){
        //                printf("%d\n",carga[i].equip);
        //        }getchar();

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

        carregavetors(s, nvisitas, carga, capcam, compatibilidade, ncam, npilhas);
}
//----------------------------------------------------------------------
void constroisolucaoinicialvisitasale(float *s, struct alocarga *carga, float *capcam, int **compatibilidade, int **nvisitas, int **nviagem, int npilhas, int ncam)
{
    for(int i=0; i<npilhas; i++) {
        for(int j=0; j<ncam; j++) {
            // Agora nviagem é específico por caminhão [i][j]
            if(nviagem[i][j] > 0)
                nvisitas[i][j] = randint(0, nviagem[i][j]);
            else
                nvisitas[i][j] = 0;
        }
    }
    carregavetors(s, nvisitas, carga, capcam, compatibilidade, ncam, npilhas);
}
void constroisolucaoinicialvisitas02(float *s, struct alocarga *carga, float *capcam, int **compatibilidade, int **nvisitas, int nviagemmax, int npilhas, int ncam)
{

        for (int i = 0; i < npilhas; i++)
        {
                printf("%d - %f\n", carga[i].equip, s[i]);
        }
        getchar();

        int nvdisp = 0;
        for (int j = 0; j < ncam; j++)
        {
                nvdisp = nviagemmax;
                for (int i = 0; i < npilhas; i++)
                {
                        if (carga[i].equip != disponivel && carga[i].op == true)
                        {
                                if (compatibilidade[j][carga[i].equip] == compativel)
                                {
                                        nvisitas[i][j] = randint(0, nvdisp);
                                        nvdisp -= nvisitas[i][j];
                                        if (nvdisp == 0)
                                                break;
                                }
                        }
                }
        }

        carregavetors(s, nvisitas, carga, capcam, compatibilidade, ncam, npilhas);

        for (int i = 0; i < npilhas; i++)
        {
                printf("%d - %f\n", carga[i].equip, s[i]);
        }
        getchar();
}

//----------------------------------------------------------------------
void constroisolucaoinicialvisitas(float *s, struct alocarga *carga, float *capcam, int **compatibilidade, int **nvisitas, int *nviagem, int npilhas, int ncam, int nviagemax)
{
        int escolha;
        bool ok = false;
        for (int j = 0; j < ncam; j++)
        {
                do
                {
                        escolha = randint(0, npilhas - 1);
                        ok = false;
                        if (carga[escolha].equip != disponivel && carga[escolha].op == true)
                                nvisitas[escolha][j] = nviagemax;
                        ok = true;
                } while (!ok);
        }
        carregavetors(s, nvisitas, carga, capcam, compatibilidade, ncam, npilhas);
}
//----------------------------------------------------------------------
//----------------------------------------------------------------------

float encontrado(float *s, float **teores, int npminerio, int varj)
{
        float soma1 = 0;
        float soma2 = 0;

        for (int i = 0; i < npminerio; i++)
        {
                soma1 += teores[i][varj] * s[i];
                soma2 += s[i];
        }
        if (soma2 == 0)
                return 0;

        return soma1 / soma2;
}

//----------------------------------------------------------------------
void iniciavetoralocarga(struct alocarga *vetor, int tam, float valor1, bool valor2)
{
        for (int i = 0; i < tam; i++)
        {
                vetor[i].equip = valor1;
                vetor[i].op = valor2;
        }
}

//----------------------------------------------------------------------
void liberavetoralocarga(struct alocarga *vetor)

{
        free(vetor);
}
//----------------------------------------------------------------------
float produtcam(int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int npilhas, int cam)
{
    float soma = 0.0;
    for(int i=0; i<npilhas; i++) {
        if(carga[i].equip != disponivel && carga[i].op == true) {
            if(compatibilidade[cam][carga[i].equip]) {
                // AQUI MUDOU: tciclo[i][cam]
                soma += nvisitas[i][cam] * tciclo[i][cam];
            }
        }
    }
    if(soma == 0) return 0.0;
    if(tcontrole == 0) return 0.0;

    return soma / tcontrole;
}
//----------------------------------------------------------------------
int dinamico(int **nvisitas, struct alocarga *carga, int **compatibilidade, int npilhas, int cam)
{
        int soma = 0;
        for (int i = 0; i < npilhas; i++)
        {
                if (carga[i].equip != disponivel && carga[i].op == true)
                        if (compatibilidade[cam][carga[i].equip] == compativel)
                        {
                                if (nvisitas[i][cam] > 0)
                                {
                                        soma++;
                                }
                        }
        }
        if (soma > 0)
                return (soma - 1);
        else
                return 0;
}
//----------------------------------------------------------------------
float producaofrente(int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, int ncam, int frente)
{
        float soma = 0.0;

        if (carga[frente].op == false)
                return 0.0;

        if (carga[frente].equip == disponivel)
                return 0.0;
        else
        {
                for (int k = 0; k < ncam; k++)
                        if (compatibilidade[k][carga[frente].equip] == compativel)
                                soma += nvisitas[frente][k] * capcam[k];
        }
        return soma;
}
//----------------------------------------------------------------------
void trocar(struct alocarga *carga, float *s, int pilha1, int pilha2, int **nvisitas, float *capcam, int **compatibilidade, int ncam)
{

        int aux = carga[pilha1].equip;
        int auxop = carga[pilha1].op;
        int auxs = s[pilha1];
        int *vetor = criavetor(ncam);
        for (int k = 0; k < ncam; k++)
                vetor[k] = nvisitas[pilha1][k];

        carga[pilha1].equip = carga[pilha2].equip;
        carga[pilha1].op = carga[pilha2].op;
        s[pilha1] = s[pilha2];
        for (int k = 0; k < ncam; k++)
                nvisitas[pilha1][k] = nvisitas[pilha2][k];

        carga[pilha2].equip = aux;
        carga[pilha2].op = auxop;
        s[pilha2] = auxs;
        for (int k = 0; k < ncam; k++)
                nvisitas[pilha2][k] = vetor[k];

        liberavetor(vetor);
}
//----------------------------------------------------------------------
void trocarnvisitas(bool *ok, int **nvisitas, int *nviagem, int passo, bool tipomov, int pilha, int caminhao)
{
        *ok = false;
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
                if (nvisitas[pilha][caminhao] - passo >= 0)
                {
                        nvisitas[pilha][caminhao] += -passo;
                        *ok = true;
                }
        }
}
//----------------------------------------------------------------------
void trocaroperacao(struct alocarga *carga, float *s, int pilha, int **nvisitas, float *capcam, int **compatibilidade, int ncam, int npilhas)
{

        if (carga[pilha].op == false)
                carga[pilha].op = true;
        else
                carga[pilha].op = false;

        carregavetors(s, nvisitas, carga, capcam, compatibilidade, ncam, npilhas);
}
//----------------------------------------------------------------------
void trocarviagemcaminhaofrente(int **nvisitas, int pilha, int caminhao1, int caminhao2, int passo)
{
        nvisitas[pilha][caminhao1] += passo;
        nvisitas[pilha][caminhao2] -= passo;
}
//----------------------------------------------------------------------
void trocarviagemcaminhaocam(int **nvisitas, int cam, int pilha1, int pilha2, int passo)
{
        nvisitas[pilha1][cam] += passo;
        nvisitas[pilha2][cam] -= passo;
}
//----------------------------------------------------------------------
void atualizamelhorsolucaoalocarga(struct alocarga *s, struct alocarga *star, int tam)
{
        for (int i = 0; i < tam; i++)
        {
                s[i].equip = star[i].equip;
                s[i].op = star[i].op;
        }
}
//----------------------------------------------------------------------
int numcamfrente(int **nvisitas, struct alocarga *carga, int **compatibilidade, int ncam, int pilha, int *nummax)
{
        int soma = 0;
        if (carga[pilha].equip == disponivel)
                return 0;
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
void imprimerelatorio(char nomearq[], int metodo, unsigned long seed, float tempo, float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem)
{
        register int i, j, k;
        FILE *arquivo;
        int *camutil = criavetor(ncam);
        bool viavel;
        float enc = 0.0;
        float auxcam = 0.0;
        int totalcam = 0;
        float soma1 = 0.00000001;
        float soma2 = 0;
        float utilizacao;
        arquivo = fopen(nomearq, "w");
        if (!arquivo)
                Erro("O arquivo nome nao pode ser aberto");
        float producaomin = prodtotal(s, npminerio);
        float producaototal = prodtotal(s, npilhas);
        float rem = 0.0;
        long long fo = 0;
        float soma3 = 0.0;
        float soma4 = 0.0;

        fo = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

        for (j = 0; j < nvar; j++)
        {
                for (i = 0; i < npminerio; i++)
                {
                        soma3 += ((teores[i][j] - rec[j]) * s[i]);
                }
                soma4 += absfloat(soma3);
                soma3 = 0;
        }

        if (estdin == 1)
                fprintf(arquivo, "Relat�rio para a alocacao ESTATICA de caminh�es utilizando o metodo ");
        else
                fprintf(arquivo, "Relat�rio para a alocacao DINAMICA de caminh�es utilizando o metodo ");

        if (metodo == 1)
                fprintf(arquivo, "VND");
        else
                fprintf(arquivo, "VNS");

        // fprintf(arquivo,"Seed: %d\nAbs: %d\nSoma: %f\nTempo: %f\nFo: %d",seed,abs(producaomin-prodrec),soma4,tempo,fo);
        // fprintf(arquivo,"\n\n\nSeed: %d\n",seed;
        // fprintf(arquivo,"Abs: %d",seed,abs(producaomin-prodrec),soma4,tempo,fo);
        // fprintf(arquivo,"Soma: %f\n",seed,abs(producaomin-prodrec),soma4,tempo,fo);
        fprintf(arquivo, "\n\n\nTempo: %f\n", tempo);
        fprintf(arquivo, "Fo: %lld\n\n\n", fo);

        if (producaomin != 0)
        {
                fprintf(arquivo, "Producao de Minerio %8.2f  [%8.1f -%8.1f -%8.1f]\n", producaomin, prodmin, prodrec, prodmax);
                fprintf(arquivo, "Producao de Esteril %8.2f\n", producaototal - producaomin);
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

        fprintf(arquivo, "\t fa_producao %12lld\n", fa_producao(producaomin, prodmax, prodmin, prodrec, wfproducaoinv, wfproducaoqualp, wfproducaoqualn));
        long long auxteor = 0;
        for (int j = 0; j < nvar; j++)
        {
                auxteor += fa_teor(encontrado(s, teores, npminerio, j), ls[j], li[j], rec[j], penalidadeteorinv[j], penalidadesup[j], penalidadeinf[j]);
        }
        fprintf(arquivo, "\t fa_teor     %12lld\n", auxteor);

        fprintf(arquivo, "\t fa_rem      %12lld\n", fa_rem(rem, remmax, remmin, remrec, wfrem, wfremqualp, wfremqualn));

        long long auxpcam = 0;
        for (int l = 0; l < ncam; l++)
        {
                auxpcam += fa_produtcam(produtcam(nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, npilhas, l), 1.0, 0.0, dispmec[l], wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn);
        }
        fprintf(arquivo, "\t fa_produtcam%12lld\n", auxpcam);

        long long auxcarga = 0;
        for (int i = 0; i < npilhas; i++)
        {

                auxcarga += fa_prodcarga(s[i], cargamax[carga[i].equip], cargamin[carga[i].equip], wfprodcargainv, wfprodcargaqual);
        }
        fprintf(arquivo, "\t fa_prodcarga%12lld\n", auxcarga);
        fprintf(arquivo, "\t fa_totalcam %12lld\n", fa_numcam(nvisitas, carga, compatibilidade, npilhas, ncam, wfnumcamutilizados));

        fprintf(arquivo, "\t fa_estatico %12lld\n", fa_estatico(nvisitas, carga, compatibilidade, npilhas, ncam, wfestatico));
        fprintf(arquivo, "\t fa_numcamfre%12lld\n", fa_numcamfrente(nvisitas, carga, compatibilidade, npilhas, ncam, wfnumcam, nummax));
        fprintf(arquivo, "\t--------------------------\n");

        //******************************************************************************
        fprintf(arquivo, "\n\n\nEquipamentos de Carga\n\n\n");

        fprintf(arquivo, "\t N�    Frente   [Prod Min - Prod Max]   Ritmo de Lavra     Pontuacao   STATUS\n");
        fprintf(arquivo, "\t------------------------------------------------------------------------------\n");

        for (i = 0; i < npilhas; i++)
        {
                if (carga[i].equip != disponivel && s[i] != 0)
                {
                        fprintf(arquivo, "\t %2d       %2d    [%8.1f  -%8.1f]     %8.1f    %10lld", carga[i].equip, i, cargamin[carga[i].equip], cargamax[carga[i].equip], s[i], fa_prodcarga(s[i], cargamax[carga[i].equip], cargamin[carga[i].equip], wfprodcargainv, wfprodcargaqual));
                        if (s[i] >= cargamin[carga[i].equip] && s[i] <= cargamax[carga[i].equip])
                                fprintf(arquivo, "         ok!\n");
                        else
                        {
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

        for (j = 0; j < nvar; j++)
        {
                enc = encontrado(s, teores, npminerio, j);
                fprintf(arquivo, "\t  %2d       [%8.4f -%8.4f -%8.4f]   %8.4f     %8.4f %10lld", j, li[j], rec[j], ls[j], enc, enc - rec[j], fa_teor(encontrado(s, teores, npminerio, j), ls[j], li[j], rec[j], penalidadeteorinv[j], penalidadesup[j], penalidadeinf[j]));
                if (enc >= li[j] && enc <= ls[j])
                        fprintf(arquivo, "        ok!\n");
                else
                {
                        viavel = false;
                        fprintf(arquivo, "        INV\n");
                }
        }
        fprintf(arquivo, "\t-----------------------------------------------------------------------------------------\n");

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

        liberavetor(camutil);

        fclose(arquivo);
}
//----------------------------------------------------------------------
int camutilizados(int **nvisitas, struct alocarga *carga, int **compatibilidade, int npilhas, int cam)
{
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
void carregatempociclo(int npilhas, float *tciclo, float *tdv, float *tdc, float *tcarga, float *tbasc)
{
        for (int i = 0; i < npilhas; i++)
        {
                tciclo[i] = tdv[i] + tdc[i] + tcarga[i] + tbasc[i];
        }
}

//----------------------------------------------------------------------
void carreganummax(int npilhas, int *nummax, float *tciclo, float *tcarga)
{
        for (int i = 0; i < npilhas; i++)
                nummax[i] = (int)(floor(tciclo[i] / tcarga[i]));
}
//----------------------------------------------------------------------
void carreganviagem(int npilhas, int *nviagem, float tcontrole, float *tciclo)
{
        for (int i = 0; i < npilhas; i++)
                nviagem[i] = (int)(floor(tcontrole / tciclo[i]));
}
//----------------------------------------------------------------------

void constroisolucaogulosa(float *s, struct alocarga *carga, float *capcam, int **compatibilidade, int **nvisitas, int **nviagem, int npilhas, int ncam, int nvar, float **teores, float *rec, int npminerio, float *QTDisp, float *cargamax, float *cargamin, int ncarga, float prodrec, float remrec)
{
    bool *loader_used = (bool*)calloc(ncarga, sizeof(bool));
    
    // 1. Ordenação de Carregadeiras (Menores primeiro)
    int *sorted_loaders = (int*)malloc(ncarga * sizeof(int));

    float prod_minerio_atual = 0.0;
    float prod_esteril_atual = 0.0;
    float meta_esteril = prodrec * remrec; // Proporção ideal de estéril baseada no REM

    for(int i=0; i<ncarga; i++){sorted_loaders[i] = i;}
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

    // 2. Ordenação de Caminhões (Maiores Primeiro)
    int *sorted_trucks = (int*)malloc(ncam * sizeof(int));
    for(int i=0; i<ncam; i++) {sorted_trucks[i] = i;}
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

    // --- 1: ESTÉRIL (Frentes de npminerio até npilhas-1) ---
    int num_est = npilhas - npminerio;
    if (num_est > 0) 
    {
        int *est_faces = (int*)malloc(num_est * sizeof(int));
        for(int i=0; i<num_est; i++){est_faces[i] = npminerio + i;}
        
        // Ordenar frentes de estéril por massa (QTDisp) decrescente
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

        int l_idx = 0; // Menor carregadeira
        for(int i=0; i<num_est; i++) 
        {
            if (prod_esteril_atual >= meta_esteril) break; //Estéril suficiente para o REM

            int f = est_faces[i];
            while(l_idx < ncarga && loader_used[sorted_loaders[l_idx]]) {l_idx++;}
            if(l_idx < ncarga)
            {
                int l_id = sorted_loaders[l_idx];
                carga[f].equip = l_id;
                loader_used[l_id] = true;
                
                // Estéril: Começa com os caminhões maiores
                for(int j=0; j<ncam; j++) 
                {
                    int c = sorted_trucks[j];
                    if(compatibilidade[c][l_id] == compativel) 
                    {
                        float falta_esteril = meta_esteril - prod_esteril_atual;
                        if (falta_esteril <= 0) break;

                        int v = nviagem[f][c];
                        if(s[f] + v*capcam[c] > QTDisp[f]) v = (int)((QTDisp[f] - s[f]) / capcam[c]);
                        if(v * capcam[c] > falta_esteril) v = (int)ceil(falta_esteril / capcam[c]);

                        nvisitas[f][c] = v;
                        s[f] += v * capcam[c];
                        prod_esteril_atual += v * capcam[c];
                    }
                }
            }
        }
        free(est_faces);
    }

    // --- 2: MINÉRIO (Frentes de 0 até npminerio-1) ---
    if (npminerio > 0) 
    {
        int *ore_faces = (int*)malloc(npminerio * sizeof(int));
        float *deviations = (float*)malloc(npminerio * sizeof(float));
        for(int i=0; i<npminerio; i++) 
        {
            ore_faces[i] = i;
            deviations[i] = 0;
            for(int j=0; j<nvar; j++){deviations[i] += absfloat(teores[i][j] - rec[j]);}
        }
        
        // Ordenar por menor desvio
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

        int l_idx = ncarga - 1; // Maior carregadeira
        for(int i=0; i<npminerio; i++) 
        {
            if (prod_minerio_atual >= prodrec) break; //Meta de minério atingida

            int f = ore_faces[i];
            while(l_idx >= 0 && loader_used[sorted_loaders[l_idx]]) {l_idx--;}
            if(l_idx >= 0) 
            {
                int l_id = sorted_loaders[l_idx];
                carga[f].equip = l_id;
                loader_used[l_id] = true;
                
                // Minério: Caminhão de menor capacidade
                for(int j=ncam-1; j>=0; j--) 
                {
                    int c = sorted_trucks[j];
                    if(compatibilidade[c][l_id] == compativel) 
                    {
                        float falta_para_meta = prodrec - prod_minerio_atual;
                        if (falta_para_meta <= 0) break;

                        int v = nviagem[f][c];
                        // Limite pela massa da frente
                        if(s[f] + v*capcam[c] > QTDisp[f]) v = (int)((QTDisp[f] - s[f]) / capcam[c]);
                        // Limite pela meta de produção
                        if(v * capcam[c] > falta_para_meta) v = (int)ceil(falta_para_meta / capcam[c]);

                        nvisitas[f][c] = v;
                        s[f] += v * capcam[c];
                        prod_minerio_atual += v * capcam[c];
                    }
                }
            }
        }
        free(ore_faces);
        free(deviations);
    }

    carregavetors(s, nvisitas, carga, capcam, compatibilidade, ncam, npilhas);

    free(loader_used); 
    free(sorted_loaders); 
    free(sorted_trucks);
}


void constroisolucaoGRASP(float *s, struct alocarga *carga, float *capcam, int **compatibilidade, int **nvisitas, int **nviagem, int npilhas, int ncam, int nvar, float **teores, float *rec, int npminerio, float *QTDisp, float *cargamax, float *cargamin, int ncarga, float prodrec, float remrec, float alpha)
{
    // 1. Limpeza e Inicialização
    iniciavetorfloat(s, npilhas, 0.0);
    iniciamatrizint(nvisitas, npilhas, ncam, 0);
    bool *loader_used = (bool*)calloc(ncarga, sizeof(bool));
    float prod_minerio_atual = 0.0;
    float prod_esteril_atual = 0.0;
    float meta_esteril = prodrec * remrec;

    // 2. Ordenação de Carregadeiras (Menores primeiro)
    int *sorted_loaders = (int*)malloc(ncarga * sizeof(int));
    for(int i=0; i<ncarga; i++){sorted_loaders[i] = i;}
    for(int i=0; i<ncarga-1; i++) {
        for(int j=i+1; j<ncarga; j++) {
            if(cargamax[sorted_loaders[i]] > cargamax[sorted_loaders[j]]) {
                int temp = sorted_loaders[i];
                sorted_loaders[i] = sorted_loaders[j];
                sorted_loaders[j] = temp;
            }
        }
    }

    // 3. Ordenação de Caminhões (Maiores Primeiro)
    int *sorted_trucks = (int*)malloc(ncam * sizeof(int));
    for(int i=0; i<ncam; i++) {sorted_trucks[i] = i;}
    for(int i=0; i<ncam-1; i++) {
        for(int j=i+1; j<ncam; j++) {
            if(capcam[sorted_trucks[i]] < capcam[sorted_trucks[j]]) {
                int temp = sorted_trucks[i];
                sorted_trucks[i] = sorted_trucks[j];
                sorted_trucks[j] = temp;
            }
        }
    }

    // --- FASE GRASP: ESTÉRIL ---
    int num_est = npilhas - npminerio;
    if (num_est > 0) 
    {
        int *est_faces = (int*)malloc(num_est * sizeof(int));
        for(int i=0; i<num_est; i++){est_faces[i] = npminerio + i;}
        
        // Ordenar frentes de estéril por massa (QTDisp) decrescente
        for(int i=0; i<num_est-1; i++) {
            for(int j=i+1; j<num_est; j++) {
                if(QTDisp[est_faces[i]] < QTDisp[est_faces[j]]) {
                    int temp = est_faces[i]; est_faces[i] = est_faces[j]; est_faces[j] = temp;
                }
            }
        }

        int l_idx = 0; 
        for(int i=0; i<num_est; i++) 
        {
            if (prod_esteril_atual >= meta_esteril) break;

            // APLICAÇÃO GRASP: Sorteio na LRC das frentes de estéril
            int restante = num_est - i;
            int range_lrc = (int)ceil(restante * alpha);
            if (range_lrc < 1) range_lrc = 1;
            int escolhido_idx = i + (rand() % range_lrc);
            
            int f = est_faces[escolhido_idx];
            // Swap para manter a frente usada fora do próximo sorteio
            int temp_f = est_faces[i]; est_faces[i] = est_faces[escolhido_idx]; est_faces[escolhido_idx] = temp_f;

            while(l_idx < ncarga && loader_used[sorted_loaders[l_idx]]) {l_idx++;}
            if(l_idx < ncarga)
            {
                int l_id = sorted_loaders[l_idx];
                carga[f].equip = l_id;
                loader_used[l_id] = true;
                
                for(int j=0; j<ncam; j++) 
                {
                    int c = sorted_trucks[j];
                    if(compatibilidade[c][l_id] == 1) 
                    {
                        float falta_esteril = meta_esteril - prod_esteril_atual;
                        if (falta_esteril <= 0) break;

                        int v = nviagem[f][c];
                        if(s[f] + v*capcam[c] > QTDisp[f]) v = (int)((QTDisp[f] - s[f]) / capcam[c]);
                        if(v * capcam[c] > falta_esteril) v = (int)ceil(falta_esteril / capcam[c]);

                        nvisitas[f][c] = v;
                        s[f] += v * capcam[c];
                        prod_esteril_atual += v * capcam[c];
                    }
                }
            }
        }
        free(est_faces);
    }

    // --- FASE GRASP: MINÉRIO ---
    if (npminerio > 0) 
    {
        int *ore_faces = (int*)malloc(npminerio * sizeof(int));
        float *deviations = (float*)malloc(npminerio * sizeof(float));
        for(int i=0; i<npminerio; i++) 
        {
            ore_faces[i] = i;
            deviations[i] = 0;
            for(int j=0; j<nvar; j++){deviations[i] += absfloat(teores[i][j] - rec[j]);}
        }
        
        // Ordenar por menor desvio (Guloso)
        for(int i=0; i<npminerio-1; i++) {
            for(int j=i+1; j<npminerio; j++) {
                if(deviations[i] > deviations[j]) {
                    float td = deviations[i]; deviations[i] = deviations[j]; deviations[j] = td;
                    int tf = ore_faces[i]; ore_faces[i] = ore_faces[j]; ore_faces[j] = tf;
                }
            }
        }

        int l_idx = ncarga - 1; // Maior carregadeira
        for(int i=0; i<npminerio; i++) 
        {
            if (prod_minerio_atual >= prodrec) break;

            // APLICAÇÃO GRASP: Sorteio na LRC das frentes de minério
            int restante = npminerio - i;
            int range_lrc = (int)ceil(restante * alpha);
            if (range_lrc < 1) range_lrc = 1;
            int escolhido_idx = i + (rand() % range_lrc);
            
            int f = ore_faces[escolhido_idx];
            int temp_f = ore_faces[i]; ore_faces[i] = ore_faces[escolhido_idx]; ore_faces[escolhido_idx] = temp_f;

            while(l_idx >= 0 && loader_used[sorted_loaders[l_idx]]) {l_idx--;}
            if(l_idx >= 0) 
            {
                int l_id = sorted_loaders[l_idx];
                carga[f].equip = l_id;
                loader_used[l_id] = true;
                
                for(int j=ncam-1; j>=0; j--) 
                {
                    int c = sorted_trucks[j];
                    if(compatibilidade[c][l_id] == 1) 
                    {
                        float falta_para_meta = prodrec - prod_minerio_atual;
                        if (falta_para_meta <= 0) break;

                        int v = nviagem[f][c];
                        if(s[f] + v*capcam[c] > QTDisp[f]) v = (int)((QTDisp[f] - s[f]) / capcam[c]);
                        if(v * capcam[c] > falta_para_meta) v = (int)ceil(falta_para_meta / capcam[c]);

                        nvisitas[f][c] = v;
                        s[f] += v * capcam[c];
                        prod_minerio_atual += v * capcam[c];
                    }
                }
            }
        }
        free(ore_faces);
        free(deviations);
    }

    free(loader_used); 
    free(sorted_loaders); 
    free(sorted_trucks);
}

void constroisolucaoGRASP_Parcial(float *s, struct alocarga *carga, float *capcam, int **compatibilidade, int **nvisitas, int **nviagem, int npilhas, int ncam, int nvar, float **teores, float *rec, int npminerio, float *QTDisp, float *cargamax, float *cargamin, int ncarga, float prodrec, float remrec, float alpha, bool reconstruir_apenas_minerio)
{
    bool *loader_used = (bool*)calloc(ncarga, sizeof(bool));
    int *sorted_loaders = (int*)malloc(ncarga * sizeof(int));
    int *sorted_trucks = (int*)malloc(ncam * sizeof(int));
    
    // Se estivermos reconstruindo apenas minério, precisamos marcar as carregadeiras do estéril como usadas
    if(reconstruir_apenas_minerio) {
        for(int i = npminerio; i < npilhas; i++) {
            if(carga[i].equip != -1) loader_used[carga[i].equip] = true;
        }
    } else {
        iniciavetorfloat(s, npilhas, 0.0);
        iniciamatrizint(nvisitas, npilhas, ncam, 0);
    }

    // --- Ordenações Base ---
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

    // --- BLOCO ESTÉRIL (SÓ EXECUTA SE NÃO FOR RECONSTRUÇÃO PARCIAL) ---
    if (!reconstruir_apenas_minerio) {
        float prod_esteril_atual = 0.0;
        float meta_esteril = prodrec * remrec;
        int num_est = npilhas - npminerio;
        if (num_est > 0) {
            int *est_faces = (int*)malloc(num_est * sizeof(int));
            for(int i=0; i<num_est; i++){est_faces[i] = npminerio + i;}
            // Ordenação por massa decrescente
            for(int i=0; i<num_est-1; i++) {
                for(int j=i+1; j<num_est; j++) {
                    if(QTDisp[est_faces[i]] < QTDisp[est_faces[j]]) {
                        int temp = est_faces[i]; est_faces[i] = est_faces[j]; est_faces[j] = temp;
                    }
                }
            }
            int l_idx = 0; 
            for(int i=0; i<num_est; i++) {
                if (prod_esteril_atual >= meta_esteril) break;
                int r_lrc = (int)ceil((num_est - i) * alpha); if (r_lrc < 1) r_lrc = 1;
                int f_idx = i + (rand() % r_lrc);
                int f = est_faces[f_idx];
                int temp_f = est_faces[i]; est_faces[i] = est_faces[f_idx]; est_faces[f_idx] = temp_f;

                while(l_idx < ncarga && loader_used[sorted_loaders[l_idx]]) {l_idx++;}
                if(l_idx < ncarga) {
                    int l_id = sorted_loaders[l_idx];
                    carga[f].equip = l_id; loader_used[l_id] = true;
                    for(int j=0; j<ncam; j++) {
                        int c = sorted_trucks[j];
                        if(compatibilidade[c][l_id] == 1) {
                            float falta = meta_esteril - prod_esteril_atual;
                            if (falta <= 0) break;
                            int v = nviagem[f][c];
                            if(s[f] + v*capcam[c] > QTDisp[f]) v = (int)((QTDisp[f] - s[f]) / capcam[c]);
                            if(v * capcam[c] > falta) v = (int)ceil(falta / capcam[c]);
                            nvisitas[f][c] = v; s[f] += v * capcam[c]; prod_esteril_atual += v * capcam[c];
                        }
                    }
                }
            }
            free(est_faces);
        }
    }

    // --- BLOCO MINÉRIO (SEMPRE RECONSTRÓI) ---
    float prod_min_atual = 0.0;
    // Se for parcial, precisamos resetar apenas as frentes de minério
    for(int i=0; i < npminerio; i++) {
        s[i] = 0;
        for(int j=0; j < ncam; j++) nvisitas[i][j] = 0;
        carga[i].equip = -1;
    }

    int *ore_faces = (int*)malloc(npminerio * sizeof(int));
    float *deviations = (float*)malloc(npminerio * sizeof(float));
    for(int i=0; i<npminerio; i++) {
        ore_faces[i] = i; deviations[i] = 0;
        for(int j=0; j<nvar; j++){ deviations[i] += absfloat(teores[i][j] - rec[j]); }
    }
    // Sort por menor desvio
    for(int i=0; i<npminerio-1; i++) {
        for(int j=i+1; j<npminerio; j++) {
            if(deviations[i] > deviations[j]) {
                float td = deviations[i]; deviations[i] = deviations[j]; deviations[j] = td;
                int tf = ore_faces[i]; ore_faces[i] = ore_faces[j]; ore_faces[j] = tf;
            }
        }
    }
    int l_idx_ore = ncarga - 1;
    for(int i=0; i<npminerio; i++) {
        if (prod_min_atual >= prodrec) break;
        int r_lrc = (int)ceil((npminerio - i) * alpha); if (r_lrc < 1) r_lrc = 1;
        int f_idx = i + (rand() % r_lrc);
        int f = ore_faces[f_idx];
        int temp_f = ore_faces[i]; ore_faces[i] = ore_faces[f_idx]; ore_faces[f_idx] = temp_f;

        while(l_idx_ore >= 0 && loader_used[sorted_loaders[l_idx_ore]]) {l_idx_ore--;}
        if(l_idx_ore >= 0) {
            int l_id = sorted_loaders[l_idx_ore];
            carga[f].equip = l_id; loader_used[l_id] = true;
            for(int j=ncam-1; j>=0; j--) {
                int c = sorted_trucks[j];
                if(compatibilidade[c][l_id] == 1) {
                    float falta = prodrec - prod_min_atual;
                    if (falta <= 0) break;
                    int v = nviagem[f][c];
                    if(s[f] + v*capcam[c] > QTDisp[f]) v = (int)((QTDisp[f] - s[f]) / capcam[c]);
                    if(v * capcam[c] > falta) v = (int)ceil(falta / capcam[c]);
                    nvisitas[f][c] = v; s[f] += v * capcam[c]; prod_min_atual += v * capcam[c];
                }
            }
        }
    }
    free(ore_faces); free(deviations); free(loader_used); free(sorted_loaders); free(sorted_trucks);
}