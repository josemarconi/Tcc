// Header of the Variable Neighbourhood Search (VNS) functions defined in Uvns.cpp.
// The include guard HVNS_H prevents the file from being included twice.
#ifndef HVNS_H
#define HVNS_H

// hstruct.h defines struct alocarga (loader id and operating flag of a front).
#include "hstruct.h"

// VNS: Variable Neighbourhood Search on the solution arrays, modified in place.
// forma selects the local search: 1 = VND, 2 = VND-RLP. Returns the number of outer iterations.
int VNS(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv,
        int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio,
        float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam,
        int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin,
        int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem,
        int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn,
        int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga,
        int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem, int forma);

// VNS_Parametrizado: VNS with the local search fixed to VND-RLP and the limits given by
// the caller: max_iter_vns (outer iterations) and max_k (maximum shaking strength). Returns nothing.
void VNS_Parametrizado(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv,
         int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio,
         float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam,
         int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin,
         int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem,
         int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn,
         int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga,
         int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem, 
         int max_iter_vns, int max_k);

#endif