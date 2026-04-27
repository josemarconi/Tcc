#ifndef HILS_H
#define HILS_H

#include "hstruct.h"

// ILS Tradicional
void ILS_Normal(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem);

void ILS_Aleatorio(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem);

// Smart ILS
void Smart_ILS_Normal(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem);

void Smart_ILS_Aleatorio(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem);

void ILS_Grasp_Normal(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, 
                int **nviagem, float *QTDisp, int ncarga);

void Smart_ILS_Grasp_Aleatorio(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, 
                int **nviagem, float *QTDisp, int ncarga);
#endif