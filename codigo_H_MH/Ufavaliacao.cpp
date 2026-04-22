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
#include "hstruct.h"


//----------------------------------------------------------------------
int favaliacao(float *s,float *li,float *ls,float *rec,int *penalidadesup,int
        *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
        ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
        ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
        ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
        ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
        , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem)
{
        int fo = 0;
        float producaomin = 1.0;
        float producaototal = 1.0;
        float rem = 0.0;
        carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);
        producaomin = prodtotal(s,npminerio);
        producaototal = prodtotal(s,npilhas);

        if(producaomin == 0)
                rem = 0.0;
        else
                rem = (producaototal-producaomin)/producaomin;





        fo += fa_producao(producaomin,prodmax,prodmin,prodrec,wfproducaoinv,wfproducaoqualp,wfproducaoqualn);
//        printf("fa_producao %8d\n",fa_producao(producaomin,prodmax,prodmin,prodrec,wfproducaoinv,wfproducaoqualp,wfproducaoqualn));
        for(int j=0;j<nvar;j++){
                fo +=fa_teor(encontrado(s,teores,npminerio,j), ls[j], li[j], rec[j], penalidadeteorinv[j], penalidadesup[j], penalidadeinf[j]);
//        printf("fa_teor[%2d] %8d\n",j,fa_teor(encontrado(s,teores,npminerio,j), ls[j], li[j], rec[j], penalidadeteorinv[j], penalidadesup[j], penalidadeinf[j]));
//        getchar();
        }

        fo += fa_rem( rem, remmax, remmin, remrec, wfrem, wfremqualp, wfremqualn);

 //       printf("fo_rem\n",fa_rem( rem, remmax, remmin, remrec, wfrem, wfremqualp, wfremqualn));
        for(int l=0;l<ncam;l++){
                fo += fa_produtcam(produtcam(nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, npilhas, l)
                                  ,1.0, 0.0, dispmec[l], wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn);

        }

        for(int i=0;i<npilhas;i++){

                fo += fa_prodcarga(s[i], cargamax[carga[i].equip], cargamin[carga[i].equip], wfprodcargainv, wfprodcargaqual);
        }
        fo += fa_estatico(nvisitas, carga, compatibilidade, npilhas, ncam, wfestatico);
        fo += fa_numcamfrente( nvisitas, carga, compatibilidade, npilhas, ncam, wfnumcam, nummax);
        fo += fa_numcam(nvisitas, carga, compatibilidade, npilhas, ncam,wfnumcamutilizados);

