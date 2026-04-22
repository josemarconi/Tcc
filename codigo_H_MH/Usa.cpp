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
#include "hstruct.h"
//----------------------------------------------------------------------
int SA(float *s,float *li,float *ls,float *rec,int *penalidadesup
            ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
            ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
            ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
            ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
            ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
            , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem, int nviagemmax, int visitamax)
{
    double temperatura;
    double alfa;
    double aleatorio;
    int pilha1;
    int pilha2;
    int coluna1;
    int coluna2;
    int pilhaop;
    int pilha;
    int caminhao;
    bool tipomov,tipomovop;
    int iter, SAmax;
    int focorrente, fovizinho, fomin, delta;
    int **star_nvisitas;
    struct alocarga *star_carga;
    float escolha;
    int movimento;
    int contadorcorrente=0;

 //  printf("Aplicando SA na alocacao dos equipamentos de carga...\n");

    star_nvisitas = criamatriz(npilhas,ncam);
    star_carga = criavetoralocarga(npilhas);

    atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(star_carga,carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);


    focorrente = fomin = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

    /****************/

    float k = 1;
    alfa = 0.97;
    SAmax = (int)(k*nviagemmax*visitamax);
    temperatura = 857000;
    while ( temperatura > 0.01 ){

    	iter = 0;
while (iter < SAmax ){
        	iter++;
       //         printf(". %2d\n",iter);
        escolha = randomico(0,1);


        if(escolha < 0.90){
                movimentovisita(nvisitas,compatibilidade,carga,1, npilhas,ncam, nviagem, &pilha,&caminhao, &tipomov,s,capcam);
                movimento = 0;
        }
        if(escolha >= 0.90 && escolha < 0.92){
                movimentooperacao( carga, s, npilhas, &pilhaop,&tipomovop, nvisitas, capcam, compatibilidade, ncam);
                movimento = 2;
               // printf("pilhaop %2d operando %2d\n",pilhaop,carga[pilhaop].op);
               // for(int i=0;i<npilhas;i++)
               //         printf("Ates SA carga %2d op %2d  s[%2d] = %8.2f    -> carga maxima %8.2f\n",carga[i].equip,carga[i].op,i,s[i],cargamax[carga[i].equip]);
               //getchar();

        }

        if(escolha >=0.92){
                movimentocarga(carga,s,npilhas,&pilha1, &pilha2, nvisitas, capcam,compatibilidade,ncam);
                movimento=1;
        }

        fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);




/*

                if (fovizinho == focorrente){
                        contadorcorrente++;
                        printf("contador corrente %4d\n",contadorcorrente);
                        contador =0;
                }
                if (fomin == 708505){
                contador++;
                printf("contador 2 %4d\n",contador);
                getchar();
                }
                else
                contador =0;

*/




           	if (fovizinho < focorrente){
               	        focorrente = fovizinho;
                        contadorcorrente = 0;

                }
                if (fovizinho < fomin) {
                        fomin = fovizinho;
			atualizamatrizint(star_nvisitas, nvisitas, npilhas,ncam);
                        atualizamelhorsolucaoalocarga(star_carga,carga,npilhas);
                        carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);
                        printf("***fomin_SA = %12d\n",fomin);
                }
                else{
                        delta =  fovizinho  - focorrente;
             	        aleatorio = randomico(0,1);
                        if (aleatorio < exp((-1)*(double) delta/temperatura)){
		                   focorrente = fovizinho;
                        }
                        else{
                                if(movimento==0){
                                        if (tipomov == false)
                                                nvisitas[pilha][caminhao]--;
                                        else{
                                                nvisitas[pilha][caminhao]++;
                                        }

                                }
                                if(movimento==1){
                                        trocar(carga,s,pilha2, pilha1, nvisitas, capcam, compatibilidade, ncam);

                                }
                                if(movimento==2){
                                        trocaroperacao( carga, s, pilhaop, nvisitas, capcam, compatibilidade, ncam, npilhas);

                                }
                                carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

                        }

                }
        } /* SAmax */
        temperatura *= alfa;
} /* temperatura */

    atualizamatrizint(nvisitas,star_nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(carga,star_carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);
    liberamatriz(star_nvisitas,npilhas);
    liberavetoralocarga(star_carga);

 return fomin;
}
//----------------------------------------------------------------------




