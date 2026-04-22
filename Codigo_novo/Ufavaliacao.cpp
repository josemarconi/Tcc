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
#include "hstruct.h"

//----------------------------------------------------------------------
long long favaliacao(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem)
{

        long long fo = 0;
        float producaomin = 1.0;
        float producaototal = 1.0;
        float rem = 0.0;
        carregavetors(s, nvisitas, carga, capcam, compatibilidade, ncam, npilhas);
        producaomin = prodtotal(s, npminerio);
        producaototal = prodtotal(s, npilhas);

        if (producaomin == 0)
                rem = 0.0;
        else
                rem = (producaototal - producaomin) / producaomin;

        fo += fa_producao(producaomin, prodmax, prodmin, prodrec, wfproducaoinv, wfproducaoqualp, wfproducaoqualn);
        //        printf("fa_producao %8d\n",fa_producao(producaomin,prodmax,prodmin,prodrec,wfproducaoinv,wfproducaoqualp,wfproducaoqualn));
        for (int j = 0; j < nvar; j++)
        {
                fo += fa_teor(encontrado(s, teores, npminerio, j), ls[j], li[j], rec[j], penalidadeteorinv[j], penalidadesup[j], penalidadeinf[j]);
                //        printf("fa_teor[%2d] %8d\n",j,fa_teor(encontrado(s,teores,npminerio,j), ls[j], li[j], rec[j], penalidadeteorinv[j], penalidadesup[j], penalidadeinf[j]));
                //        getchar();
        }

        fo += fa_rem(rem, remmax, remmin, remrec, wfrem, wfremqualp, wfremqualn);

        //       printf("fo_rem\n",fa_rem( rem, remmax, remmin, remrec, wfrem, wfremqualp, wfremqualn));
        for (int l = 0; l < ncam; l++)
        {
                fo += fa_produtcam(produtcam(nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, npilhas, l), 1.0, 0.0, dispmec[l], wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn);
        }

        for (int i = 0; i < npilhas; i++)
        {

                fo += fa_prodcarga(s[i], cargamax[carga[i].equip], cargamin[carga[i].equip], wfprodcargainv, wfprodcargaqual);
        }
        fo += fa_estatico(nvisitas, carga, compatibilidade, npilhas, ncam, wfestatico);
        fo += fa_numcamfrente(nvisitas, carga, compatibilidade, npilhas, ncam, wfnumcam, nummax);
        fo += fa_numcam(nvisitas, carga, compatibilidade, npilhas, ncam, wfnumcamutilizados);

        return fo;
}
//----------------------------------------------------------------------
float prodtotal(float *s, int npilhas)
{
        float soma = 0.0;

        for (int i = 0; i < npilhas; i++)
        {
                soma += s[i];
        }
        return soma;
}
//----------------------------------------------------------------------
long long fa_producao(float producaomin, float prodmax, float prodmin, float prodrec, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn)
{
        long long fo = 0;

        if (producaomin < prodmin)
                return wfproducaoinv * fameta3(producaomin, prodmin, prodrec, 1000, prodmax);
        if (producaomin >= prodmin && producaomin < prodrec)
                return wfproducaoqualn * fameta3(producaomin, prodmin, prodrec, 1000, prodmax);
        if (producaomin == prodrec)
                return 0;
        if (producaomin > prodrec && producaomin <= prodmax)
                return wfproducaoqualp * fameta3(producaomin, prodmax, prodrec, 1000, 1);
        if (producaomin > prodmax)
                return wfproducaoinv * fameta3(producaomin, prodmax, prodrec, 1000, 1);

        return fo;
}
//----------------------------------------------------------------------
long long fa_teor(float teor, float teormax, float teormin, float teorrec, int wfteorinv, int wfteorqualp, int wfteorqualn)
{
        float delta = 0.0;

        long long fo = 0;

        if (teor < teormin)
        {
                if (teorrec == teormin)
                        delta = teormax - teormin;
                else
                        delta = teorrec - teormin;

                return wfteorinv * fameta2(teor, teorrec, absfloat(delta));
        }
        if (teor >= teormin && teor < teorrec)
        {
                if (teorrec == teormin)
                        delta = teormax - teormin;
                else
                        delta = teorrec - teormin;

                return wfteorqualn * fameta2(teor, teorrec, absfloat(delta));
        }
        if (teor == teorrec)
                return 0;
        if (teor > teorrec && teor <= teormax)
        {
                if (teorrec == teormax)
                        delta = teormax - teormin;
                else
                        delta = teormax - teorrec;

                return wfteorqualp * fameta2(teor, teorrec, absfloat(delta));
        }
        if (teor > teormax)
        {
                if (teorrec == teormax)
                        delta = teormax - teormin;
                else
                        delta = teormax - teorrec;

                return wfteorinv * fameta2(teor, teorrec, absfloat(delta));
        }

        return fo;
}
//----------------------------------------------------------------------
long long fa_rem(float rem, float remmax, float remmin, float remrec, int wfreminv, int wfremqualp, int wfremqualn)
{
        long long fo = 0;
        if (rem < remmin)
                return wfreminv * fameta(rem, remmin, remrec, 1000);
        if (rem >= remmin && rem < remrec)
                return wfremqualn * fameta(rem, remmin, remrec, 1000);
        if (rem == remrec)
                return 0;
        if (rem > remrec && rem <= remmax)
                return wfremqualp * fameta(rem, remmax, remrec, 1000);
        if (rem > remmax)
                return wfreminv * fameta(rem, remmax, remrec, 1000);

        return fo;
}
//----------------------------------------------------------------------
long long fa_produtcam(float produtcam, float produtcammax, float produtcammin, float produtcamrec, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn)
{
        long long fo = 0;
        if (produtcam == 0.0)
                return 0.0;
        if (produtcam < produtcammin)
                return wfprodutcaminv * fameta(produtcam, produtcammin, produtcamrec, 1000);
        if (produtcam >= produtcammin && produtcam < produtcamrec)
                return wfprodutcamqualn * fameta(produtcam, produtcammin, produtcamrec, 1000);
        if (produtcam == produtcamrec)
                return 0.0;
        if (produtcam > produtcamrec && produtcam <= produtcammax)
                return wfprodutcamqualp * fameta(produtcam, produtcammax, produtcamrec, 1000);
        if (produtcam > produtcammax)
                return wfprodutcaminv * fameta(produtcam, produtcammax, produtcamrec, 1000);

        return fo;
}
//----------------------------------------------------------------------
long long fa_prodcarga(float prodcarga, float prodcargamax, float prodcargamin, int wfprodcargainv, int wfprodcargaqual)
{
        long long fo = 0;
        if (prodcarga == 0)
                return 0;
        if (prodcarga < prodcargamin)
                return wfprodcargainv * fameta(prodcarga, prodcargamin, prodcargamax, 1000);
        if (prodcarga >= prodcargamin && prodcarga <= prodcargamax)
                return wfprodcargaqual * fameta(prodcarga, prodcargamin, prodcargamax, 1000);
        if (prodcarga > prodcargamax)
                return wfprodcargainv * fameta(prodcarga, prodcargamax, prodcargamin, 1000);

        return fo;
}
//----------------------------------------------------------------------
long long fa_estatico(int **nvisitas, struct alocarga *carga, int **compatibilidade, int npilhas, int ncam, int wfestatico)
{
        int soma = 0;
        for (int k = 0; k < ncam; k++)
        {
                soma += (dinamico(nvisitas, carga, compatibilidade, npilhas, k));
        }
        return (wfestatico * estdin) * (soma);
}
//----------------------------------------------------------------------
long long fameta(float x, float valor, float meta, int valormax)
{

        return (long long)ceil((-valormax * (x - meta)) / (meta - valor));
}
//----------------------------------------------------------------------