return fo;

}
//----------------------------------------------------------------------
float prodtotal(float *s, int npilhas)
{
        float soma=0.0;

        for(int i=0;i<npilhas;i++){
                soma += s[i];
        }
return soma;
}
//----------------------------------------------------------------------
int fa_producao(float producaomin, float prodmax, float prodmin, float prodrec, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn)
{
        int fo=0;

        if(producaomin < prodmin)
                return wfproducaoinv*fameta(producaomin, prodmin, prodrec,10000);
        if(producaomin >= prodmin && producaomin < prodrec)
                return wfproducaoqualn*fameta(producaomin, prodmin, prodrec,10000);
        if(producaomin == prodrec)
                return 0;
        if(producaomin > prodrec && producaomin <= prodmax)
                return wfproducaoqualp*fameta(producaomin, prodmax, prodrec,10000);
        if(producaomin > prodmax)
                return wfproducaoinv*fameta(producaomin, prodmax, prodrec,10000);


return fo;
}
//----------------------------------------------------------------------
int fa_teor(float teor, float teormax, float teormin, float teorrec, int wfteorinv, int wfteorqualp, int wfteorqualn)
{
        int fo=0;
        if(teor < teormin)
                return wfteorinv*fameta2(teor, teorrec,0.0001);
        if(teor >= teormin && teor < teorrec)
                return wfteorqualn*fameta2(teor, teorrec,0.0001);
        if(teor == teorrec)
                return 0;
        if(teor > teorrec && teor <= teormax)
                return wfteorqualp*fameta2(teor, teorrec,0.0001);
        if(teor > teormax)
                return wfteorinv*fameta2(teor, teorrec,0.0001);


return fo;
}
//----------------------------------------------------------------------
int fa_rem(float rem, float remmax, float remmin, float remrec, int wfreminv, int wfremqualp, int wfremqualn)
{
        int fo=0;
        if(rem < remmin)
                return wfreminv*fameta(rem, remmin, remrec,1000);
        if(rem >= remmin && rem < remrec)
                return 0*wfremqualn*fameta(rem, remmin, remrec,1000);
        if(rem == remrec)
                return 0;
        if(rem > remrec && rem <= remmax)
                return 0*wfremqualp*fameta(rem, remmax, remrec,1000);
        if(rem > remmax)
                return 0*wfreminv*fameta(rem, remmax, remrec,1000);



return fo;
}
//----------------------------------------------------------------------
int fa_produtcam(float produtcam, float produtcammax, float produtcammin, float produtcamrec, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn)
{
        int fo=0;
        if(produtcam == 0.0)
                return 0.0;
        if(produtcam < produtcammin)
                return wfprodutcaminv*fameta(produtcam, produtcammin, produtcamrec,10000);
        if(produtcam >= produtcammin && produtcam < produtcamrec)
                return 0*wfprodutcamqualn*fameta(produtcam, produtcammin, produtcamrec,1000);
        if(produtcam == produtcamrec)
                return 0.0;
        if(produtcam > produtcamrec && produtcam <= produtcammax)
                return 0*wfprodutcamqualp*fameta(produtcam, produtcammax, produtcamrec,1000);
        if(produtcam > produtcammax)
                return wfprodutcaminv*fameta(produtcam, produtcammax, produtcamrec,10000);



return fo;
}
//----------------------------------------------------------------------
int fa_prodcarga(float prodcarga, float prodcargamax, float prodcargamin, int wfprodcargainv, int wfprodcargaqual)
{
        int fo=0;
        if (prodcarga == 0)
                return 0;
        if(prodcarga < prodcargamin)
                return wfprodcargainv*fameta(prodcarga, prodcargamin, prodcargamax,10000);
        if(prodcarga >= prodcargamin && prodcarga <= prodcargamax)
                return 0*wfprodcargaqual*fameta(prodcarga, prodcargamin, prodcargamax,1000);
        if(prodcarga > prodcargamax)
                return wfprodcargainv*fameta(prodcarga, prodcargamax, prodcargamin,10000);



return fo;
}
//----------------------------------------------------------------------
int fa_estatico(int **nvisitas, struct alocarga *carga, int **compatibilidade, int npilhas,int ncam, int wfestatico)
{
        int soma=0;
        for(int k=0;k<ncam;k++){
                soma += (dinamico(nvisitas, carga, compatibilidade, npilhas,k));
        }
return (wfestatico*estdin)*(soma);
}
//----------------------------------------------------------------------
int fameta(float x, float valor, float meta, int valormax)
{
return ceil((-valormax*(x-meta))/(meta - valor));
}
//----------------------------------------------------------------------

int fa_numcamfrente(int **nvisitas, struct alocarga *carga, int **compatibilidade, int npilhas,int ncam, int wfnumcam, int *nummax)
{
        int soma=0;
        int aux;
        for(int i=0;i<npilhas;i++){
                aux =  numcamfrente( nvisitas, carga,  compatibilidade, ncam, i, nummax) -  nummax[i];
                if(aux > 0)
                soma += aux;
        }
return (wfnumcam*estdin)*(soma);
}
//----------------------------------------------------------------------
int fa_numcam(int **nvisitas, struct alocarga *carga, int **compatibilidade, int npilhas,int ncam, int wfnumcamutilizados)
{
        int soma=0;
        for(int l = 0; l< ncam; l++ )
                soma += camutilizados(nvisitas,carga,compatibilidade, npilhas, l);

return 0*wfnumcamutilizados*(soma);
}
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------

//----------------------------------------------------------------------
int fameta2(float x,float meta, float delta)
{
return ceil(absfloat(x-meta)/delta);
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------


