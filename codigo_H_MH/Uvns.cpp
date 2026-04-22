//#include <vcl.h>
//#include <condefs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <limits.h>
#include <ctype.h>
#include <time.h>
#include <float.h>
#include <errno.h>
#include <math.h>
//------------------------------------------------------------------------------
#include "Hutil.h"
#include "Hdefine.h"
#include "Hutilcarga.h"
#include "Hfavaliacao.h"
#include "Hmovimento.h"
#include "Hsa.h"
#include "Hvnd.h"
//------------------------------------------------------------------------------
//cl√°ssico
int VNSv1(float *s,float *li,float *ls,float *rec,int *penalidadesup
            ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
            ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
            ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
            ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
            ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
            , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem
            , int metodo, char *inst)
{
    int pilha1;
    int pilha2;
    int coluna1;
    int coluna2;
    int pilhaop;
    int pilha;
    int cam,caminhao,caminhao1,caminhao2;
    bool tipomov,tipomovop;
    int passo;

    int focorrente,fovizinho, fomin;
    int **star_nvisitas;
    struct alocarga *star_carga;
    float tempo;
    clock_t inicioVNS, fimVNS; // marcador de tempo
        inicioVNS = clock();

    star_nvisitas = criamatriz(npilhas,ncam);
    star_carga = criavetoralocarga(npilhas);

    atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(star_carga,carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

    //printf("***fomin_VNS = %8d\n",fomin);

    focorrente = fovizinho = fomin = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

    //printf("***fomin_VNS = %8d\n",fomin);
    //printf("***focorrente_VNS = %8d\n",focorrente);
    int k = 1;


     fimVNS = clock();
     tempo = (float)(fimVNS - inicioVNS)/CLOCKS_PER_SEC ;

     if(estdin ==1) imprimevalordefo("foVNS_PAEC.rel", tempo, fomin, focorrente);
     else imprimevalordefo("foVNS_PADC.rel", tempo, fomin, focorrente);

     //while(tempo < 900.0) {//while 1

        k=1;
        while(k <= 6) //while 1
        {
           switch(k){
                case 1: movimentovisita(nvisitas,compatibilidade,carga,1, npilhas,ncam, nviagem, &pilha,&caminhao, &tipomov,s,capcam);
                break;
                case 2: movimentooperacaocam( nvisitas, compatibilidade, carga, passo, npilhas, ncam, nviagem, &cam, &pilha, s, capcam);
                break;
                case 3: movimentoviagemfrente( nvisitas, compatibilidade, carga,  passo,  npilhas, ncam, nviagem, &pilha, &caminhao1, &caminhao2, s, capcam);
                break;
                case 4: movimentoviagemcam( nvisitas, compatibilidade, carga,  passo,  npilhas, ncam, nviagem, &cam, &pilha1, &pilha2, s, capcam);
                break;
                case 5: movimentocarga(carga,s,npilhas,&pilha1, &pilha2, nvisitas, capcam,compatibilidade,ncam);
                break;
                case 6: movimentooperacao( carga, s, npilhas, &pilhaop,&tipomovop, nvisitas, capcam, compatibilidade, ncam);
                break;
           }
           //printf("tempo %8.2f\t %12d\n",tempo,fomin);

           focorrente = VNDvC(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);


            fimVNS = clock();
            tempo = (float)(fimVNS - inicioVNS)/CLOCKS_PER_SEC ;

            if ( focorrente < fomin){
                fomin = focorrente;
                atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
                atualizamelhorsolucaoalocarga(star_carga, carga,npilhas);
                k = 1;
                /*
                imprimerelatorio("MelhorSolucao.rel",2,8521, tempo,s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
                /**/

            }
            else k++;
            //printf("***tempo %6.1f   focorr %8d  fomin_VNS = %8d\n",tempo, focorrente,fomin);

            if(estdin ==1) imprimevalordefo("foVNS_PAEC.rel", tempo, fomin, focorrente);
            else  imprimevalordefo("foVNS_PADC.rel", tempo, fomin, focorrente);
        //}//while 1
        //printf("***tempo %6.1f   focorr %8d  fomin_VNS = %8d\n",tempo, focorrente,fomin);
    }
    //		printf("***fomin_VNS = %8d\n",fomin);
    atualizamatrizint(nvisitas,star_nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(carga,star_carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);
    liberamatriz(star_nvisitas,npilhas);
    liberavetoralocarga(star_carga);

    imprimeRel(fomin,metodo,tempo,inst);
	






//////////////////////////////////////////////////////////////////
FILE *fptr, *fptr1;
fptr = fopen("dadostempo_1.txt", "a");
fptr1 = fopen("dadosfo_1.txt", "a");
fprintf(fptr1,"%d\n", fomin);
fprintf(fptr,"%f\n", tempo);
fclose(fptr);	
fclose(fptr1);	
/////////////////////////////////////////////////////////////////






    return fomin;
}

//------------------------------------------------------------------------------

int VNSv2(float *s,float *li,float *ls,float *rec,int *penalidadesup
            ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
            ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
            ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
            ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
            ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
            , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem
            , int metodo, char *inst)
{
    int pilha1;
    int pilha2;
    int coluna1;
    int coluna2;
    int pilhaop;
    int pilha;
    int cam,caminhao,caminhao1,caminhao2;
    bool tipomov,tipomovop;
    int passo;

    int focorrente,fovizinho, fomin;
    int **star_nvisitas;
    struct alocarga *star_carga;
    float tempo;
    clock_t inicioVNS, fimVNS; // marcador de tempo
        inicioVNS = clock();

    star_nvisitas = criamatriz(npilhas,ncam);
    star_carga = criavetoralocarga(npilhas);

    atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(star_carga,carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

    //printf("***fomin_VNS = %8d\n",fomin);

    focorrente = fovizinho = fomin = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

    //printf("***fomin_VNS = %8d\n",fomin);
    //printf("***focorrente_VNS = %8d\n",focorrente);
    int k = 1;


     fimVNS = clock();
     tempo = (float)(fimVNS - inicioVNS)/CLOCKS_PER_SEC ;

     if(estdin ==1) imprimevalordefo("foVNS_PAEC.rel", tempo, fomin, focorrente);
     else imprimevalordefo("foVNS_PADC.rel", tempo, fomin, focorrente);

      //while(tempo < 900.0) {//while 1
        k=1;
        while(k <= 6) //while 1
        {
           switch(k){
                case 1: movimentovisita(nvisitas,compatibilidade,carga,1, npilhas,ncam, nviagem, &pilha,&caminhao, &tipomov,s,capcam);
                break;
                case 2: movimentooperacaocam( nvisitas, compatibilidade, carga, passo, npilhas, ncam, nviagem, &cam, &pilha, s, capcam);
                break;
                case 3: movimentoviagemfrente( nvisitas, compatibilidade, carga,  passo,  npilhas, ncam, nviagem, &pilha, &caminhao1, &caminhao2, s, capcam);
                break;
                case 4: movimentoviagemcam( nvisitas, compatibilidade, carga,  passo,  npilhas, ncam, nviagem, &cam, &pilha1, &pilha2, s, capcam);
                break;
                case 5: movimentocarga(carga,s,npilhas,&pilha1, &pilha2, nvisitas, capcam,compatibilidade,ncam);
                break;
                case 6: movimentooperacao( carga, s, npilhas, &pilhaop,&tipomovop, nvisitas, capcam, compatibilidade, ncam);
                break;
           }
           //printf("tempo %8.2f\t %12d\n",tempo,fomin);

           focorrente = VNDvALE1(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);


            fimVNS = clock();
            tempo = (float)(fimVNS - inicioVNS)/CLOCKS_PER_SEC ;

            if ( focorrente < fomin){
                fomin = focorrente;
                atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
                atualizamelhorsolucaoalocarga(star_carga, carga,npilhas);
                k = 1;
                                /*
                imprimerelatorio("MelhorSolucao.rel",2,8521, tempo,s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
                /**/
            }
            else k++;
            //printf("***tempo %6.1f   focorr %8d  fomin_VNS = %8d\n",tempo, focorrente,fomin);

            if(estdin ==1) imprimevalordefo("foVNS_PAEC.rel", tempo, fomin, focorrente);
            else  imprimevalordefo("foVNS_PADC.rel", tempo, fomin, focorrente);
        //}//while 1
        //printf("***tempo %6.1f   focorr %8d  fomin_VNS = %8d\n",tempo, focorrente,fomin);
    }
    //    printf("***fomin_VNS = %8d\n",fomin);
    atualizamatrizint(nvisitas,star_nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(carga,star_carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);
    liberamatriz(star_nvisitas,npilhas);
    liberavetoralocarga(star_carga);
    imprimeRel(fomin,metodo,tempo,inst);

		






//////////////////////////////////////////////////////////////////
FILE *fptr, *fptr1;
fptr = fopen("dadostempo_2.txt", "a");
fptr1 = fopen("dadosfo_2.txt", "a");
fprintf(fptr1,"%d\n", fomin);
fprintf(fptr,"%f\n", tempo);
fclose(fptr);	
fclose(fptr1);	
/////////////////////////////////////////////////////////////////








    return fomin;
}

//------------------------------------------------------------------------------


int VNSv3(float *s,float *li,float *ls,float *rec,int *penalidadesup
            ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
            ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
            ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
            ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
            ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
            , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem
            , int metodo, char *inst)
{
    int pilha1;
    int pilha2;
    int coluna1;
    int coluna2;
    int pilhaop;
    int pilha;
    int cam,caminhao,caminhao1,caminhao2;
    bool tipomov,tipomovop;
    int passo;

    int focorrente,fovizinho, fomin;
    int **star_nvisitas;
    struct alocarga *star_carga;
    float tempo;
clock_t inicioVNS, fimVNS; // marcador de tempo
        inicioVNS = clock();

    star_nvisitas = criamatriz(npilhas,ncam);
    star_carga = criavetoralocarga(npilhas);

    atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(star_carga,carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

focorrente = fovizinho = fomin = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

int k, stop = 1;


      fimVNS = clock();
      tempo = (float)(fimVNS - inicioVNS)/CLOCKS_PER_SEC ;

       if(estdin ==1)
       imprimevalordefo("foVNS_PAEC.rel", tempo, fomin, focorrente);
       else
       imprimevalordefo("foVNS_PADC.rel", tempo, fomin, focorrente);

 //while(tempo < 900.0) {//while 1


// k recebemdo valores aleatorios entre 1 e 6
k = rand () % 6 + 1;

    while(stop <= 6) //while 1
    {
       switch(k){
                case 1: movimentovisita(nvisitas,compatibilidade,carga,1, npilhas,ncam, nviagem, &pilha,&caminhao, &tipomov,s,capcam);
                break;
                case 2: movimentooperacaocam( nvisitas, compatibilidade, carga, passo, npilhas, ncam, nviagem, &cam, &pilha, s, capcam);
                break;
                case 3: movimentoviagemfrente( nvisitas, compatibilidade, carga,  passo,  npilhas, ncam, nviagem, &pilha, &caminhao1, &caminhao2, s, capcam);
                break;
                case 4: movimentoviagemcam( nvisitas, compatibilidade, carga,  passo,  npilhas, ncam, nviagem, &cam, &pilha1, &pilha2, s, capcam);
                break;
                case 5: movimentocarga(carga,s,npilhas,&pilha1, &pilha2, nvisitas, capcam,compatibilidade,ncam);
                break;
                case 6: movimentooperacao( carga, s, npilhas, &pilhaop,&tipomovop, nvisitas, capcam, compatibilidade, ncam);
                break;
      }
//        printf("tempo %8.2f\t %12d\n",tempo,fomin);
          focorrente = VNDvC(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
      fimVNS = clock();
      tempo = (float)(fimVNS - inicioVNS)/CLOCKS_PER_SEC ;

       if ( focorrente < fomin){


          fomin = focorrente;
          atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
          atualizamelhorsolucaoalocarga(star_carga, carga,npilhas);
          stop = 1;
                          /*
                imprimerelatorio("MelhorSolucao.rel",2,8521, tempo,s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
                /**/
       } else ++stop;
//       printf("***tempo %6.1f   focorr %8d  fomin_VNS = %8d\n",tempo, focorrente,fomin);

		k = rand () % 6 + 1;

        if(estdin ==1)
    		imprimevalordefo("foVNS_PAEC.rel", tempo, fomin, focorrente);
        else
        	imprimevalordefo("foVNS_PADC.rel", tempo, fomin, focorrente);

    //}//while 1
}
//    printf("***fomin_VNS = %8d\n",fomin);
    atualizamatrizint(nvisitas,star_nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(carga,star_carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);
    liberamatriz(star_nvisitas,npilhas);
    liberavetoralocarga(star_carga);
    imprimeRel(fomin,metodo,tempo,inst);

	






//////////////////////////////////////////////////////////////////
FILE *fptr, *fptr1;
fptr = fopen("dadostempo_3.txt", "a");
fptr1 = fopen("dadosfo_3.txt", "a");
fprintf(fptr1,"%d\n", fomin);
fprintf(fptr,"%f\n", tempo);
fclose(fptr);	
fclose(fptr1);	
/////////////////////////////////////////////////////////////////






return fomin;
}

//------------------------------------------------------------------------------
int VNSv4(float *s,float *li,float *ls,float *rec,int *penalidadesup
            ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
            ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
            ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
            ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
            ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
            , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem
            , int metodo, char *inst)
{
    int pilha1;
    int pilha2;
    int coluna1;
    int coluna2;
    int pilhaop;
    int pilha;
    int cam,caminhao,caminhao1,caminhao2;
    bool tipomov,tipomovop;
    int passo;

    int focorrente,fovizinho, fomin;
    int **star_nvisitas;
    struct alocarga *star_carga;
    float tempo;
clock_t inicioVNS, fimVNS; // marcador de tempo
        inicioVNS = clock();

    star_nvisitas = criamatriz(npilhas,ncam);
    star_carga = criavetoralocarga(npilhas);

    atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(star_carga,carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

focorrente = fovizinho = fomin = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

int k, stop = 1;


      fimVNS = clock();
      tempo = (float)(fimVNS - inicioVNS)/CLOCKS_PER_SEC ;

       if(estdin ==1)
       imprimevalordefo("foVNS_PAEC.rel", tempo, fomin, focorrente);
       else
       imprimevalordefo("foVNS_PADC.rel", tempo, fomin, focorrente);

 //while(tempo < 900.0) {//while 1


// k recebemdo valores aleatorios entre 1 e 6
k = rand () % 6 + 1;

    while(stop <= 6) //while 1
    {
       switch(k){
                case 1: movimentovisita(nvisitas,compatibilidade,carga,1, npilhas,ncam, nviagem, &pilha,&caminhao, &tipomov,s,capcam);
                break;
                case 2: movimentooperacaocam( nvisitas, compatibilidade, carga, passo, npilhas, ncam, nviagem, &cam, &pilha, s, capcam);
                break;
                case 3: movimentoviagemfrente( nvisitas, compatibilidade, carga,  passo,  npilhas, ncam, nviagem, &pilha, &caminhao1, &caminhao2, s, capcam);
                break;
                case 4: movimentoviagemcam( nvisitas, compatibilidade, carga,  passo,  npilhas, ncam, nviagem, &cam, &pilha1, &pilha2, s, capcam);
                break;
                case 5: movimentocarga(carga,s,npilhas,&pilha1, &pilha2, nvisitas, capcam,compatibilidade,ncam);
                break;
                case 6: movimentooperacao( carga, s, npilhas, &pilhaop,&tipomovop, nvisitas, capcam, compatibilidade, ncam);
                break;
      }
//        printf("tempo %8.2f\t %12d\n",tempo,fomin);
          focorrente = VNDvALE1(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
      fimVNS = clock();
      tempo = (float)(fimVNS - inicioVNS)/CLOCKS_PER_SEC ;

       if ( focorrente < fomin){


          fomin = focorrente;
          atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
          atualizamelhorsolucaoalocarga(star_carga, carga,npilhas);
          stop = 1;
                         /*
                imprimerelatorio("MelhorSolucao.rel",2,8521, tempo,s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
                /**/
       } else stop++;
//       printf("***tempo %6.1f   focorr %8d  fomin_VNS = %8d\n",tempo, focorrente,fomin);


		k = rand () % 6 + 1;

        if(estdin ==1)
    		imprimevalordefo("foVNS_PAEC.rel", tempo, fomin, focorrente);
        else
        	imprimevalordefo("foVNS_PADC.rel", tempo, fomin, focorrente);

    //}//while 1
}
//    printf("***fomin_VNS = %8d\n",fomin);
    atualizamatrizint(nvisitas,star_nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(carga,star_carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);
    liberamatriz(star_nvisitas,npilhas);
    liberavetoralocarga(star_carga);
    imprimeRel(fomin,metodo,tempo,inst);
    
    	






//////////////////////////////////////////////////////////////////
FILE *fptr, *fptr1;
fptr = fopen("dadostempo_4.txt", "a");
fptr1 = fopen("dadosfo_4.txt", "a");
fprintf(fptr1,"%d\n", fomin);
fprintf(fptr,"%f\n", tempo);
fclose(fptr);	
fclose(fptr1);	
/////////////////////////////////////////////////////////////////






    

return fomin;
}

//------------------------------------------------------------------------------
int VNSv5(float *s,float *li,float *ls,float *rec,int *penalidadesup
            ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
            ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
            ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
            ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
            ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
            , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem
            , int metodo, char *inst)
{
    int pilha1;
    int pilha2;
    int coluna1;
    int coluna2;
    int pilhaop;
    int pilha;
    int cam,caminhao,caminhao1,caminhao2;
    bool tipomov,tipomovop;
    int passo;

    int stop = 0;
    int memoria[6]={0};
    int teste=0;
    int i, j, k;

    int focorrente,fovizinho, fomin;
    int **star_nvisitas;
    struct alocarga *star_carga;
    float tempo;
clock_t inicioVNS, fimVNS; // marcador de tempo
        inicioVNS = clock();

    star_nvisitas = criamatriz(npilhas,ncam);
    star_carga = criavetoralocarga(npilhas);

    atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(star_carga,carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

focorrente = fovizinho = fomin = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);




      fimVNS = clock();
      tempo = (float)(fimVNS - inicioVNS)/CLOCKS_PER_SEC ;

       if(estdin ==1)
       imprimevalordefo("foVNS_PAEC.rel", tempo, fomin, focorrente);
       else
       imprimevalordefo("foVNS_PADC.rel", tempo, fomin, focorrente);

 //while(tempo < 900.0) {//while 1

//Alocando vetor de escolhas para o Switch
srand(time(NULL));
for(i=0;i<6;i++){

	k = rand () % 6 + 1;

	for(j=0;j<=i;j++){
		if(k == memoria[j]){
			teste=1;
			i--;
		}
	}

	if(teste==0){
		memoria[i]=k;
	}

	teste=0;

}


    while(stop < 6) //while 1
    {
       switch(memoria[stop]){
                case 1: movimentovisita(nvisitas,compatibilidade,carga,1, npilhas,ncam, nviagem, &pilha,&caminhao, &tipomov,s,capcam);
                break;
                case 2: movimentooperacaocam( nvisitas, compatibilidade, carga, passo, npilhas, ncam, nviagem, &cam, &pilha, s, capcam);
                break;
                case 3: movimentoviagemfrente( nvisitas, compatibilidade, carga,  passo,  npilhas, ncam, nviagem, &pilha, &caminhao1, &caminhao2, s, capcam);
                break;
                case 4: movimentoviagemcam( nvisitas, compatibilidade, carga,  passo,  npilhas, ncam, nviagem, &cam, &pilha1, &pilha2, s, capcam);
                break;
                case 5: movimentocarga(carga,s,npilhas,&pilha1, &pilha2, nvisitas, capcam,compatibilidade,ncam);
                break;
                case 6: movimentooperacao( carga, s, npilhas, &pilhaop,&tipomovop, nvisitas, capcam, compatibilidade, ncam);
                break;
      }
//        printf("tempo %8.2f\t %12d\n",tempo,fomin);
          focorrente = VNDvC(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
      fimVNS = clock();
      tempo = (float)(fimVNS - inicioVNS)/CLOCKS_PER_SEC ;

       if ( focorrente < fomin){


          fomin = focorrente;
          atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
          atualizamelhorsolucaoalocarga(star_carga, carga,npilhas);

                          /*
                imprimerelatorio("MelhorSolucao.rel",2,8521, tempo,s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
                /**/

       	} else ++stop;
//       printf("***tempo %6.1f   focorr %8d  fomin_VNS = %8d\n",tempo, focorrente,fomin);



        if(estdin ==1)
    		imprimevalordefo("foVNS_PAEC.rel", tempo, fomin, focorrente);
        else
        	imprimevalordefo("foVNS_PADC.rel", tempo, fomin, focorrente);

    }//while 1


//    printf("***fomin_VNS = %8d\n",fomin);
    atualizamatrizint(nvisitas,star_nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(carga,star_carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);
    liberamatriz(star_nvisitas,npilhas);
    liberavetoralocarga(star_carga);
    imprimeRel(fomin,metodo,tempo,inst);

	






//////////////////////////////////////////////////////////////////
FILE *fptr, *fptr1;
fptr = fopen("dadostempo_5.txt", "a");
fptr1 = fopen("dadosfo_5.txt", "a");
fprintf(fptr1,"%d\n", fomin);
fprintf(fptr,"%f\n", tempo);
fclose(fptr);	
fclose(fptr1);	
/////////////////////////////////////////////////////////////////







return fomin;
}

//------------------------------------------------------------------------------
int VNSv6(float *s,float *li,float *ls,float *rec,int *penalidadesup
            ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
            ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
            ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
            ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
            ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
            , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem
            , int metodo, char *inst)
{
    int pilha1;
    int pilha2;
    int coluna1;
    int coluna2;
    int pilhaop;
    int pilha;
    int cam,caminhao,caminhao1,caminhao2;
    bool tipomov,tipomovop;
    int passo;

    int stop = 0;
    int memoria[6]={0};
    int teste=0;
    int i, j, k;

    int focorrente,fovizinho, fomin;
    int **star_nvisitas;
    struct alocarga *star_carga;
    float tempo;
clock_t inicioVNS, fimVNS; // marcador de tempo
        inicioVNS = clock();

    star_nvisitas = criamatriz(npilhas,ncam);
    star_carga = criavetoralocarga(npilhas);

    atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(star_carga,carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

focorrente = fovizinho = fomin = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);




      fimVNS = clock();
      tempo = (float)(fimVNS - inicioVNS)/CLOCKS_PER_SEC ;

       if(estdin ==1)
       imprimevalordefo("foVNS_PAEC.rel", tempo, fomin, focorrente);
       else
       imprimevalordefo("foVNS_PADC.rel", tempo, fomin, focorrente);

 //while(tempo < 900.0) {//while 1

//Alocando vetor de escolhas para o Switch
srand(time(NULL));
for(i=0;i<6;i++){

	k = rand () % 6 + 1;

	for(j=0;j<=i;j++){
		if(k == memoria[j]){
			teste=1;
			i--;
		}
	}

	if(teste==0){
		memoria[i]=k;
	}

	teste=0;

}


    while(stop < 6) //while 1
    {
       switch(memoria[stop]){
                case 1: movimentovisita(nvisitas,compatibilidade,carga,1, npilhas,ncam, nviagem, &pilha,&caminhao, &tipomov,s,capcam);
                break;
                case 2: movimentooperacaocam( nvisitas, compatibilidade, carga, passo, npilhas, ncam, nviagem, &cam, &pilha, s, capcam);
                break;
                case 3: movimentoviagemfrente( nvisitas, compatibilidade, carga,  passo,  npilhas, ncam, nviagem, &pilha, &caminhao1, &caminhao2, s, capcam);
                break;
                case 4: movimentoviagemcam( nvisitas, compatibilidade, carga,  passo,  npilhas, ncam, nviagem, &cam, &pilha1, &pilha2, s, capcam);
                break;
                case 5: movimentocarga(carga,s,npilhas,&pilha1, &pilha2, nvisitas, capcam,compatibilidade,ncam);
                break;
                case 6: movimentooperacao( carga, s, npilhas, &pilhaop,&tipomovop, nvisitas, capcam, compatibilidade, ncam);
                break;
      }
//        printf("tempo %8.2f\t %12d\n",tempo,fomin);
          focorrente = VNDvALEcMem(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
      fimVNS = clock();
      tempo = (float)(fimVNS - inicioVNS)/CLOCKS_PER_SEC ;

       if ( focorrente < fomin){


          fomin = focorrente;
          atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
          atualizamelhorsolucaoalocarga(star_carga, carga,npilhas);
          //stop = 1;
                          /*
                imprimerelatorio("MelhorSolucao.rel",2,8521, tempo,s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
                /**/
    	}
       	//} else ++stop;
//       printf("***tempo %6.1f   focorr %8d  fomin_VNS = %8d\n",tempo, focorrente,fomin);



        if(estdin ==1)
    		imprimevalordefo("foVNS_PAEC.rel", tempo, fomin, focorrente);
        else
        	imprimevalordefo("foVNS_PADC.rel", tempo, fomin, focorrente);

    //}//while 1
    stop++;
}
//    printf("***fomin_VNS = %8d\n",fomin);
    atualizamatrizint(nvisitas,star_nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(carga,star_carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);
    liberamatriz(star_nvisitas,npilhas);
    liberavetoralocarga(star_carga);
    imprimeRel(fomin,metodo,tempo,inst);

	






//////////////////////////////////////////////////////////////////
FILE *fptr, *fptr1;
fptr = fopen("dadostempo_6.txt", "a");
fptr1 = fopen("dadosfo_6.txt", "a");
fprintf(fptr1,"%d\n", fomin);
fprintf(fptr,"%f\n", tempo);
fclose(fptr);	
fclose(fptr1);	
/////////////////////////////////////////////////////////////////






return fomin;
}


//------------------------------------------------------------------------------
int VNSv7(float *s,float *li,float *ls,float *rec,int *penalidadesup
            ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
            ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
            ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
            ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
            ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
            , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem
            , int metodo, char *inst)
{
    int pilha1;
    int pilha2;
    int coluna1;
    int coluna2;
    int pilhaop;
    int pilha;
    int cam,caminhao,caminhao1,caminhao2;
    bool tipomov,tipomovop;
    int passo;

    int focorrente,fovizinho, fomin;
    int **star_nvisitas;
    struct alocarga *star_carga;
    float tempo;
    clock_t inicioVNS, fimVNS; // marcador de tempo
        inicioVNS = clock();

    star_nvisitas = criamatriz(npilhas,ncam);
    star_carga = criavetoralocarga(npilhas);

    atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(star_carga,carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

    //printf("***fomin_VNS = %8d\n",fomin);

    focorrente = fovizinho = fomin = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

    //printf("***fomin_VNS = %8d\n",fomin);
    //printf("***focorrente_VNS = %8d\n",focorrente);
    int k = 1;


     fimVNS = clock();
     tempo = (float)(fimVNS - inicioVNS)/CLOCKS_PER_SEC ;

     if(estdin ==1) imprimevalordefo("foVNS_PAEC.rel", tempo, fomin, focorrente);
     else imprimevalordefo("foVNS_PADC.rel", tempo, fomin, focorrente);

      //while(tempo < 900.0) {//while 1
        k=1;
        while(k <= 6) //while 1
        {
           switch(k){
                case 1: movimentovisita(nvisitas,compatibilidade,carga,1, npilhas,ncam, nviagem, &pilha,&caminhao, &tipomov,s,capcam);
                break;
                case 2: movimentooperacaocam( nvisitas, compatibilidade, carga, passo, npilhas, ncam, nviagem, &cam, &pilha, s, capcam);
                break;
                case 3: movimentoviagemfrente( nvisitas, compatibilidade, carga,  passo,  npilhas, ncam, nviagem, &pilha, &caminhao1, &caminhao2, s, capcam);
                break;
                case 4: movimentoviagemcam( nvisitas, compatibilidade, carga,  passo,  npilhas, ncam, nviagem, &cam, &pilha1, &pilha2, s, capcam);
                break;
                case 5: movimentocarga(carga,s,npilhas,&pilha1, &pilha2, nvisitas, capcam,compatibilidade,ncam);
                break;
                case 6: movimentooperacao( carga, s, npilhas, &pilhaop,&tipomovop, nvisitas, capcam, compatibilidade, ncam);
                break;
           }
           //printf("tempo %8.2f\t %12d\n",tempo,fomin);

           focorrente = VNDvALEcMem(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);


            fimVNS = clock();
            tempo = (float)(fimVNS - inicioVNS)/CLOCKS_PER_SEC ;

            if ( focorrente < fomin){
                fomin = focorrente;
                atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
                atualizamelhorsolucaoalocarga(star_carga, carga,npilhas);
                k = 1;
                                /*
                imprimerelatorio("MelhorSolucao.rel",2,8521, tempo,s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
                /**/
            }
            else k++;
            //printf("***tempo %6.1f   focorr %8d  fomin_VNS = %8d\n",tempo, focorrente,fomin);

            if(estdin ==1) imprimevalordefo("foVNS_PAEC.rel", tempo, fomin, focorrente);
            else  imprimevalordefo("foVNS_PADC.rel", tempo, fomin, focorrente);
        //}//while 1
        //printf("***tempo %6.1f   focorr %8d  fomin_VNS = %8d\n",tempo, focorrente,fomin);
    }
    //    printf("***fomin_VNS = %8d\n",fomin);
    atualizamatrizint(nvisitas,star_nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(carga,star_carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);
    liberamatriz(star_nvisitas,npilhas);
    liberavetoralocarga(star_carga);
    imprimeRel(fomin,metodo,tempo,inst);



//////////////////////////////////////////////////////////////////
FILE *fptr, *fptr1;
fptr = fopen("dadostempo_7.txt", "a");
fptr1 = fopen("dadosfo_7.txt", "a");
fprintf(fptr1,"%d\n", fomin);
fprintf(fptr,"%f\n", tempo);
fclose(fptr);	
fclose(fptr1);	
/////////////////////////////////////////////////////////////////






    return fomin;
}


//------------------------------------------------------------------------------
int VNSv8(float *s,float *li,float *ls,float *rec,int *penalidadesup
            ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
            ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
            ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
            ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
            ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
            , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem
            , int metodo, char *inst)
{
    int pilha1;
    int pilha2;
    int coluna1;
    int coluna2;
    int pilhaop;
    int pilha;
    int cam,caminhao,caminhao1,caminhao2;
    bool tipomov,tipomovop;
    int passo;

    int focorrente,fovizinho, fomin;
    int **star_nvisitas;
    struct alocarga *star_carga;
    float tempo;
clock_t inicioVNS, fimVNS; // marcador de tempo
        inicioVNS = clock();

    star_nvisitas = criamatriz(npilhas,ncam);
    star_carga = criavetoralocarga(npilhas);

    atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(star_carga,carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

focorrente = fovizinho = fomin = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

int k, stop = 1;


      fimVNS = clock();
      tempo = (float)(fimVNS - inicioVNS)/CLOCKS_PER_SEC ;

       if(estdin ==1)
       imprimevalordefo("foVNS_PAEC.rel", tempo, fomin, focorrente);
       else
       imprimevalordefo("foVNS_PADC.rel", tempo, fomin, focorrente);

 //while(tempo < 900.0) {//while 1


// k recebemdo valores aleatorios entre 1 e 6
k = rand () % 6 + 1;

    while(stop <= 6) //while 1
    {
    	
       switch(k){
                case 1: movimentovisita(nvisitas,compatibilidade,carga,1, npilhas,ncam, nviagem, &pilha,&caminhao, &tipomov,s,capcam);
                break;
                case 2: movimentooperacaocam( nvisitas, compatibilidade, carga, passo, npilhas, ncam, nviagem, &cam, &pilha, s, capcam);
                break;
                case 3: movimentoviagemfrente( nvisitas, compatibilidade, carga,  passo,  npilhas, ncam, nviagem, &pilha, &caminhao1, &caminhao2, s, capcam);
                break;
                case 4: movimentoviagemcam( nvisitas, compatibilidade, carga,  passo,  npilhas, ncam, nviagem, &cam, &pilha1, &pilha2, s, capcam);
                break;
                case 5: movimentocarga(carga,s,npilhas,&pilha1, &pilha2, nvisitas, capcam,compatibilidade,ncam);
                break;
                case 6: movimentooperacao( carga, s, npilhas, &pilhaop,&tipomovop, nvisitas, capcam, compatibilidade, ncam);
                break;
      }
//        printf("tempo %8.2f\t %12d\n",tempo,fomin);
          focorrente = VNDvALEcMem(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
      fimVNS = clock();
      tempo = (float)(fimVNS - inicioVNS)/CLOCKS_PER_SEC ;

       if ( focorrente < fomin){


          fomin = focorrente;
          atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
          atualizamelhorsolucaoalocarga(star_carga, carga,npilhas);
          stop = 1;
                         /*
                imprimerelatorio("MelhorSolucao.rel",2,8521, tempo,s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
                /**/
       } else stop++;
//       printf("***tempo %6.1f   focorr %8d  fomin_VNS = %8d\n",tempo, focorrente,fomin);


		k = rand () % 6 + 1;

        if(estdin ==1)
    		imprimevalordefo("foVNS_PAEC.rel", tempo, fomin, focorrente);
        else
        	imprimevalordefo("foVNS_PADC.rel", tempo, fomin, focorrente);

    //}//while 1
}
//    printf("***fomin_VNS = %8d\n",fomin);
    atualizamatrizint(nvisitas,star_nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(carga,star_carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);
    liberamatriz(star_nvisitas,npilhas);
    liberavetoralocarga(star_carga);
    imprimeRel(fomin,metodo,tempo,inst);



//////////////////////////////////////////////////////////////////
FILE *fptr, *fptr1;
fptr = fopen("dadostempo_8.txt", "a");
fptr1 = fopen("dadosfo_8.txt", "a");
fprintf(fptr1,"%d\n", fomin);
fprintf(fptr,"%f\n", tempo);
fclose(fptr);	
fclose(fptr1);	
/////////////////////////////////////////////////////////////////





return fomin;
}


//------------------------------------------------------------------------------
int VNSv9(float *s,float *li,float *ls,float *rec,int *penalidadesup
            ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
            ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
            ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
            ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
            ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
            , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem
            , int metodo, char *inst)
{
    int pilha1;
    int pilha2;
    int coluna1;
    int coluna2;
    int pilhaop;
    int pilha;
    int cam,caminhao,caminhao1,caminhao2;
    bool tipomov,tipomovop;
    int passo;

    int stop = 0;
    int memoria[6]={0};
    int teste=0;
    int i, j, k;

    int focorrente,fovizinho, fomin;
    int **star_nvisitas;
    struct alocarga *star_carga;
    float tempo;
clock_t inicioVNS, fimVNS; // marcador de tempo
        inicioVNS = clock();

    star_nvisitas = criamatriz(npilhas,ncam);
    star_carga = criavetoralocarga(npilhas);

    atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(star_carga,carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

focorrente = fovizinho = fomin = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);




      fimVNS = clock();
      tempo = (float)(fimVNS - inicioVNS)/CLOCKS_PER_SEC ;

       if(estdin ==1)
       imprimevalordefo("foVNS_PAEC.rel", tempo, fomin, focorrente);
       else
       imprimevalordefo("foVNS_PADC.rel", tempo, fomin, focorrente);

 //while(tempo < 900.0) {//while 1

//Alocando vetor de escolhas para o Switch
srand(time(NULL));
for(i=0;i<6;i++){

	k = rand () % 6 + 1;

	for(j=0;j<=i;j++){
		if(k == memoria[j]){
			teste=1;
			i--;
		}
	}

	if(teste==0){
		memoria[i]=k;
	}

	teste=0;

}


    while(stop < 6) //while 1
    {
       switch(memoria[stop]){
                case 1: movimentovisita(nvisitas,compatibilidade,carga,1, npilhas,ncam, nviagem, &pilha,&caminhao, &tipomov,s,capcam);
                break;
                case 2: movimentooperacaocam( nvisitas, compatibilidade, carga, passo, npilhas, ncam, nviagem, &cam, &pilha, s, capcam);
                break;
                case 3: movimentoviagemfrente( nvisitas, compatibilidade, carga,  passo,  npilhas, ncam, nviagem, &pilha, &caminhao1, &caminhao2, s, capcam);
                break;
                case 4: movimentoviagemcam( nvisitas, compatibilidade, carga,  passo,  npilhas, ncam, nviagem, &cam, &pilha1, &pilha2, s, capcam);
                break;
                case 5: movimentocarga(carga,s,npilhas,&pilha1, &pilha2, nvisitas, capcam,compatibilidade,ncam);
                break;
                case 6: movimentooperacao( carga, s, npilhas, &pilhaop,&tipomovop, nvisitas, capcam, compatibilidade, ncam);
                break;
      }
//        printf("tempo %8.2f\t %12d\n",tempo,fomin);
          focorrente = VNDvALE1(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
      fimVNS = clock();
      tempo = (float)(fimVNS - inicioVNS)/CLOCKS_PER_SEC ;

       if ( focorrente < fomin){


          fomin = focorrente;
          atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
          atualizamelhorsolucaoalocarga(star_carga, carga,npilhas);

                          /*
                imprimerelatorio("MelhorSolucao.rel",2,8521, tempo,s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
                /**/

       	} else ++stop;
//       printf("***tempo %6.1f   focorr %8d  fomin_VNS = %8d\n",tempo, focorrente,fomin);



        if(estdin ==1)
    		imprimevalordefo("foVNS_PAEC.rel", tempo, fomin, focorrente);
        else
        	imprimevalordefo("foVNS_PADC.rel", tempo, fomin, focorrente);

    }//while 1


//    printf("***fomin_VNS = %8d\n",fomin);
    atualizamatrizint(nvisitas,star_nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(carga,star_carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);
    liberamatriz(star_nvisitas,npilhas);
    liberavetoralocarga(star_carga);
    imprimeRel(fomin,metodo,tempo,inst);




//////////////////////////////////////////////////////////////////
FILE *fptr, *fptr1;
fptr = fopen("dadostempo_9.txt", "a");
fptr1 = fopen("dadosfo_9.txt", "a");
fprintf(fptr1,"%d\n", fomin);
fprintf(fptr,"%f\n", tempo);
fclose(fptr);	
fclose(fptr1);	
/////////////////////////////////////////////////////////////////





return fomin;
}


//------------------------------------------------------------------------------
int VNSv10(float *s,float *li,float *ls,float *rec,int *penalidadesup
            ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
            ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
            ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
            ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
            ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
            , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem
            , int metodo, char *inst)
{
    int pilha1;
    int pilha2;
    int coluna1;
    int coluna2;
    int pilhaop;
    int pilha;
    int cam,caminhao,caminhao1,caminhao2;
    bool tipomov,tipomovop;
    int passo;


    int i, j, k=0, iter=1;
    int k_atual=0;
    int MAX=60;
    int probabilidade[500]={0};
    int chance[7]={0};
    int melhora[7]={0};
    int porcentagem[7]={10,10,10,10,10,10,10};


    int focorrente,fovizinho, fomin;
    int **star_nvisitas;
    struct alocarga *star_carga;
    float tempo;
    clock_t inicioVNS, fimVNS; // marcador de tempo
        inicioVNS = clock();

    star_nvisitas = criamatriz(npilhas,ncam);
    star_carga = criavetoralocarga(npilhas);

    atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(star_carga,carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);



    focorrente = fovizinho = fomin = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);




     fimVNS = clock();
     tempo = (float)(fimVNS - inicioVNS)/CLOCKS_PER_SEC ;

     if(estdin ==1) imprimevalordefo("foVNS_PAEC.rel", tempo, fomin, focorrente);
     else imprimevalordefo("foVNS_PADC.rel", tempo, fomin, focorrente);


	// Vetor probabilidade recebemdo a mesma quantidade de cada escolha possivel para vizinhanÁa
    //Cada escolha tem 10 espaÁos no vetor probabilidade, assim, foi adotado 10 como 100% de chance
    j=1;	
	for(i=1;i<=60;i++){
		probabilidade[i]=j;
		//printf("Escolha:%d\tProb: %d\n", i,j);
		if(i%10==0)j++;		
	}
	j=1;

    while(tempo < 10.0) {//while 1
	    //while(k <= 6) //while 1
	        
		k = rand () % MAX + 1;
	        
	    switch(probabilidade[k]){
	       			
	        case 1: movimentovisita(nvisitas,compatibilidade,carga,1, npilhas,ncam, nviagem, &pilha,&caminhao, &tipomov,s,capcam);
	        break;
	        case 2: movimentooperacaocam( nvisitas, compatibilidade, carga, passo, npilhas, ncam, nviagem, &cam, &pilha, s, capcam);
	        break;
	        case 3: movimentoviagemfrente( nvisitas, compatibilidade, carga,  passo,  npilhas, ncam, nviagem, &pilha, &caminhao1, &caminhao2, s, capcam);
	        break;
	        case 4: movimentoviagemcam( nvisitas, compatibilidade, carga,  passo,  npilhas, ncam, nviagem, &cam, &pilha1, &pilha2, s, capcam);
	        break;
	        case 5: movimentocarga(carga,s,npilhas,&pilha1, &pilha2, nvisitas, capcam,compatibilidade,ncam);
	        break;
	        case 6: movimentooperacao( carga, s, npilhas, &pilhaop,&tipomovop, nvisitas, capcam, compatibilidade, ncam);
	    	break;
	   	}
	
	   	focorrente = VNDvC(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
	                , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
	                , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
	                , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
	                , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
	                , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
	
	
	    fimVNS = clock();
	    tempo = (float)(fimVNS - inicioVNS)/CLOCKS_PER_SEC ;
	    
	    chance[probabilidade[k]]++;
	
	    if ( focorrente < fomin){
	        fomin = focorrente;
	        atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
	        atualizamelhorsolucaoalocarga(star_carga, carga,npilhas);
	        k_atual = 1;
	        melhora[probabilidade[k]]++;
	         
	    }
	    else k_atual++;
	    
		iter++;
	    
	    // A cada 10 iteraÁıes sem melhora o vetor probabilidade È restaurado
	    if(k_atual = 10){
	    	
	    	for(i=1;i<=60;i++){
				probabilidade[i]=j;
				//printf("Escolha:%d\tProb: %d\n", i,j);
			if(i%10==0)j++;		
			}
			j=1;
	    	MAX=60;
	    	k_atual = 0;
		}else{
			if(iter = 30){ // A cada 30 iteraÁıes antes de completar 10 iteraÁıes sem sucesso o vetor probabilidade È atualizado
			
				// Calculo da probabilidade de cada numero
		   		// Arredondamento realizado ( de 0,50 atÈ 0,99 arredonda para cima, de 0,49 atÈ 0,01 para baixo arredonda para baixo
			    for(i=1;i<=6;i++){
					porcentagem[i]= porcentagem[i] + 0.5 +(10.00*melhora[i]/chance[i]);
					melhora[i]=0;
					chance[i]=0;
				}
				
			    // Preenchendo o vetor probabilidade de acordo com a porcentagem calculada
			    MAX=0;
				for(i=1;i<=6;i++){
						
					MAX += porcentagem[i];
					
					for(j=MAX-porcentagem[i]+1;j<=MAX;j++){
						probabilidade[j]=i;			
					}
				
					//printf("escolha: %d melhoras: %d chances: %d probabilidade: %d\n", i, melhora[i], chance[i], porcentagem[i]);
				}
				iter=0;
				
			}
			
		}
	    
	    if(estdin ==1) imprimevalordefo("foVNS_PAEC.rel", tempo, fomin, focorrente);
	    else  imprimevalordefo("foVNS_PADC.rel", tempo, fomin, focorrente);
	    
	    
    } // Fim while 1

    atualizamatrizint(nvisitas,star_nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(carga,star_carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);
    liberamatriz(star_nvisitas,npilhas);
    liberavetoralocarga(star_carga);
    imprimeRel(fomin,metodo,tempo,inst);






//////////////////////////////////////////////////////////////////
FILE *fptr, *fptr1;
fptr = fopen("dadostempo_10.txt", "a");
fptr1 = fopen("dadosfo_10.txt", "a");
fprintf(fptr1,"%d\n", fomin);
fprintf(fptr,"%f\n", tempo);
fclose(fptr);	
fclose(fptr1);	
/////////////////////////////////////////////////////////////////





    return fomin;
}


//------------------------------------------------------------------------------
int VNSv11(float *s,float *li,float *ls,float *rec,int *penalidadesup
            ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
            ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
            ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
            ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
            ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
            , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem
            , int metodo, char *inst)
{
    int pilha1;
    int pilha2;
    int coluna1;
    int coluna2;
    int pilhaop;
    int pilha;
    int cam,caminhao,caminhao1,caminhao2;
    bool tipomov,tipomovop;
    int passo;


    int i, j, k=0, iter=1;
    int k_atual=0;
    int MAX=60;
    int probabilidade[500]={0};
    int chance[7]={0};
    int melhora[7]={0};
    int porcentagem[7]={10,10,10,10,10,10,10};


    int focorrente,fovizinho, fomin;
    int **star_nvisitas;
    struct alocarga *star_carga;
    float tempo;
    clock_t inicioVNS, fimVNS; // marcador de tempo
        inicioVNS = clock();

    star_nvisitas = criamatriz(npilhas,ncam);
    star_carga = criavetoralocarga(npilhas);

    atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(star_carga,carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);



    focorrente = fovizinho = fomin = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);




     fimVNS = clock();
     tempo = (float)(fimVNS - inicioVNS)/CLOCKS_PER_SEC ;

     if(estdin ==1) imprimevalordefo("foVNS_PAEC.rel", tempo, fomin, focorrente);
     else imprimevalordefo("foVNS_PADC.rel", tempo, fomin, focorrente);


	// Vetor probabilidade recebemdo a mesma quantidade de cada escolha possivel para vizinhanÁa
    //Cada escolha tem 10 espaÁos no vetor probabilidade, assim, foi adotado 10 como 100% de chance
    j=1;	
	for(i=1;i<=60;i++){
		probabilidade[i]=j;
		//printf("Escolha:%d\tProb: %d\n", i,j);
		if(i%10==0)j++;		
	}
	j=1;

    while(tempo < 10.0) {//while 1
	    //while(k <= 6) //while 1
	        
		k = rand () % MAX + 1;
	        
	    switch(probabilidade[k]){
	       			
	        case 1: movimentovisita(nvisitas,compatibilidade,carga,1, npilhas,ncam, nviagem, &pilha,&caminhao, &tipomov,s,capcam);
	        break;
	        case 2: movimentooperacaocam( nvisitas, compatibilidade, carga, passo, npilhas, ncam, nviagem, &cam, &pilha, s, capcam);
	        break;
	        case 3: movimentoviagemfrente( nvisitas, compatibilidade, carga,  passo,  npilhas, ncam, nviagem, &pilha, &caminhao1, &caminhao2, s, capcam);
	        break;
	        case 4: movimentoviagemcam( nvisitas, compatibilidade, carga,  passo,  npilhas, ncam, nviagem, &cam, &pilha1, &pilha2, s, capcam);
	        break;
	        case 5: movimentocarga(carga,s,npilhas,&pilha1, &pilha2, nvisitas, capcam,compatibilidade,ncam);
	        break;
	        case 6: movimentooperacao( carga, s, npilhas, &pilhaop,&tipomovop, nvisitas, capcam, compatibilidade, ncam);
	    	break;
	   	}
	
	   	focorrente = VNDvALE1(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
	                , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
	                , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
	                , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
	                , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
	                , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
	
	
	    fimVNS = clock();
	    tempo = (float)(fimVNS - inicioVNS)/CLOCKS_PER_SEC ;
	    
	    chance[probabilidade[k]]++;
	
	    if ( focorrente < fomin){
	        fomin = focorrente;
	        atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
	        atualizamelhorsolucaoalocarga(star_carga, carga,npilhas);
	        k_atual = 1;
	        melhora[probabilidade[k]]++;
	         
	    }
	    else k_atual++;
	    
		iter++;
	    
	    // A cada 10 iteraÁıes sem melhora o vetor probabilidade È restaurado
	    if(k_atual = 10){
	    	
	    	for(i=1;i<=60;i++){
				probabilidade[i]=j;
				//printf("Escolha:%d\tProb: %d\n", i,j);
			if(i%10==0)j++;		
			}
			j=1;
	    	MAX=60;
	    	k_atual = 0;
		}else{
			if(iter = 30){ // A cada 30 iteraÁıes antes de completar 10 iteraÁıes sem sucesso o vetor probabilidade È atualizado
			
				// Calculo da probabilidade de cada numero
		   		// Arredondamento realizado ( de 0,50 atÈ 0,99 arredonda para cima, de 0,49 atÈ 0,01 para baixo arredonda para baixo
			    for(i=1;i<=6;i++){
					porcentagem[i]= porcentagem[i] + 0.5 +(10.00*melhora[i]/chance[i]);
					melhora[i]=0;
					chance[i]=0;
				}
				
			    // Preenchendo o vetor probabilidade de acordo com a porcentagem calculada
			    MAX=0;
				for(i=1;i<=6;i++){
						
					MAX += porcentagem[i];
					
					for(j=MAX-porcentagem[i]+1;j<=MAX;j++){
						probabilidade[j]=i;			
					}
				
					//printf("escolha: %d melhoras: %d chances: %d probabilidade: %d\n", i, melhora[i], chance[i], porcentagem[i]);
				}
				iter=0;
				
			}
			
		}
	    
	    if(estdin ==1) imprimevalordefo("foVNS_PAEC.rel", tempo, fomin, focorrente);
	    else  imprimevalordefo("foVNS_PADC.rel", tempo, fomin, focorrente);
	    
	    
    } // Fim while 1

    atualizamatrizint(nvisitas,star_nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(carga,star_carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);
    liberamatriz(star_nvisitas,npilhas);
    liberavetoralocarga(star_carga);
    imprimeRel(fomin,metodo,tempo,inst);







//////////////////////////////////////////////////////////////////
FILE *fptr, *fptr1;
fptr = fopen("dadostempo_11.txt", "a");
fptr1 = fopen("dadosfo_11.txt", "a");
fprintf(fptr1,"%d\n", fomin);
fprintf(fptr,"%f\n", tempo);
fclose(fptr);	
fclose(fptr1);	
/////////////////////////////////////////////////////////////////





    return fomin;
}


//------------------------------------------------------------------------------
int VNSv12(float *s,float *li,float *ls,float *rec,int *penalidadesup
            ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
            ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
            ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
            ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
            ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
            , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem
            , int metodo, char *inst)
{
    int pilha1;
    int pilha2;
    int coluna1;
    int coluna2;
    int pilhaop;
    int pilha;
    int cam,caminhao,caminhao1,caminhao2;
    bool tipomov,tipomovop;
    int passo;


    int i, j, k=0, iter=1;
    int k_atual=0;
    int MAX=60;
    int probabilidade[500]={0};
    int chance[7]={0};
    int melhora[7]={0};
    int porcentagem[7]={10,10,10,10,10,10,10};


    int focorrente,fovizinho, fomin;
    int **star_nvisitas;
    struct alocarga *star_carga;
    float tempo;
    clock_t inicioVNS, fimVNS; // marcador de tempo
        inicioVNS = clock();

    star_nvisitas = criamatriz(npilhas,ncam);
    star_carga = criavetoralocarga(npilhas);

    atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(star_carga,carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);



    focorrente = fovizinho = fomin = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);




     fimVNS = clock();
     tempo = (float)(fimVNS - inicioVNS)/CLOCKS_PER_SEC ;

     if(estdin ==1) imprimevalordefo("foVNS_PAEC.rel", tempo, fomin, focorrente);
     else imprimevalordefo("foVNS_PADC.rel", tempo, fomin, focorrente);


	// Vetor probabilidade recebemdo a mesma quantidade de cada escolha possivel para vizinhanÁa
    //Cada escolha tem 10 espaÁos no vetor probabilidade, assim, foi adotado 10 como 100% de chance
    j=1;	
	for(i=1;i<=60;i++){
		probabilidade[i]=j;
		//printf("Escolha:%d\tProb: %d\n", i,j);
		if(i%10==0)j++;		
	}
	j=1;

    while(tempo < 10.0) {//while 1
	    //while(k <= 6) //while 1

	    //printf("Tempo: %.2f\n", tempo);    
	    
		k = rand () % MAX + 1;
	        
	    switch(probabilidade[k]){
	       			
	        case 1: movimentovisita(nvisitas,compatibilidade,carga,1, npilhas,ncam, nviagem, &pilha,&caminhao, &tipomov,s,capcam);
	        break;
	        case 2: movimentooperacaocam( nvisitas, compatibilidade, carga, passo, npilhas, ncam, nviagem, &cam, &pilha, s, capcam);
	        break;
	        case 3: movimentoviagemfrente( nvisitas, compatibilidade, carga,  passo,  npilhas, ncam, nviagem, &pilha, &caminhao1, &caminhao2, s, capcam);
	        break;
	        case 4: movimentoviagemcam( nvisitas, compatibilidade, carga,  passo,  npilhas, ncam, nviagem, &cam, &pilha1, &pilha2, s, capcam);
	        break;
	        case 5: movimentocarga(carga,s,npilhas,&pilha1, &pilha2, nvisitas, capcam,compatibilidade,ncam);
	        break;
	        case 6: movimentooperacao( carga, s, npilhas, &pilhaop,&tipomovop, nvisitas, capcam, compatibilidade, ncam);
	    	break;
	   	}
	
	   	focorrente = VNDvALEcMem(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
	                , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
	                , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
	                , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
	                , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
	                , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
	
	
	    fimVNS = clock();
	    tempo = (float)(fimVNS - inicioVNS)/CLOCKS_PER_SEC ;
	    
	    chance[probabilidade[k]]++;
	
	    if ( focorrente < fomin){
	        fomin = focorrente;
	        atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
	        atualizamelhorsolucaoalocarga(star_carga, carga,npilhas);
	        k_atual = 1;
	        melhora[probabilidade[k]]++;
	         
	    }
	    else k_atual++;
	    
		iter++;
	    
	    // A cada 10 iteraÁıes sem melhora o vetor probabilidade È restaurado
	    if(k_atual = 10){
	    	
	    	for(i=1;i<=60;i++){
				probabilidade[i]=j;
				//printf("Escolha:%d\tProb: %d\n", i,j);
			if(i%10==0)j++;		
			}
			j=1;
	    	MAX=60;
	    	k_atual = 0;
		}else{
			if(iter = 30){ // A cada 30 iteraÁıes antes de completar 10 iteraÁıes sem sucesso o vetor probabilidade È atualizado
			
				// Calculo da probabilidade de cada numero
		   		// Arredondamento realizado ( de 0,50 atÈ 0,99 arredonda para cima, de 0,49 atÈ 0,01 para baixo arredonda para baixo
			    for(i=1;i<=6;i++){
					porcentagem[i]= porcentagem[i] + 0.5 +(10.00*melhora[i]/chance[i]);
					melhora[i]=0;
					chance[i]=0;
				}
				
			    // Preenchendo o vetor probabilidade de acordo com a porcentagem calculada
			    MAX=0;
				for(i=1;i<=6;i++){
						
					MAX += porcentagem[i];
					
					for(j=MAX-porcentagem[i]+1;j<=MAX;j++){
						probabilidade[j]=i;			
					}
				
					//printf("escolha: %d melhoras: %d chances: %d probabilidade: %d\n", i, melhora[i], chance[i], porcentagem[i]);
				}
				iter=0;
				
			}
			
		}
	    
	    if(estdin ==1) imprimevalordefo("foVNS_PAEC.rel", tempo, fomin, focorrente);
	    else  imprimevalordefo("foVNS_PADC.rel", tempo, fomin, focorrente);
	    
	    
    } // Fim while 1

    atualizamatrizint(nvisitas,star_nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(carga,star_carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);
    liberamatriz(star_nvisitas,npilhas);
    liberavetoralocarga(star_carga);
    imprimeRel(fomin,metodo,tempo,inst);







//////////////////////////////////////////////////////////////////
FILE *fptr, *fptr1;
fptr = fopen("dadostempo_12.txt", "a");
fptr1 = fopen("dadosfo_12.txt", "a");
fprintf(fptr1,"%d\n", fomin);
fprintf(fptr,"%f\n", tempo);
fclose(fptr);	
fclose(fptr1);	
/////////////////////////////////////////////////////////////////





    return fomin;
}

///
