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
//----------------------------------------------------------------------
#include "Hutil.h"
#include "Hdefine.h"
#include "Hutilcarga.h"
#include "Hfavaliacao.h"
#include "Hsa.h"
#include "Hmovimento.h"

#include "hstruct.h"
//----------------------------------------------------------------------
//----------------------------------------------------------------------






void movimentovisita(int **nvisitas,int **compatibilidade,struct alocarga *carga, int passo, int npilhas,int ncam, int *nviagem, int *pilha,int *caminhao, bool *tipomov,float *s, float *capcam)
{

bool ok;
int cont=0;

do{
*pilha = randint(0,npilhas-1);
*caminhao = randint(0,ncam-1);
*tipomov = randint(0,1);
passo = randint(1,passo);

ok = false;
cont++;

if(carga[*pilha].op == true)
if((carga[*pilha].equip != disponivel) && (compatibilidade[*caminhao][carga[*pilha].equip ] == compativel)){
       trocarnvisitas(&ok,nvisitas,  nviagem,  passo, *tipomov, *pilha, *caminhao);

}
}while((!ok)&&(cont<100000));
carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

}

//----------------------------------------------------------------------
void movimentooperacao(struct alocarga *carga,float *s,int npilhas,int *pilhaop,bool *tipomovop, int **nvisitas, float *capcam,int **compatibilidade,int ncam)
{
bool ok;
int cont=0;

do{
        *pilhaop = randint(0,npilhas-1);
        ok = false;
        cont++;
        if(carga[*pilhaop].equip != disponivel)
                ok =true;
}while((!ok)&&(cont<100000));

trocaroperacao( carga, s, *pilhaop, nvisitas, capcam, compatibilidade, ncam, npilhas);
carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

}
//----------------------------------------------------------------------
void movimentocarga(struct alocarga *carga,float *s,int npilhas,int *pilha1, int *pilha2, int **nvisitas, float *capcam,int **compatibilidade,int ncam)
{
int cont=0;
bool ok;
do{
*pilha1 = randint(0,npilhas-1);
*pilha2 = randint(0,npilhas-1);
cont++;
ok = false;
if(carga[*pilha1].equip != carga[*pilha2].equip ){
        if((carga[*pilha1].op != false) || (carga[*pilha2].op != false )){
                trocar(carga,s,*pilha1,*pilha2, nvisitas, capcam,compatibilidade,ncam);
                ok = true;
        }
}
}while((!ok)&&(cont<100000));
carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

}
//----------------------------------------------------------------------
int melhorvizinhocarga(float *s,float *li,float *ls,float *rec,int *penalidadesup
                             ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
                             ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
                             ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
                             ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
                             ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
                             , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem)
{
    int k1,k2;
    int   fomin, fooriginal, fovizinho;

	fooriginal = fomin = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);


	for (int i=0;i<npilhas;i++){
    		for (int j=i+1;j<npilhas;j++){

                        trocar(carga,s,i,j, nvisitas, capcam,compatibilidade,ncam);
    			fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

                	if (fovizinho < fomin){
                	        k1 = i;
                    	        k2 = j;
                    	        fomin = fovizinho;
              		}
                	trocar(carga,s,j,i, nvisitas, capcam,compatibilidade,ncam);
                }

    }
    if (fomin < fooriginal)  trocar(carga,s,k1,k2, nvisitas, capcam,compatibilidade,ncam);
	return fomin;
}
//*******************************************************************************
int melhorvizinhogeral(float *s,float *li,float *ls,float *rec,int *penalidadesup
                             ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
                             ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
                             ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
                             ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
                             ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
                             , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem)
{
    int pilha1,pilha2;
    int   fomin, fooriginal, fovizinho;
    int pilha,caminhao,tipomov;
    int passo =1;
    bool ok;



	fooriginal = fomin = fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);



     for(int p1=0;p1<npilhas;p1++){
     for(int p2=p1+1;p2<npilhas;p2++){
     trocar(carga,s,p1,p2, nvisitas, capcam,compatibilidade,ncam);

	for (int i=0;i<npilhas;i++){
        if(carga[i].equip != disponivel)
    		for (int j=0;j<ncam;j++){
                if(compatibilidade[j][carga[i].equip] == compativel)
                        for(int k=0;k<2;k++){
                                trocarnvisitas(&ok,nvisitas,  nviagem,  passo, k, i, j);
                                fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);


                	        if (fovizinho < fomin && ok == true){
                                        pilha1 = p1;
                                        pilha2 = p2;
                                        pilha = i;
                        	        caminhao = j;
                                        tipomov = k;
                        	        fomin = fovizinho;
                                       // printf("***P1 %2d P2%2d Pilha%2d Cam%2d Tipomov %2d\n",pilha1,pilha2,pilha,caminhao,tipomov);
              		        }

                                if(k==0&& ok==true)
                	                trocarnvisitas(&ok,nvisitas,  nviagem,  passo, 1, i, j);
                                if(k==1&& ok==true)
                                        trocarnvisitas(&ok,nvisitas,  nviagem,  passo, 0, i, j);
                        }
                }

    }

    trocar(carga,s,p2,p1, nvisitas, capcam,compatibilidade,ncam);
    }
    }
    if (fomin < fooriginal){
    trocar(carga,s,pilha1,pilha2, nvisitas, capcam,compatibilidade,ncam);
    trocarnvisitas(&ok,nvisitas,  nviagem,  passo, tipomov, pilha, caminhao);
    printf("***P1 %2d P2%2d Pilha%2d Cam%2d Tipomov %2d\n",pilha1,pilha2,pilha,caminhao,tipomov);
    }
    return fomin;


}
//----------------------------------------------------------------------

