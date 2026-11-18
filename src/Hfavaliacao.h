// Declarations of the objective function favaliacao and its penalty helpers (implemented in Ufavaliacao.cpp).
long long favaliacao(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem);

// Penalty terms of the objective function. Each returns a long long penalty and is summed by favaliacao.
long long fa_producao(float producaomin, float prodmax, float prodmin, float prodrec, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn);
long long fa_teor(float teor, float teormax, float teormin, float teorrec, int wfteorinv, int wfteorqualp, int wfteorqualn);
long long fa_rem(float rem, float remmax, float remmin, float remrec, int wfreminv, int wfremqualp, int wfremqualn);
long long fa_produtcam(float produtcam, float produtcammax, float produtcammin, float produtcamrec, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn);
long long fa_prodcarga(float prodcarga, float prodcargamax, float prodcargamin, int wfprodcargainv, int wfprodcargaqual);
long long fa_estatico(int **nvisitas, struct alocarga *carga, int **compatibilidade, int npilhas, int ncam, int wfestatico);
long long fa_numcamfrente(int **nvisitas, struct alocarga *carga, int **compatibilidade, int npilhas, int ncam, int wfnumcam, int *nummax);
long long fa_numcam(int **nvisitas, struct alocarga *carga, int **compatibilidade, int npilhas, int ncam, int wfnumcamutilizados);

// Returns the sum of the production s[0] to s[npilhas-1].
float prodtotal(float *s, int npilhas);
// Linear penalty (see fameta in Ufavaliacao.cpp): zero at meta, valormax at valor.
long long fameta(float x, float valor, float meta, int valormax);
// Deviation penalty: ceil(|x - meta| / delta).
long long fameta2(float x, float meta, float delta);
// Linear penalty with a safeguarded slope; the result is non-negative.
long long fameta3(float x, float valor, float meta, int valormax, int controle);