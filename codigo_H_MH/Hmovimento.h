void movimentocarga(struct alocarga *carga,float *s,int npilhas,int *pilha1, int *pilha2, int **nvisitas, float *capcam,int **compatibilidade,int ncam);
void movimentovisita(int **nvisitas,int **compatibilidade,struct alocarga *carga, int passo, int npilhas,int ncam, int *nviagem, int *pilha,int *caminhao, bool *tipomov,float *s, float *capcam);
void movimentooperacao(struct alocarga *carga,float *s,int npilhas,int *pilha1, bool *tipomov, int **nvisitas, float *capcam,int **compatibilidade,int ncam);
void movimentoviagemfrente(int **nvisitas,int **compatibilidade,struct alocarga *carga, int passo, int npilhas,int ncam, int *nviagem, int *pilha,int *caminhao1,int *caminhao2,float *s, float *capcam);
void movimentoviagemcam(int **nvisitas,int **compatibilidade,struct alocarga *carga, int passo, int npilhas,int ncam, int *nviagem, int *cam,int *pilha1,int *pilha2,float *s, float *capcam);
void movimentooperacaocam(int **nvisitas,int **compatibilidade,struct alocarga *carga, int passo, int npilhas,int ncam, int *nviagem, int *cam,int *pilha,float *s, float *capcam);

int melhorvizinhonvisitas(float *s,float *li,float *ls,float *rec,int *penalidadesup
                             ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
                             ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
                             ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
                             ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
                             ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
                             , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem);
int melhorvizinhogeral(float *s,float *li,float *ls,float *rec,int *penalidadesup
                             ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
                             ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
                             ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
                             ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
                             ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
                             , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem);

int melhorvizinhocarga(float *s,float *li,float *ls,float *rec,int *penalidadesup
                             ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
                             ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
                             ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
                             ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
                             ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
                             , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem);
int melhorvizinhooperacao(float *s,float *li,float *ls,float *rec,int *penalidadesup
                             ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
                             ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
                             ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
                             ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
                             ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
                             , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem);

int melhorvizinhonviagemfrente(float *s,float *li,float *ls,float *rec,int *penalidadesup
                             ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
                             ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
                             ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
                             ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
                             ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
                             , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem);

int melhorvizinhonviagemcam(float *s,float *li,float *ls,float *rec,int *penalidadesup
                             ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
                             ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
                             ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
                             ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
                             ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
                             , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem);

int melhorvizinhooperacaocam(float *s,float *li,float *ls,float *rec,int *penalidadesup
                             ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
                             ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
                             ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
                             ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
                             ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
                             , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem);