float temperatura(float *s,float *li,float *ls,float *rec,int *penalidadesup
            ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
            ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
            ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
            ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
            ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
            ,int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem, int nviagemmax, int visitamax)
{
    double temperatura;
    double alfa;
    double aleatorio;
    int pilha1;
    int pilha2;
    int coluna1;
    int coluna2;
    int pilha;
    int caminhao;
    bool tipomov;
    int iter, SAmax;
    int focorrente, fovizinho, fomin, delta;
    float escolha;
    int movimento;


    focorrente = fomin = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

    /****************/
    int contador=0;
    float k = 1;
    alfa = 0.99;
    SAmax = (int)(k*nviagemmax*visitamax);
    temperatura = 100;
    float percent=0;
    float prob=0.0;
    while ( prob < 0.95 ){
        contador=0;
    	iter = 0;
        
    while (iter < SAmax ){
        	iter++;
       //         printf(". %2d\n",iter);
        escolha = randomico(0,1);
        if(escolha < 0.33){
                movimentovisita(nvisitas,compatibilidade,carga,1, npilhas,ncam, nviagem, &pilha,&caminhao, &tipomov,s,capcam);
                movimento = 0;
        }
        if(escolha >=0.33 && escolha < 0.66){
                movimentooperacao( carga, s, npilhas, &pilha1,&tipomov, nvisitas, capcam, compatibilidade, ncam);
                movimento = 2;
        }

        if(escolha >=0.66){
                movimentocarga(carga,s,npilhas,&pilha1, &pilha2, nvisitas, capcam,compatibilidade,ncam);
                movimento=1;
        }

        fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

           	if (fovizinho < focorrente){

                        contador++;
                }
                else{
                        delta =  fovizinho  - focorrente;
             	        aleatorio = randomico(0,1);
                        if (aleatorio < exp((-1)*(double) delta/temperatura)){

                                contador++;
                        }
                }
                if(movimento==0){
                        if (tipomov == false)
                                nvisitas[pilha][caminhao]--;
                        else
                                nvisitas[pilha][caminhao]++;
                }
                if(movimento==1){
                        trocar(carga,s,pilha2, pilha1, nvisitas, capcam, compatibilidade, ncam);
                }
                if(movimento==2){
                        if (carga[pilha1].op == true)
                                carga[pilha1].op = false;
                        else
                                carga[pilha1].op = true;
                }
                carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

        } /* SAmax */
       prob=(float)contador/SAmax;
       if(prob < 0.95)
            temperatura *= 1.1;
} /* temperatura */


 return temperatura;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
int SA_2(float *s,float *li,float *ls,float *rec,int *penalidadesup
            ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
            ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
            ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
            ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
            ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
            , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem, int nviagemmax, int visitamax)
{
    double temperatura;
    double alfa;
    double aleatorio;
    int pilha1;
    int pilha2;
    int coluna1;
    int coluna2;
    int pilhaop;
    int pilha;
    int caminhao;
    bool tipomov,tipomovop,ok;
    int iter, SAmax;
    int focorrente, fovizinho, fomin, delta;
    int focarga,fooperacao,fonvisitas;
    int **star_nvisitas;
    struct alocarga *star_carga;
    int movimento;
    int contador,contop,contcarga,contnvisitas;
    int contadorcorrente=0;
    contador = contop = contcarga = contnvisitas = 0;
 //  printf("Aplicando SA na alocacao dos equipamentos de carga...\n");

    star_nvisitas = criamatriz(npilhas,ncam);
    star_carga = criavetoralocarga(npilhas);

    atualizamatrizint(star_nvisitas,nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(star_carga,carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);


    focarga = fooperacao = fonvisitas = focorrente = fomin = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

    /****************/

    float k = 1;
    alfa = 0.97;
    SAmax = (int)(k*nviagemmax*visitamax);
    temperatura = 857000;
    while ( temperatura > 0.01 ){

    	iter = 0;
while (iter < SAmax ){
        	iter++;
       //         printf(". %2d\n",iter);


//------------------------------------------------------------------------------
                movimentovisita(nvisitas,compatibilidade,carga,1, npilhas,ncam, nviagem, &pilha,&caminhao, &tipomov,s,capcam);
                fonvisitas = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
               if(tipomov == true)
                        trocarnvisitas(&ok, nvisitas, nviagem, 1, false, pilha, caminhao);
               else
                        trocarnvisitas(&ok, nvisitas, nviagem, 1, true, pilha, caminhao);



                movimentooperacao( carga, s, npilhas, &pilhaop,&tipomovop, nvisitas, capcam, compatibilidade, ncam);
                fooperacao = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
                trocaroperacao( carga, s, pilhaop, nvisitas, capcam, compatibilidade, ncam, npilhas);


                movimentocarga(carga,s,npilhas,&pilha1, &pilha2, nvisitas, capcam,compatibilidade,ncam);
                focarga = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
                trocar(carga,s,pilha2, pilha1, nvisitas, capcam, compatibilidade, ncam);


                if(fonvisitas < focarga && fonvisitas < fooperacao){
                        fovizinho = fonvisitas;
                        trocarnvisitas(&ok, nvisitas, nviagem, 1, tipomov, pilha, caminhao);
                        movimento = 0;
                        contnvisitas++;
                }
                if(focarga < fooperacao && focarga < fonvisitas){
                        fovizinho = focarga;
                        movimento=1;
                        trocar(carga,s,pilha1, pilha2, nvisitas, capcam, compatibilidade, ncam);
                        contcarga++;
                }
                if(fooperacao < fonvisitas && fooperacao < focarga){
                        fovizinho = fooperacao;
                        movimento = 2;
                        trocaroperacao( carga, s, pilhaop, nvisitas, capcam, compatibilidade, ncam, npilhas);
                        contop++;
                }
                if(fonvisitas == focarga || fonvisitas == fooperacao){
                        fovizinho = fonvisitas;
                        trocarnvisitas(&ok, nvisitas, nviagem, 1, tipomov, pilha, caminhao);
                        movimento = 0;
                        contnvisitas++;
                }

//------------------------------------------------------------------------------

 /*       fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

*/


/*

                if (fovizinho == focorrente){
                        contadorcorrente++;
                        printf("contador corrente %4d\n",contadorcorrente);
                        contador =0;
                }
                if (fomin == 708505){
                contador++;
                printf("contador 2 %4d\n",contador);
                getchar();
                }
                else
                contador =0;

*/




           	if (fovizinho < focorrente){
               	        focorrente = fovizinho;
                        contadorcorrente = 0;

                }
                if (fovizinho < fomin) {
                        fomin = fovizinho;
			atualizamatrizint(star_nvisitas, nvisitas, npilhas,ncam);
                        atualizamelhorsolucaoalocarga(star_carga,carga,npilhas);
                        carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);
                        printf("***fomin_SA = %12d\n",fomin);
                }
                else{
                        delta =  fovizinho  - focorrente;
             	        aleatorio = randomico(0,1);
                        if (aleatorio < exp((-1)*(double) delta/temperatura)){
		                   focorrente = fovizinho;
                        }
                        else{
                                if(movimento==0){
                                       if(tipomov == true)
                                                trocarnvisitas(&ok, nvisitas, nviagem, 1, false, pilha, caminhao);
                                       else
                                                trocarnvisitas(&ok, nvisitas, nviagem, 1, true, pilha, caminhao);

                                }
                                if(movimento==1){
                                        trocar(carga,s,pilha2, pilha1, nvisitas, capcam, compatibilidade, ncam);

                                }
                                if(movimento==2){
                                        trocaroperacao( carga, s, pilhaop, nvisitas, capcam, compatibilidade, ncam, npilhas);

                                }
                                carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

                        }

                }
        } /* SAmax */
        temperatura *= alfa;
} /* temperatura */

    atualizamatrizint(nvisitas,star_nvisitas,npilhas,ncam);
    atualizamelhorsolucaoalocarga(carga,star_carga,npilhas);
    carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);
    liberamatriz(star_nvisitas,npilhas);
    liberavetoralocarga(star_carga);

 return fomin;
}
//----------------------------------------------------------------------
int faznada()
{
printf("FAZ NADA");
getchar();
return 0;
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
