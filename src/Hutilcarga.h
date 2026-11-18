// Prototypes of the loader and truck construction, evaluation and report functions defined in Uutilcarga.cpp.
// Each comment below describes the function that follows it.
// Allocates a vector of tam struct alocarga entries; released with liberavetoralocarga.
struct alocarga *criavetoralocarga(int tam);
// Copies the loader assignment (equip and op) of tam entries from star into s.
void atualizamelhorsolucaoalocarga(struct alocarga *s, struct alocarga *star, int tam);
// Recomputes the production vector s from the visit matrix nvisitas and the truck capacities capcam.
void carregavetors(float *s, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, int ncam, int npilhas);
// Computes tciclo[i] = tdv[i] + tdc[i] + tcarga[i] + tbasc[i] for every pile.
void carregatempociclo(int npilhas, float *tciclo, float *tdv, float *tdc, float *tcarga, float *tbasc);
// Computes nummax[i] = floor(tciclo[i] / tcarga[i]) for every pile.
void carreganummax(int npilhas, int *nummax, float *tciclo, float *tcarga);
// Computes nviagem[i] = floor(tcontrole / tciclo[i]) for every pile.
void carreganviagem(int npilhas, int *nviagem, float tcontrole, float *tciclo);
// Random initial loader assignment (loader i on pile i, then random swaps), followed by a recomputation of s.
void constroisolucaoinicialcarga(struct alocarga *carga, float *s, int ncarga, int npilhas, int **nvisitas, float *capcam, int **compatibilidade, int ncam);
// Random initial visit counts nvisitas[i][j] in 0..nviagem[i][j], followed by a recomputation of s.
void constroisolucaoinicialvisitasale(float *s, struct alocarga *carga, float *capcam, int **compatibilidade, int **nvisitas, int **nviagem, int npilhas, int ncam);
// Initial visit construction: for each truck one random pile is drawn and, if it has a loader and is operational, receives nviagemax trips; s is recomputed.
void constroisolucaoinicialvisitas(float *s, struct alocarga *carga, float *capcam, int **compatibilidade, int **nvisitas, int *nviagem, int npilhas, int ncam, int viagemax);
// Weighted average grade of parameter varj over the ore piles, weighted by s; 0 when there is no ore production.
float encontrado(float *s, float **teores, int npminerio, int varj);
// Writes the report of a solution (method, run time, objective value, REM, components, tables) to the text file nomearq.
void imprimerelatorio(char nomearq[], int metodo, unsigned long seed, float tempo, float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem);
// Sets equip = valor1 and op = valor2 for the first tam entries of vetor.
void iniciavetoralocarga(struct alocarga *vetor, int tam, float valor1, bool valor2);
// Frees a vector allocated by criavetoralocarga.
void liberavetoralocarga(struct alocarga *vetor);
// Number of trucks compatible with the loader of pile pilha that visit it.
int numcamfrente(int **nvisitas, struct alocarga *carga, int **compatibilidade, int ncam, int pilha, int *nummax);
// Time-use fraction of truck cam: sum of nvisitas * tciclo over the served piles, divided by tcontrole.
float produtcam(int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int npilhas, int cam);
// Number of operational piles served by truck cam, minus one (0 if none).
int dinamico(int **nvisitas, struct alocarga *carga, int **compatibilidade, int npilhas, int cam);
// Production of pile frente: sum over compatible trucks of nvisitas * truck capacity.
float producaofrente(int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, int ncam, int frente);
// Swaps piles pilha1 and pilha2 (loader, operation flag, production s and visit counts).
void trocar(struct alocarga *carga, float *s, int pilha1, int pilha2, int **nvisitas, float *capcam, int **compatibilidade, int ncam);
// Adds or removes passo trips of truck caminhao at pile pilha if the result stays in range; *ok reports success.
void trocarnvisitas(bool *ok, int **nvisitas, int *nviagem, int passo, bool tipomov, int pilha, int caminhao);
// Toggles the operation flag of pile pilha and recomputes s.
void trocaroperacao(struct alocarga *carga, float *s, int pilha, int **nvisitas, float *capcam, int **compatibilidade, int ncam, int npilhas);
// Moves passo trips at pile pilha from truck caminhao2 to truck caminhao1.
void trocarviagemcaminhaofrente(int **nvisitas, int pilha, int caminhao1, int caminhao2, int passo);
// Moves passo trips of truck cam from pile pilha2 to pile pilha1.
void trocarviagemcaminhaocam(int **nvisitas, int cam, int pilha1, int pilha2, int passo);
// Returns 1 if truck cam makes at least one trip to an operational compatible pile, otherwise 0.
int camutilizados(int **nvisitas, struct alocarga *carga, int **compatibilidade, int npilhas, int cam);
// Visit construction variant with a budget of nviagemmax trips per truck (contains debug output in the .cpp).
void constroisolucaoinicialvisitas02(float *s, struct alocarga *carga, float *capcam, int **compatibilidade, int **nvisitas, int nviagemmax, int npilhas, int ncam);
// Greedy initial solution: loaders and visit counts, waste phase first, then ore phase.
void constroisolucaogulosa(float *s, struct alocarga *carga, float *capcam, int **compatibilidade, int **nvisitas, int **nviagem, int npilhas, int ncam, int nvar, float **teores, float *rec, int npminerio, float *QTDisp, float *cargamax, float *cargamin, int ncarga, float prodrec, float remrec);
// GRASP initial solution: randomized greedy construction; alpha sets the size of the restricted candidate list.
void constroisolucaoGRASP(float *s, struct alocarga *carga, float *capcam, int **compatibilidade,
                          int **nvisitas, int **nviagem, int npilhas, int ncam, int nvar,
                          float **teores, float *rec, int npminerio, float *QTDisp,
                          float *cargamax, float *cargamin, int ncarga, float prodrec,
                          float remrec, float alpha);
// Partial GRASP rebuild: only the ore piles when reconstruir_apenas_minerio is true, otherwise the whole solution.
void constroisolucaoGRASP_Parcial(float *s, struct alocarga *carga, float *capcam, int **compatibilidade, int **nvisitas, int **nviagem, int npilhas, int ncam, int nvar, float **teores, float *rec, int npminerio, float *QTDisp, float *cargamax, float *cargamin, int ncarga, float prodrec, float remrec, float alpha, bool reconstruir_apenas_minerio);
