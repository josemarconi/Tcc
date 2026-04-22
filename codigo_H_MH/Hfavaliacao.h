int favaliacao(float *s,float *li,float *ls,float *rec,int *penalidadesup,int
        *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
        ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
        ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
        ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
        ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
        , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem);
float prodtotal(float *s, int npilhas);
int fa_producao(float producaomin, float prodmax, float prodmin, float prodrec, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn);
int fa_teor(float teor, float teormax, float teormin, float teorrec, int wfteorinv, int wfteorqualp, int wfteorqualn);
int fa_rem(float rem, float remmax, float remmin, float remrec, int wfreminv, int wfremqualp, int wfremqualn);
int fa_produtcam(float produtcam, float produtcammax, float produtcammin, float produtcamrec, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn);
int fa_prodcarga(float prodcarga, float prodcargamax, float prodcargamin, int wfprodcargainv, int wfprodcargaqual);
int fa_estatico(int **nvisitas, struct alocarga *carga, int **compatibilidade, int npilhas,int ncam, int wfestatico);
int fameta(float x, float valor, float meta, int valormax);
int fa_numcamfrente(int **nvisitas, struct alocarga *carga, int **compatibilidade, int npilhas,int ncam, int wfnumcam, int *nummax);
int fa_numcam(int **nvisitas, struct alocarga *carga, int **compatibilidade, int npilhas,int ncam, int wfnumcamutilizados);
int fameta2(float x,float meta, float delta);




 