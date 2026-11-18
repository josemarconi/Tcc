// Objective function (FO) of the mine planning problem. It evaluates one candidate
// solution and returns a penalty value that the search methods minimise.
// The terms are: ore and total production, grade of each quality variable, REM,
// truck utilisation, loader production, and truck-count terms.
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
// Evaluates the objective function of solution s and returns the total penalty (lower is better).
// s[i]: production of pile i; li, ls, rec: lower limit, upper limit and target of each quality variable.
// teores[i][j]: grade of variable j in pile i; the first npminerio piles are the ore piles.
// nvisitas, carga, compatibilidade, tciclo describe truck visits, loader assignments and cycle times.
// The wf* arguments are the weights of the terms; penalidadesup, penalidadeinf, penalidadeteorinv are per-variable weights.
// Side effect: s is overwritten with the production recomputed from the current assignment (carregavetors).
long long favaliacao(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem)
{

        // Accumulated objective value; each term below adds its penalty to it.
        long long fo = 0;
        float producaomin = 1.0;
        float producaototal = 1.0;
        float rem = 0.0;
        // Recompute the production of every pile from the truck visits and loader assignments.
        carregavetors(s, nvisitas, carga, capcam, compatibilidade, ncam, npilhas);
        // Ore production: sum of the first npminerio piles.
        producaomin = prodtotal(s, npminerio);
        // Total production: sum over all npilhas piles.
        producaototal = prodtotal(s, npilhas);

        // REM (waste-to-ore ratio) = (total - ore) / ore; it is 0 when there is no ore production.
        if (producaomin == 0)
                rem = 0.0;
        else
                rem = (producaototal - producaomin) / producaomin;

        // Penalty for the ore production against the prodmin, prodrec and prodmax limits.
        fo += fa_producao(producaomin, prodmax, prodmin, prodrec, wfproducaoinv, wfproducaoqualp, wfproducaoqualn);
        //        printf("fa_producao %8d\n",fa_producao(producaomin,prodmax,prodmin,prodrec,wfproducaoinv,wfproducaoqualp,wfproducaoqualn));
        // Grade penalty for each quality variable j, using the ore grade (encontrado) against its limits and target.
        for (int j = 0; j < nvar; j++)
        {
                fo += fa_teor(encontrado(s, teores, npminerio, j), ls[j], li[j], rec[j], penalidadeteorinv[j], penalidadesup[j], penalidadeinf[j]);
                //        printf("fa_teor[%2d] %8d\n",j,fa_teor(encontrado(s,teores,npminerio,j), ls[j], li[j], rec[j], penalidadeteorinv[j], penalidadesup[j], penalidadeinf[j]));
                //        getchar();
        }

        // REM penalty against the remmin, remrec and remmax limits.
        fo += fa_rem(rem, remmax, remmin, remrec, wfrem, wfremqualp, wfremqualn);

        //       printf("fo_rem\n",fa_rem( rem, remmax, remmin, remrec, wfrem, wfremqualp, wfremqualn));
        // Truck utilisation penalty: for each truck l, produtcam (its utilisation of the control time)
        // is penalised with the fixed limits 0.0 and 1.0 and with dispmec[l] as the target.
        for (int l = 0; l < ncam; l++)
        {
                fo += fa_produtcam(produtcam(nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, npilhas, l), 1.0, 0.0, dispmec[l], wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn);
        }

        // Loader production penalty: each pile production s[i] against the minimum and maximum production of its loader.
        for (int i = 0; i < npilhas; i++)
        {

                fo += fa_prodcarga(s[i], cargamax[carga[i].equip], cargamin[carga[i].equip], wfprodcargainv, wfprodcargaqual);
        }
        // Dynamic truck term (fa_estatico). It is scaled by estdin, which is 0 in Hdefine.h, so it adds nothing.
        fo += fa_estatico(nvisitas, carga, compatibilidade, npilhas, ncam, wfestatico);
        // Excess trucks per pile over nummax (fa_numcamfrente). It is also scaled by estdin.
        fo += fa_numcamfrente(nvisitas, carga, compatibilidade, npilhas, ncam, wfnumcam, nummax);
        // Number of trucks used (fa_numcam).
        fo += fa_numcam(nvisitas, carga, compatibilidade, npilhas, ncam, wfnumcamutilizados);

        return fo;
}
//----------------------------------------------------------------------
// Total production: sum of s[0] to s[npilhas-1].
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
// Penalty for the total ore production producaomin against prodmin (minimum), prodrec (target) and prodmax (maximum).
long long fa_producao(float producaomin, float prodmax, float prodmin, float prodrec, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn)
{
        long long fo = 0;

        // Below the minimum: penalty with wfproducaoinv, slope between prodmin and prodrec.
        if (producaomin < prodmin)
                return wfproducaoinv * fameta3(producaomin, prodmin, prodrec, 1000, prodmax);
        // Between the minimum and the target: penalty with wfproducaoqualn.
        if (producaomin >= prodmin && producaomin < prodrec)
                return wfproducaoqualn * fameta3(producaomin, prodmin, prodrec, 1000, prodmax);
        // Exactly at the target: no penalty.
        if (producaomin == prodrec)
                return 0;
        // Above the target up to the maximum: penalty with wfproducaoqualp, slope between prodmax and prodrec.
        if (producaomin > prodrec && producaomin <= prodmax)
                return wfproducaoqualp * fameta3(producaomin, prodmax, prodrec, 1000, 1);
        // Above the maximum: penalty with wfproducaoinv.
        if (producaomin > prodmax)
                return wfproducaoinv * fameta3(producaomin, prodmax, prodrec, 1000, 1);

        return fo;
}
//----------------------------------------------------------------------
// Grade penalty for one quality variable. teor is the ore grade; teormax and teormin are the upper and
// lower limits (the caller passes ls[j] and li[j]); teorrec is the target. Each branch uses fameta2.
long long fa_teor(float teor, float teormax, float teormin, float teorrec, int wfteorinv, int wfteorqualp, int wfteorqualn)
{
        // delta: scale of the deviation, taken from the distance between the limits and the target (see branches).
        float delta = 0.0;

        long long fo = 0;

        // Grade below the minimum: penalty with wfteorinv.
        if (teor < teormin)
        {
                // Target equal to the minimum: the full range between the limits is used as scale.
                if (teorrec == teormin)
                        delta = teormax - teormin;
                else
                        delta = teorrec - teormin;

                return wfteorinv * fameta2(teor, teorrec, absfloat(delta));
        }
        // Grade between the minimum and the target: penalty with wfteorqualn.
        if (teor >= teormin && teor < teorrec)
        {
                if (teorrec == teormin)
                        delta = teormax - teormin;
                else
                        delta = teorrec - teormin;

                return wfteorqualn * fameta2(teor, teorrec, absfloat(delta));
        }
        // Grade exactly at the target: no penalty.
        if (teor == teorrec)
                return 0;
        // Grade above the target up to the maximum: penalty with wfteorqualp.
        if (teor > teorrec && teor <= teormax)
        {
                if (teorrec == teormax)
                        delta = teormax - teormin;
                else
                        delta = teormax - teorrec;

                return wfteorqualp * fameta2(teor, teorrec, absfloat(delta));
        }
        // Grade above the maximum: penalty with wfteorinv.
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
// REM penalty with the same structure as fa_teor, using fameta (linear, zero at the target).
long long fa_rem(float rem, float remmax, float remmin, float remrec, int wfreminv, int wfremqualp, int wfremqualn)
{
        long long fo = 0;
        // REM below the minimum: penalty with wfreminv.
        if (rem < remmin)
                return wfreminv * fameta(rem, remmin, remrec, 1000);
        // REM between the minimum and the target: penalty with wfremqualn.
        if (rem >= remmin && rem < remrec)
                return wfremqualn * fameta(rem, remmin, remrec, 1000);
        // REM exactly at the target: no penalty.
        if (rem == remrec)
                return 0;
        // REM above the target up to the maximum: penalty with wfremqualp.
        if (rem > remrec && rem <= remmax)
                return wfremqualp * fameta(rem, remmax, remrec, 1000);
        // REM above the maximum: penalty with wfreminv.
        if (rem > remmax)
                return wfreminv * fameta(rem, remmax, remrec, 1000);

        return fo;
}
//----------------------------------------------------------------------
// Utilisation penalty for one truck. An unused truck (0) has no penalty.
// Weights: wfprodutcaminv outside the limits, wfprodutcamqualn below the target, wfprodutcamqualp above it.
long long fa_produtcam(float produtcam, float produtcammax, float produtcammin, float produtcamrec, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn)
{
        long long fo = 0;
        if (produtcam == 0.0)
                return 0.0;
        // Below the minimum: penalty with wfprodutcaminv.
        if (produtcam < produtcammin)
                return wfprodutcaminv * fameta(produtcam, produtcammin, produtcamrec, 1000);
        // Between the minimum and the target: penalty with wfprodutcamqualn.
        if (produtcam >= produtcammin && produtcam < produtcamrec)
                return wfprodutcamqualn * fameta(produtcam, produtcammin, produtcamrec, 1000);
        if (produtcam == produtcamrec)
                return 0.0;
        // Above the target up to the maximum: penalty with wfprodutcamqualp.
        if (produtcam > produtcamrec && produtcam <= produtcammax)
                return wfprodutcamqualp * fameta(produtcam, produtcammax, produtcamrec, 1000);
        // Above the maximum: penalty with wfprodutcaminv.
        if (produtcam > produtcammax)
                return wfprodutcaminv * fameta(produtcam, produtcammax, produtcamrec, 1000);

        return fo;
}
//----------------------------------------------------------------------
// Loader production penalty for one pile. prodcarga is the production of the pile and the limits are those of its loader.
// A pile with zero production has no penalty.
long long fa_prodcarga(float prodcarga, float prodcargamax, float prodcargamin, int wfprodcargainv, int wfprodcargaqual)
{
        long long fo = 0;
        if (prodcarga == 0)
                return 0;
        // Below the loader minimum: penalty with wfprodcargainv (the fameta value falls to 0 at the maximum).
        if (prodcarga < prodcargamin)
                return wfprodcargainv * fameta(prodcarga, prodcargamin, prodcargamax, 1000);
        // Inside [minimum, maximum]: penalty with wfprodcargaqual; it is 1000 at the minimum and 0 at the maximum.
        if (prodcarga >= prodcargamin && prodcarga <= prodcargamax)
                return wfprodcargaqual * fameta(prodcarga, prodcargamin, prodcargamax, 1000);
        // Above the loader maximum: penalty with wfprodcargainv. The limits are passed swapped, so the penalty grows with the excess.
        if (prodcarga > prodcargamax)
                return wfprodcargainv * fameta(prodcarga, prodcargamax, prodcargamin, 1000);

        return fo;
}
//----------------------------------------------------------------------
// Dynamic truck term: for each truck k, dinamico(k) counts the visited piles served by compatible loaders, minus one (not below zero).
// The total is multiplied by wfestatico and estdin (estdin is 0 in Hdefine.h, so the result is 0).
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
// Linear penalty: ceil(-valormax*(x-meta)/(meta-valor)). It is 0 at meta and valormax at valor.
// Used by the production, REM, truck and loader penalties.
long long fameta(float x, float valor, float meta, int valormax)
{

        return (long long)ceil((-valormax * (x - meta)) / (meta - valor));
}
//----------------------------------------------------------------------

// Excess trucks per pile: for each pile, numcamfrente counts the compatible trucks that visit it.
// The excess over nummax[i] is summed and multiplied by wfnumcam and estdin (0 in Hdefine.h).
long long fa_numcamfrente(int **nvisitas, struct alocarga *carga, int **compatibilidade, int npilhas, int ncam, int wfnumcam, int *nummax)
{
        int soma = 0;
        int aux = 0;
        // For each pile i: keep only the positive excess of trucks over nummax[i].
        for (int i = 0; i < npilhas; i++)
        {
                aux = numcamfrente(nvisitas, carga, compatibilidade, ncam, i, nummax) - nummax[i];
                if (aux > 0)
                        soma += aux;
        }
        return (wfnumcam * estdin) * (soma);
}
//----------------------------------------------------------------------
// Trucks-used term: number of trucks with at least one visit (camutilizados), times wfnumcamutilizados.
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
// Deviation penalty: ceil(|x - meta| / delta), the absolute distance from the target in units of delta. Used by fa_teor.
long long fameta2(float x, float meta, float delta)
{
        return (long long)ceil(absfloat(x - meta) / delta); // to fix (delta is a minimum, not the difference that should be used) //
}

// Linear penalty like fameta, with safeguards: when valor equals meta, the slope uses valormax - controle
// (the 5th argument); a near-zero divisor is replaced by 1. The result is returned as a non-negative value.
long long fameta3(float x, float valor, float meta, int valormax, int controle)
{

        // Slope denominator: meta - valor, or valormax - controle when valor equals meta.
        float divisor = (meta == valor) ? (float)(valormax - controle) : (meta - valor);

        if (absfloat(divisor) < 0.00001)
                divisor = 1.0; // Avoids division by zero

        long long resultado = (long long)ceil((-valormax * (x - meta)) / divisor);

        // If the objective is to minimise, the penalty must never be negative
        // A negative result is returned as its absolute value.
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