int melhorvizinhonvisitas(float *s,float *li,float *ls,float *rec,int *penalidadesup
                             ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
                             ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
                             ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
                             ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
                             ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
                             , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem)
{
    int fomin, fooriginal, fovizinho;
    int pilha,caminhao,tipomov;
    int passo = 1;
    bool ok;

	fooriginal = fomin = fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);


	for (int i=0;i<npilhas;i++){
        if(carga[i].equip != disponivel)
    		for (int j=0;j<ncam;j++){
                if(compatibilidade[j][carga[i].equip] == compativel)
                        for(int k=0;k<2;k++){
                                trocarnvisitas(&ok,nvisitas,  nviagem,  passo, k, i, j);
                                fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

                	        if (fovizinho < fomin && ok == true){

                	                pilha = i;
                        	        caminhao = j;
                                        tipomov = k;
                        	        fomin = fovizinho;
                                        //printf("***Pilha%2d Cam%2d Tipomov %2d\n",pilha,caminhao,tipomov);
              		        }

                                if(k==0&& ok==true)
                	                trocarnvisitas(&ok,nvisitas,  nviagem,  passo, 1, i, j);
                                if(k==1&& ok==true)
                                        trocarnvisitas(&ok,nvisitas,  nviagem,  passo, 0, i, j);
                        }
                }

    }
    if (fomin < fooriginal){
        trocarnvisitas(&ok,nvisitas,  nviagem,  passo, tipomov, pilha, caminhao);
        //printf("***Pilha%2d Cam %2d Tipomov %2d\n",pilha,caminhao,tipomov);
    }
    return fomin;
}
//----------------------------------------------------------------------
int melhorvizinhooperacao(float *s,float *li,float *ls,float *rec,int *penalidadesup
                             ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
                             ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
                             ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
                             ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
                             ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
                             , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem)
{
    int k1;
    int   fomin, fooriginal, fovizinho;

	fooriginal = fomin = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);


	for (int i=0;i<npilhas;i++){

                        trocaroperacao( carga, s, i, nvisitas, capcam, compatibilidade, ncam, npilhas);

    			fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

                	if (fovizinho < fomin){
                	        k1 = i;
                    	        fomin = fovizinho;
              		}
                	trocaroperacao( carga, s, i, nvisitas, capcam, compatibilidade, ncam, npilhas);


    }
    if (fomin < fooriginal)  trocaroperacao( carga, s, k1, nvisitas, capcam, compatibilidade, ncam, npilhas);
	return fomin;
}
//----------------------------------------------------------------------
void movimentoviagemfrente(int **nvisitas,int **compatibilidade,struct alocarga *carga, int passo, int npilhas,int ncam, int *nviagem, int *pilha,int *caminhao1,int *caminhao2,float *s, float *capcam)
{

bool ok;
int cont =0;

do{
*pilha = randint(0,npilhas-1);
*caminhao1 = randint(0,ncam-1);
*caminhao2 = randint(0,1);
passo = randint(1,passo);
cont++;

ok = false;

if(carga[*pilha].op == true)
if((carga[*pilha].equip != disponivel) && (*caminhao1 != *caminhao2))
        if( compatibilidade[*caminhao1][carga[*pilha].equip ] == compativel&& compatibilidade[*caminhao2][carga[*pilha].equip ] == compativel)
                if(((nvisitas[*pilha][*caminhao1] + passo) <= nviagem[*pilha]) && ((nvisitas[*pilha][*caminhao2] - passo) >= 0)){
                        trocarviagemcaminhaofrente(nvisitas, *pilha, *caminhao1, *caminhao2,passo);
                        ok = true;

                }
}while((!ok) && (cont<100000));
carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

}