long long fa_numcamfrente(int **nvisitas, struct alocarga *carga, int **compatibilidade, int npilhas, int ncam, int wfnumcam, int *nummax)
{
        int soma = 0;
        int aux = 0;
        for (int i = 0; i < npilhas; i++)
        {
                aux = numcamfrente(nvisitas, carga, compatibilidade, ncam, i, nummax) - nummax[i];
                if (aux > 0)
                        soma += aux;
        }
        return (wfnumcam * estdin) * (soma);
}
//----------------------------------------------------------------------
long long fa_numcam(int **nvisitas, struct alocarga *carga, int **compatibilidade, int npilhas, int ncam, int wfnumcamutilizados)
{
        int soma = 0;
        for (int l = 0; l < ncam; l++)
                soma += camutilizados(nvisitas, carga, compatibilidade, npilhas, l);

        return wfnumcamutilizados * (soma);
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
long long fameta2(float x, float meta, float delta)
{
        return (long long)ceil(absfloat(x - meta) / delta); // corrigir (delta ta um mínimo, não a diferença que deveria utilizar) //
}

long long fameta3(float x, float valor, float meta, int valormax, int controle)
{

        float divisor = (meta == valor) ? (float)(valormax - controle) : (meta - valor);

        if (absfloat(divisor) < 0.00001)
                divisor = 1.0; // Evita divisão por zero

        long long resultado = (long long)ceil((-valormax * (x - meta)) / divisor);

        // Se o objetivo é minimizar, a penalidade nunca deve ser negativa
        if (resultado < 0)
                return abs(resultado);
        return resultado;
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
