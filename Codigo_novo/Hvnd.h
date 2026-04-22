int VND(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
         int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
         float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
         int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
         int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
         int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
         int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
         int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem);

bool melhorVizinhoNT(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                     int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                     float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                     int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                     int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                     int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                     int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                     int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem,
                     long long *fo_atual);

bool melhorVizinhoTT(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                     int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                     float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                     int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                     int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                     int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                     int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                     int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem,
                     long long *fo_atual);

bool melhorVizinhoTP(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                     int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                     float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                     int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                     int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                     int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                     int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                     int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem,
                     long long *fo_atual);

bool melhorVizinhoST(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                     int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                     float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                     int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                     int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                     int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                     int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                     int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem,
                     long long *fo_atual);

bool melhorVizinhoL(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                    int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                    float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                    int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                    int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                    int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                    int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                    int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem,
                    long long *fo_atual);

bool melhorVizinhoPO(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                     int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                     float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                     int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                     int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                     int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                     int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                     int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem,
                     long long *fo_atual);

bool melhorVizinhoTO(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                     int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                     float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                     int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                     int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                     int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                     int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                     int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem,
                     long long *fo_atual);

bool melhorVizinhoSS(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                     int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                     float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                     int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                     int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                     int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                     int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                     int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem,
                     long long *fo_atual);

bool VND_Aleatorio(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                    int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                    float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                    int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                    int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                    int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                    int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                    int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem);
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