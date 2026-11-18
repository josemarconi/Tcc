// Prototypes of the ILS (Iterated Local Search) and GRASP-based ILS functions implemented in Uils.cpp.
// The functions share the solution vector s, the trip and loader data, and the objective weights (wf*).
// Each one improves the solution in place. The GRASP variants also take QTDisp and ncarga.
#ifndef HILS_H
#define HILS_H

// hstruct.h defines struct alocarga (the loader assigned to each front and its operation flag).
#include "hstruct.h"

// Traditional ILS
// ILS-Referencia: classic ILS with the VND local search; the perturbation level grows after failed iterations.
void ILS_Referencia(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem);

// ILS-RLP: classic ILS with the VND-RLP local search (neighbourhood order shuffled).
void ILS_RLP(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem);

// Smart ILS
// Smart-ILS-Referencia: ILS with the VND local search; the perturbation level grows only every 5 failures.
void Smart_ILS_Referencia(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem);

// Smart-ILS-RLP: smart ILS with the VND-RLP local search.
void Smart_ILS_RLP(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem);

// ILS-GRASP-Referencia: ILS with the VND local search, whose perturbation adds a partial GRASP rebuild of ore fronts.
void ILS_GRASP_Referencia(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, 
                int **nviagem, float *QTDisp, int ncarga);

// Smart-ILS-GRASP-RLP: GRASP-ILS with the VND-RLP local search and the smart escalation rule.
void Smart_ILS_GRASP_RLP(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, 
                int **nviagem, float *QTDisp, int ncarga);

// Repeats the declaration of ILS-GRASP-Referencia given above.
void ILS_GRASP_Referencia(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem, float *QTDisp, int ncarga);

// ILS-GRASP-RLP: GRASP-ILS with the VND-RLP local search and the normal escalation rule.
void ILS_GRASP_RLP(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist ,float *dispmec,int **nviagem,float *QTDisp,int ncarga);

// Smart-ILS-GRASP-Referencia: GRASP-ILS with the VND local search and the smart escalation rule.
void Smart_ILS_GRASP_Referencia(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam,int wfdist,float *dispmec,int **nviagem,float *QTDisp,int ncarga);

// Repeats the declaration of Smart-ILS-GRASP-RLP given above.
void Smart_ILS_GRASP_RLP(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem, float *QTDisp, int ncarga);

// ILS-RLP-Parametrizado: ILS with VND-RLP. max_iter is the number of iterations and
// max_nivel the largest perturbation level.
void ILS_RLP_Parametrizado(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem, int max_iter, int max_nivel);

#endif