//----------------------------------------------------------------------
void movimentoviagemcam(int **nvisitas,int **compatibilidade,struct alocarga *carga, int passo, int npilhas,int ncam, int *nviagem, int *cam,int *pilha1,int *pilha2,float *s, float *capcam)
{

bool ok;
int cont=0;

do{
*pilha1 = randint(0,npilhas-1);
*pilha2 = randint(0,npilhas-1);
*cam = randint(0,ncam-1);
passo = randint(1,passo);
cont++;
ok = false;

if((carga[*pilha1].op == true) && (carga[*pilha2].op == true))
if(*pilha1 != *pilha2)
if((carga[*pilha1].equip != disponivel) && (carga[*pilha2].equip != disponivel))
        if(( compatibilidade[*cam][carga[*pilha1].equip ] == compativel) && (compatibilidade[*cam][carga[*pilha2].equip ] == compativel))
                if(((nvisitas[*pilha1][*cam] + passo) <= nviagem[*pilha1]) && ((nvisitas[*pilha2][*cam] - passo) >= 0)){
                        trocarviagemcaminhaocam(nvisitas, *cam, *pilha1, *pilha2,passo);
                        ok = true;

                }
}while((!ok)&&(cont<100000));
carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

}
//----------------------------------------------------------------------
void movimentooperacaocam(int **nvisitas,int **compatibilidade,struct alocarga *carga, int passo, int npilhas,int ncam, int *nviagem, int *cam,int *pilha,float *s, float *capcam)
{

bool ok;
int cont =0;

do{
*pilha = randint(0,npilhas-1);
*cam = randint(0,ncam-1);
cont++;
ok = false;

if((carga[*pilha].op == true) && (carga[*pilha].equip != disponivel))
        if( compatibilidade[*cam][carga[*pilha].equip ] == compativel && nvisitas[*pilha][*cam] != 0){
                        nvisitas[*pilha][*cam] = 0;
                        ok = true;

                }
}while((!ok) && (cont < 100000));
carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

}
//----------------------------------------------------------------------
int melhorvizinhonviagemfrente(float *s,float *li,float *ls,float *rec,int *penalidadesup
                             ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
                             ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
                             ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
                             ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
                             ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
                             , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem)
{
    int fomin, fooriginal, fovizinho;
    int pilha,caminhao1,caminhao2;
    int passo = 1;

	fooriginal = fomin = fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);


	for (int i=0;i<npilhas;i++){
        if(carga[i].equip != disponivel && carga[i].op == true)
    		for (int j=0;j<ncam;j++){
                        for(int k = 0; k<ncam;k++){
                                if(j==k)break;
                                if(compatibilidade[j][carga[i].equip] == compativel && compatibilidade[k][carga[i].equip] == compativel)
                                if((nvisitas[i][j] + passo) <= nviagem[i] && (nvisitas[i][k] - passo) >= 0){
                                //printf("*** %2d  -> %2d\n", nvisitas[i][j],nvisitas[i][k]);
                                trocarviagemcaminhaofrente(nvisitas, i, j, k,passo);
                                fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

                	        if (fovizinho < fomin){

                	                pilha = i;
                        	        caminhao1 = j;
                        	        caminhao2 = k;
                        	        fomin = fovizinho;

              		        }
                                //printf("*** %2d  -> %2d\n", nvisitas[i][j],nvisitas[i][k]);
                                trocarviagemcaminhaofrente(nvisitas, i, k, j,passo);
                                //printf("aaa %2d  -> %2d\n", nvisitas[i][j],nvisitas[i][k]);getchar();
                                }
                        }
                }

    }
    if (fomin < fooriginal){
                trocarviagemcaminhaofrente(nvisitas, pilha, caminhao1, caminhao2,passo);
//                printf("FINAL  Pilha %2d Cam1 %2d Cam2 %2d ------- %2d  -> %2d\n",pilha,caminhao1,caminhao2,nvisitas[pilha][caminhao1],nvisitas[pilha][caminhao2]);getchar();
    }
    return fomin;
}
//----------------------------------------------------------------------
//----------------------------------------------------------------------
int melhorvizinhonviagemcam(float *s,float *li,float *ls,float *rec,int *penalidadesup
                             ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
                             ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
                             ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
                             ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
                             ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
                             , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem)
{
    int fomin, fooriginal, fovizinho;
    int cam,pilha1,pilha2;
    int passo = 1;

	fooriginal = fomin = fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);


	for (int i=0;i<ncam;i++){
                for (int j=0;j<npilhas;j++){
                        if(carga[j].equip != disponivel && carga[j].op == true)
                                for(int k = 0; k<npilhas;k++){
                                if(j==k)break;
                                if(compatibilidade[i][carga[j].equip] == compativel && compatibilidade[i][carga[k].equip] == compativel)
                                if(carga[k].equip != disponivel && carga[k].op == true){
                                        if((nvisitas[j][i] + passo) <= nviagem[j] && (nvisitas[k][i] - passo) >= 0){
                                trocarviagemcaminhaocam(nvisitas, i, j, k,passo);
                                fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

                	        if (fovizinho < fomin){

                	                cam = i;
                        	        pilha1 = j;
                        	        pilha2 = k;
                        	        fomin = fovizinho;
                                        //printf("***Pilha%2d Cam%2d Tipomov %2d\n",pilha,caminhao,tipomov);
              		        }
                                trocarviagemcaminhaocam(nvisitas, i, k, j,passo);
                                }
                                }
                        }
                }

    }
    if (fomin < fooriginal){
                trocarviagemcaminhaocam(nvisitas, cam, pilha1, pilha2,passo);
//        printf("***Cam%2d Pilha1 %2d Pilha2 %2d\n",cam,pilha1,pilha2);getchar();
    }
    return fomin;
}
//----------------------------------------------------------------------
int melhorvizinhooperacaocam(float *s,float *li,float *ls,float *rec,int *penalidadesup
                             ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
                             ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
                             ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
                             ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
                             ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
                             , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem)
{
    int fomin, fooriginal, fovizinho;
    int cam,pilha,aux;

	fooriginal = fomin = fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);


	for (int i=0;i<npilhas;i++){
                if(carga[i].equip != disponivel && carga[i].op == true)
                                for(int j = 0; j<ncam;j++){
                                        if(compatibilidade[j][carga[i].equip] == compativel && nvisitas[i][j] != 0){
                                        aux = nvisitas[i][j];
                                        nvisitas[i][j] = 0;
                                fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

                	        if (fovizinho < fomin){

                                        pilha = i;
                	                cam = j;
                        	        fomin = fovizinho;
                                        //printf("***Pilha%2d Cam%2d Tipomov %2d\n",pilha,caminhao,tipomov);
              		        }
                                nvisitas[i][j] = aux;
                                }
                                }
                        
                

    }
    if(fomin < fooriginal){
                nvisitas[pilha][cam] = 0;
//        printf("***Cam% 2d Pilha %2d \n",cam,pilha);getchar();
    }
    return fomin;
}

