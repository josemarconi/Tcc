// Prototypes of the Variable Neighbourhood Descent (VND) functions defined in Uvnd.cpp.
// Solution arrays: s[f] = production of front f; nvisitas[f][c] = trips of truck c on front f;
// nviagem[f][c] = maximum trips allowed; carga[f] = loader id (equip) and operating flag (op)
// of front f; capcam[c] = capacity of truck c; compatibilidade[c][e] = compatibility of truck c
// with loader e; ncam = number of trucks; npilhas = number of fronts.
// Every other data argument is passed through to favaliacao (objective function).
// fo_atual, where present, points to the best objective, which is updated on improvement.
// VND: local search with a fixed neighbourhood order; it returns 0.
int VND_Referencia(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
         int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
         float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
         int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
         int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
         int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
         int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
         int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem);

// Neighbourhood NT (number of trips): adds or removes one trip on a (front, truck) pair.
// Returns true at the first improving move and updates *fo_atual.
bool melhorVizinhoNT(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                     int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                     float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                     int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                     int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                     int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                     int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                     int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem,
                     long long *fo_atual);

// Neighbourhood TT (truck trip relocation): moves one trip of a truck from one front to another.
// Returns true at the first improving move and updates *fo_atual.
bool melhorVizinhoTT(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                     int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                     float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                     int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                     int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                     int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                     int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                     int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem,
                     long long *fo_atual);

// Neighbourhood TP (trip transfer inside a front): moves one trip from one truck to another
// truck on the same front. Same return convention as NT.
bool melhorVizinhoTP(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                     int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                     float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                     int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                     int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                     int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                     int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                     int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem,
                     long long *fo_atual);

// Neighbourhood ST (trip swap): exchanges one trip between two (front, truck) cells.
// Same return convention as NT.
bool melhorVizinhoST(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                     int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                     float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                     int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                     int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                     int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                     int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                     int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem,
                     long long *fo_atual);

// Neighbourhood L (loader swap): exchanges the loaders of two fronts together with their trips.
// Same return convention as NT.
bool melhorVizinhoL(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                    int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                    float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                    int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                    int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                    int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                    int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                    int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem,
                    long long *fo_atual);

// Neighbourhood PO (pit operation): switches a front on or off.
// Same return convention as NT.
bool melhorVizinhoPO(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                     int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                     float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                     int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                     int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                     int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                     int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                     int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem,
                     long long *fo_atual);

// Neighbourhood TO (truck operation): removes all trips of one truck from one front.
// Same return convention as NT.
bool melhorVizinhoTO(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                     int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                     float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                     int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                     int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                     int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                     int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                     int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem,
                     long long *fo_atual);

// Neighbourhood SS (swap shovels): exchanges the loaders of two fronts and removes the trips that
// became incompatible. Same return convention as NT.
bool melhorVizinhoSS(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                     int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                     float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                     int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                     int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                     int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                     int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                     int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem,
                     long long *fo_atual);

// VND-RLP: VND driver with a random neighbourhood order and adaptive weights.
// Returns true if at least one improvement was made.
bool VND_RLP(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                    int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                    float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                    int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                    int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                    int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                    int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                    int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem);
// The block below holds commented-out, inactive prototypes of older VND variants (with other
// signatures, e.g. int *nviagem). They are not compiled and no definitions exist in src/.
/*
int VND(float *s,float *li,float *ls,float *rec,int *penalidadesup
            ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
            ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
            ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
            ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
            ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
            , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem);
 
 
 int VNDvC(float *s,float *li,float *ls,float *rec,int *penalidadesup
            ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
            ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
            ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
            ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
            ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
            , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem);


 int VNDvALE1(float *s,float *li,float *ls,float *rec,int *penalidadesup
            ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
            ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
            ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
            ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
            ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
            , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem);


 int VNDvALEcMem(float *s,float *li,float *ls,float *rec,int *penalidadesup
            ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
            ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
            ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
            ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
            ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
            , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem);
            
     int VNDvAdaptativo(float *s,float *li,float *ls,float *rec,int *penalidadesup
            ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
            ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
            ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
            ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
            ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
            , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem);
*/