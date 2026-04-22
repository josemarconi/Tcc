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

int VNDvC(float *s,float *li,float *ls,float *rec,int *penalidadesup
            ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
            ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
            ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
            ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
            ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
            , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem)
{
srand(time(NULL));
int focorrente,fovizinho;
    int **star_nvisitas;
    struct alocarga *star_carga;

    star_nvisitas = criamatriz(npilhas,ncam);
    star_carga = criavetoralocarga(npilhas);

    atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(star_carga,carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

focorrente = fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

int k = 1;

    while(k <= 6) //while 1
    {
       switch(k){
                case 1: fovizinho = melhorvizinhonvisitas(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
                break;
                case 2: fovizinho = melhorvizinhooperacaocam(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
                break;
                case 3: fovizinho = melhorvizinhonviagemcam(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
                break;
                case 4: fovizinho = melhorvizinhonviagemfrente(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
                break;
                case 5: fovizinho = melhorvizinhocarga(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
                break;
                case 6: fovizinho = melhorvizinhooperacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
                break;




          }
       if (fovizinho < focorrente){

          focorrente = fovizinho;
          atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
          atualizamelhorsolucaoalocarga(star_carga, carga,npilhas);
          k = 1;
 //         printf("***fomin_VND = %8d\n",focorrente);
       }
       else k++;
}//while 1

    atualizamatrizint(nvisitas,star_nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(carga,star_carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);
    liberamatriz(star_nvisitas,npilhas);
    liberavetoralocarga(star_carga);
return focorrente;
}



//------------------------------------------------------------------------------
int VNDvALE1(float *s,float *li,float *ls,float *rec,int *penalidadesup
            ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
            ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
            ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
            ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
            ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
            , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem)
{
srand(time(NULL));
int focorrente,fovizinho;
    int **star_nvisitas;
    struct alocarga *star_carga;

    star_nvisitas = criamatriz(npilhas,ncam);
    star_carga = criavetoralocarga(npilhas);

    atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(star_carga,carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

    focorrente = fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

int k, stop = 1;

// k recebemdo valores aleatorios entre 1 e 6
k = rand () % 6 + 1;

    while(stop <= 6) //while 1
    {
       switch(k){
                case 1: fovizinho = melhorvizinhonvisitas(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
                break;
                case 2: fovizinho = melhorvizinhooperacaocam(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
                break;
                case 3: fovizinho = melhorvizinhonviagemcam(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
                break;
                case 4: fovizinho = melhorvizinhonviagemfrente(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
                break;
                case 5: fovizinho = melhorvizinhocarga(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
                break;
                case 6: fovizinho = melhorvizinhooperacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
                break;

          }
       if (fovizinho < focorrente){

          focorrente = fovizinho;
          atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
          atualizamelhorsolucaoalocarga(star_carga, carga,npilhas);
          stop = 1;
          
       }
       else stop++;

       k = rand () % 6 + 1;

}//while 1

    atualizamatrizint(nvisitas,star_nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(carga,star_carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);
    liberamatriz(star_nvisitas,npilhas);
    liberavetoralocarga(star_carga);
return focorrente;
}


//------------------------------------------------------------------------------

int VNDvALEcMem(float *s,float *li,float *ls,float *rec,int *penalidadesup
            ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
            ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
            ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
            ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
            ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
            , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem)
{
    srand(time(NULL));
    int focorrente,fovizinho;
    int **star_nvisitas;
    struct alocarga *star_carga;
    int i, j, k;
    int stop = 0;
    int memoria[6]={0};
	int teste = 0;

    star_nvisitas = criamatriz(npilhas,ncam);
    star_carga = criavetoralocarga(npilhas);

    atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(star_carga,carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

    focorrente = fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);



    //Alocando vetor de escolhas para o Switch
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

    //for(i = 0; i < 6; ++i) printf("memo[%d] = %d \n",i,memoria[i]); getchar();
    stop = 0;
    while(stop < 6) //while 1
    {
       switch(memoria[stop]){
           case 1: fovizinho = melhorvizinhonvisitas(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
           break;
           case 2: fovizinho = melhorvizinhooperacaocam(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
           break;
           case 3: fovizinho = melhorvizinhonviagemcam(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
           break;
           case 4: fovizinho = melhorvizinhonviagemfrente(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
           break;
           case 5: fovizinho = melhorvizinhocarga(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
           break;
           case 6: fovizinho = melhorvizinhooperacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
           break;

       }

       if (fovizinho < focorrente){

            focorrente = fovizinho;
            atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
            atualizamelhorsolucaoalocarga(star_carga, carga,npilhas);
            
       } else stop++;

    }//while 1

    atualizamatrizint(nvisitas,star_nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(carga,star_carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);
    liberamatriz(star_nvisitas,npilhas);
    liberavetoralocarga(star_carga);

    return focorrente;
}




int VNDvAdaptativo(float *s,float *li,float *ls,float *rec,int *penalidadesup
            ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
            ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
            ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
            ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
            ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
            , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem)
{
    int focorrente,fovizinho;
    int **star_nvisitas;
    struct alocarga *star_carga;
    int i, j, k=0;
    int MAX=0;
    int stop = 0;
    int probabilidade[61]={0};
    int chance[7]={0};
    int melhora[7]={0};
    int porcentagem[7]={0};
    
	srand(time(NULL));
    star_nvisitas = criamatriz(npilhas,ncam);
    star_carga = criavetoralocarga(npilhas);

    atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(star_carga,carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

    focorrente = fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

    // Vetor probabilidade recebemdo a mesma quantidade de cada escolha possivel para vizinhança
    //Cada escolha tem 10 espaços no vetor probabilidade, assim, foi adotado 10 como 100% de chance
    j=1;	
	for(i=1;i<=60;i++){
		probabilidade[i]=j;
		//printf("Escolha:%d\tProb: %d\n", i,j);
		if(i%10==0)j++;		
	}
	
    while(stop < 30) //while 1
    {
    	k = rand () % 60 + 1;
		
       	switch(probabilidade[k]){
           	case 1: fovizinho = melhorvizinhonvisitas(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
           break;
           case 2: fovizinho = melhorvizinhooperacaocam(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
           break;
           case 3: fovizinho = melhorvizinhonviagemcam(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
           break;
           case 4: fovizinho = melhorvizinhonviagemfrente(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
           break;
           case 5: fovizinho = melhorvizinhocarga(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
           break;
           case 6: fovizinho = melhorvizinhooperacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
           break;

       	}

		chance[probabilidade[k]]++;
		
       	if (fovizinho < focorrente){
			
			melhora[probabilidade[k]]++;
            focorrente = fovizinho;
            atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
            atualizamelhorsolucaoalocarga(star_carga, carga,npilhas);

		}else	stop++;
		
		
    }//while 1
    
    // Calculo da probabilidade de cada numero
    // Arredondamento realizado ( de 0,50 até 0,99 arredonda para cima, de 0,49 até 0,01 para baixo arredonda para baixo
    for(i=1;i<=6;i++){
		porcentagem[i]=((10.00*melhora[i]/chance[i])+0.5);
	}
	
	// Preenchendo o vetor probabilidade de acordo com a porcentagem calculada
	for(i=1;i<=6;i++){
				
		MAX += porcentagem[i];
		
		for(j=MAX-porcentagem[i]+1;j<=MAX;j++){
			probabilidade[j]=i;			
		}
		
		//printf("escolha: %d melhoras: %d chances: %d probabilidade: %d\n", i, melhora[i], chance[i], porcentagem[i]);
	}
	

	
	// Escolha de movimento utilizando a probabilidade calculada -------- Sugestão: Escolhada pode ser rodada em um For para realizar mais alguns movimentos utilizando probabilidade calculada	
	k = rand () % MAX + 1;
	//printf("\n\n\nK prob.: %d  Tam. Vet: %d\n", probabilidade[k],MAX);
	
	switch(probabilidade[k]){
		case 1: fovizinho = melhorvizinhonvisitas(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
    	break;
        case 2: fovizinho = melhorvizinhooperacaocam(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
       	break;
       	case 3: fovizinho = melhorvizinhonviagemcam(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
	  	break;
       	case 4: fovizinho = melhorvizinhonviagemfrente(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
        break;
        case 5: fovizinho = melhorvizinhocarga(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
        break;
        case 6: fovizinho = melhorvizinhooperacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
        break;

    }       
    if (fovizinho < focorrente){
			
        focorrente = fovizinho;
    	atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
    	atualizamelhorsolucaoalocarga(star_carga, carga,npilhas);

	}
	
    atualizamatrizint(nvisitas,star_nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(carga,star_carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);
    liberamatriz(star_nvisitas,npilhas);
    liberavetoralocarga(star_carga);

    return focorrente;
}
