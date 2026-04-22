struct alocarga *criavetoralocarga(int tam);
void atualizamelhorsolucaoalocarga(struct alocarga *s, struct alocarga *star, int tam);
void carregavetors(float *s, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, int ncam, int npilhas);
void carregatempociclo(int npilhas, float *tciclo, float *tdv, float *tdc, float *tcarga, float *tbasc);
void carreganummax(int npilhas, int *nummax, float *tciclo, float *tcarga);
void carreganviagem(int npilhas, int *nviagem, float tcontrole, float *tciclo);
void constroisolucaoinicialcarga(struct alocarga *carga, float *s, int ncarga, int npilhas, int **nvisitas, float *capcam, int **compatibilidade, int ncam);
void constroisolucaoinicialvisitasale(float *s, struct alocarga *carga, float *capcam, int **compatibilidade, int **nvisitas, int **nviagem, int npilhas, int ncam);
void constroisolucaoinicialvisitas(float *s, struct alocarga *carga, float *capcam, int **compatibilidade, int **nvisitas, int *nviagem, int npilhas, int ncam, int viagemax);
float encontrado(float *s, float **teores, int npminerio, int varj);
void imprimerelatorio(char nomearq[], int metodo, unsigned long seed, float tempo, float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem);
void iniciavetoralocarga(struct alocarga *vetor, int tam, float valor1, bool valor2);
void liberavetoralocarga(struct alocarga *vetor);
int numcamfrente(int **nvisitas, struct alocarga *carga, int **compatibilidade, int ncam, int pilha, int *nummax);
float produtcam(int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int npilhas, int cam);
int dinamico(int **nvisitas, struct alocarga *carga, int **compatibilidade, int npilhas, int cam);
float producaofrente(int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, int ncam, int frente);
void trocar(struct alocarga *carga, float *s, int pilha1, int pilha2, int **nvisitas, float *capcam, int **compatibilidade, int ncam);
void trocarnvisitas(bool *ok, int **nvisitas, int *nviagem, int passo, bool tipomov, int pilha, int caminhao);
void trocaroperacao(struct alocarga *carga, float *s, int pilha, int **nvisitas, float *capcam, int **compatibilidade, int ncam, int npilhas);
void trocarviagemcaminhaofrente(int **nvisitas, int pilha, int caminhao1, int caminhao2, int passo);
void trocarviagemcaminhaocam(int **nvisitas, int cam, int pilha1, int pilha2, int passo);
int camutilizados(int **nvisitas, struct alocarga *carga, int **compatibilidade, int npilhas, int cam);
void constroisolucaoinicialvisitas02(float *s, struct alocarga *carga, float *capcam, int **compatibilidade, int **nvisitas, int nviagemmax, int npilhas, int ncam);
void constroisolucaogulosa(float *s, struct alocarga *carga, float *capcam, int **compatibilidade, int **nvisitas, int **nviagem, int npilhas, int ncam, int nvar, float **teores, float *rec, int npminerio, float *QTDisp, float *cargamax, float *cargamin, int ncarga, float prodrec, float remrec);
void constroisolucaoGRASP(float *s, struct alocarga *carga, float *capcam, int **compatibilidade,
                          int **nvisitas, int **nviagem, int npilhas, int ncam, int nvar,
                          float **teores, float *rec, int npminerio, float *QTDisp,
                          float *cargamax, float *cargamin, int ncarga, float prodrec,
                          float remrec, float alpha);
