// Prototypes of the neighbourhood moves implemented in src/Umovimento.cpp. Each move changes
// the solution arrays (nvisitas, carga, s) in place. The local searches call these functions.
// movimentoNumberTrips: adds or removes one trip of a random truck in a random front.
void movimentoNumberTrips(int **nvisitas, int **nviagem, float *s, float *capcam, int npilhas, int ncam, struct alocarga *carga);
// movimentoLoad: exchanges the loaders and the truck-trip rows of two random fronts.
void movimentoLoad(struct alocarga *carga, int npilhas, int ncam, int **compatibilidade, int **nvisitas, float *s, float *capcam, int **nviagem);
// movimentoRelocateTripTruck: moves one trip of truck c from front f1 to front f2.
void movimentoRelocateTripTruck(int **nvisitas, int **nviagem, int f1, int f2, int c, float *s, float *capcam, int **compatibilidade, struct alocarga *carga);
// movimentoRelocateTripPit: moves one trip from truck c1 to truck c2 inside front f.
void movimentoRelocateTripPit(int **nvisitas, int **nviagem, int f, int c1, int c2, float *s, float *capcam, int **compatibilidade, struct alocarga *carga);
// movimentoPitOperation: toggles the active flag of front f (switching off clears its trips).
void movimentoPitOperation(struct alocarga *carga, int f, int **nvisitas, float *s, int ncam);
// movimentoTruckOperation: removes all trips of truck c_id from front f.
void movimentoTruckOperation(int **nvisitas, int f, int c_id, float *s, float *capcam);
// movimentoSwapTrips: moves one trip from truck c1 in front f1 to truck c2 in front f2.
void movimentoSwapTrips(int **nvisitas, int f1, int f2, int c1, int c2, float *s, float *capcam, int **compatibilidade, struct alocarga *carga, int **nviagem);
// movimentoSwapShovels: exchanges the loaders of fronts f1 and f2 and clears incompatible trips.
void movimentoSwapShovels(struct alocarga *carga, int f1, int f2, int **nvisitas, int ncam, int **compatibilidade, float *s, float *capcam);

// From this line to the closing comment marker at the end of the file, the prototypes are
// commented out and not compiled. They declare older move functions and the melhorvizinho*
// best-neighbour searches.
/*
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
*/
