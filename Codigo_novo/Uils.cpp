#include "Hils.h"
#include "Hvnd.h"
#include "Hfavaliacao.h"
#include "Hutil.h"
#include "Hmovimento.h"
#include "Hutilcarga.h"
#include <string.h>
#include <stdlib.h>

/*
 * PERTURBAÇÃO
 */
void realizar_perturbacao_inteligente(int **nvisitas, int **nviagem, float *s, float *capcam, int npilhas, int ncam, 
                                      struct alocarga *carga, int **compatibilidade, int nivel) {
    
    // O 'nível' define a intensidade da perturbação (quantos movimentos faremos)
    for (int i = 0; i < nivel; i++) {
        
        // Sorteamos aleatoriamente um dos 8 movimentos desenvolvidos no projeto
        int mov = rand() % 8; 

        // Geramos índices aleatórios de frentes e caminhões para os movimentos que precisam de alvos
        int f1 = rand() % npilhas;
        int f2 = rand() % npilhas;
        int c1 = rand() % ncam;
        int c2 = rand() % ncam;\

        switch(mov) {
            case 0: // Altera a quantidade de viagens em uma rota (Leve)
                movimentoNumberTrips(nvisitas, nviagem, s, capcam, npilhas, ncam, carga); 
                break;
            case 1: // Move as viagens de um caminhão de uma frente para outra
                movimentoRelocateTripTruck(nvisitas, nviagem, f1, f2, c1, s, capcam, compatibilidade, carga);
                break;
            case 2: // Muda o destino de um caminhão entre frentes
                movimentoRelocateTripPit(nvisitas, nviagem, f1, c1, c2, s, capcam, compatibilidade, carga);
                break;
            case 3: // Troca a carregadeira de uma frente (Forte/Impactante)
                movimentoLoad(carga, npilhas, ncam, compatibilidade, nvisitas, s, capcam, nviagem);
                break;
            case 4: // Altera o status de operação de uma frente
                movimentoPitOperation(carga, f1, nvisitas, s, ncam);
                break;
            case 5: // Altera o status de operação de um caminhão
                movimentoTruckOperation(nvisitas, f1, c1, s, capcam);
                break;
            case 6: // Troca as rotas entre dois caminhões e duas frentes (Complexo)
                movimentoSwapTrips(nvisitas, f1, f2, c1, c2, s, capcam, compatibilidade, carga, nviagem);
                break;
            case 7: // Inverte as carregadeiras entre duas frentes (Reestruturação total)
                movimentoSwapShovels(carga, f1, f2, nvisitas, ncam, compatibilidade, s, capcam);
                break;
        }
    }
}

/**
 * MACRO MESTRE DO ILS (Iterated Local Search)
 * A macro permite criar as 4 variações mudando apenas o tipo de VND e a lógica de falhas.
 */
#define EXECUTAR_CORE_ILS(VND_FUNC, SMART_LOGIC) \
    /* 1. Alocação de memória para salvar a 'Melhor Solução de Elite' encontrada */ \
    float *s_best = criavetorfloat(npilhas); \
    int **nvis_best = criamatriz(npilhas, ncam); \
    struct alocarga *carga_best = criavetoralocarga(npilhas); \
    \
    /* 2. Avaliação da Solução Inicial (geralmente vinda do GRASP) */ \
    long long fo_melhor = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem); \
    \
    /* Faz o backup inicial */ \
    atualizavetorfloat(s_best, s, npilhas); \
    atualizamatrizint(nvis_best, nvisitas, npilhas, ncam); \
    atualizamelhorsolucaoalocarga(carga_best, carga, npilhas); \
    \
    int nivel = 1;  /* Força da perturbação */ \
    int falhas = 0; /* Contador para a lógica Smart */ \
    \
    /* Loop principal do ILS (100 iterações de Perturbação + Busca Local) */ \
    for (int iter = 0; iter < 100; iter++) { \
        \
        /* FASE A: Perturbação - Dá o "empurrão" na solução atual */ \
        realizar_perturbacao_inteligente(nvisitas, nviagem, s, capcam, npilhas, ncam, carga, compatibilidade, nivel); \
        \
        /* FASE B: Busca Local (VND) - Refina a solução perturbada até o ótimo local */ \
        VND_FUNC(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem); \
        \
        /* FASE C: Avaliação e Aceitação */ \
        long long fo_nova = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem); \
        \
        if (fo_nova < fo_melhor) { \
            /* Se melhorou: Salva como nova solução de elite e reseta a força */ \
            fo_melhor = fo_nova; \
            atualizavetorfloat(s_best, s, npilhas); \
            atualizamatrizint(nvis_best, nvisitas, npilhas, ncam); \
            atualizamelhorsolucaoalocarga(carga_best, carga, npilhas); \
            nivel = 1; \
            falhas = 0; \
        } else { \
            /* Se não melhorou: Incrementa falhas e ajusta o nível de perturbação */ \
            falhas++; \
            if (SMART_LOGIC) { \
                /* Lógica SMART: Só aumenta o nível de perturbação a cada 5 falhas acumuladas */ \
                if (falhas % 5 == 0) nivel++; \
            } else { \
                /* Lógica NORMAL: Aumenta a perturbação imediatamente a cada erro */ \
                nivel++; \
            } \
            \
            /* Limite de segurança para não destruir a solução (reset do nível) */ \
            if (nivel > 8) nivel = 1; \
            \
            /* Rollback: Volta a solução para o melhor estado conhecido antes de tentar nova perturbação */ \
            atualizavetorfloat(s, s_best, npilhas); \
            atualizamatrizint(nvisitas, nvis_best, npilhas, ncam); \
            atualizamelhorsolucaoalocarga(carga, carga_best, npilhas); \
        } \
    } \
    /* Fim do loop: Garante que a solução final retornada seja a melhor de todas as iterações */ \
    atualizavetorfloat(s, s_best, npilhas); \
    atualizamatrizint(nvisitas, nvis_best, npilhas, ncam); \
    atualizamelhorsolucaoalocarga(carga, carga_best, npilhas); \
    \
    /* Limpeza de memória */ \
    liberavetorfloat(s_best); \
    liberamatriz(nvis_best, npilhas); \
    free(carga_best);

// --- IMPLEMENTAÇÃO DAS 4 VARIANTES ---

// 1. ILS Clássico: Usa o VND fixo e aumenta o nível de perturbação a cada falha.
void ILS_Normal(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem) {
    EXECUTAR_CORE_ILS(VND, false)
}

// 2. ILS Aleatório: Usa o VND com ordem de vizinhanças aleatória e pesos de aprendizado.
void ILS_Aleatorio(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem) {
    EXECUTAR_CORE_ILS(VND_Aleatorio, false)
}

// 3. Smart ILS: VND Normal, mas demora mais para aumentar a agressividade da perturbação.
void Smart_ILS_Normal(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem) {
    EXECUTAR_CORE_ILS(VND, true)
}

// 4. Smart ILS Aleatório: O topo da inteligência do projeto, unindo VND Aleatório com Perturbação Smart.
void Smart_ILS_Aleatorio(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem) {
    EXECUTAR_CORE_ILS(VND_Aleatorio, true)
}




// ILS - GRASP: Variante que integra o ILS com o GRASP, usando a construção parcial do GRASP como perturbação. // 

#define EXECUTAR_ILS_GRASP(VND_FUNC, SMART_LOGIC) \
    float *s_best = criavetorfloat(npilhas); \
    int **nvis_best = criamatriz(npilhas, ncam); \
    struct alocarga *carga_best = criavetoralocarga(npilhas); \
    \
    long long fo_melhor = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem); \
    \
    atualizavetorfloat(s_best, s, npilhas); \
    atualizamatrizint(nvis_best, nvisitas, npilhas, ncam); \
    atualizamelhorsolucaoalocarga(carga_best, carga, npilhas); \
    \
    float alpha_p = 0.2; \
    int falhas = 0; \
    \
    for (int iter = 0; iter < 100; iter++) { \
        /* PERTURBAÇÃO: Reconstrução Parcial apenas do Minério */ \
        constroisolucaoGRASP_Parcial(s, carga, capcam, compatibilidade, nvisitas, nviagem, npilhas, ncam, nvar, teores, rec, npminerio, QTDisp, cargamax, cargamin, ncarga, prodrec, remrec, alpha_p, true); \
        \
        /* BUSCA LOCAL */ \
        VND_FUNC(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem); \
        \
        long long fo_nova = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem); \
        \
        if (fo_nova < fo_melhor) { \
            fo_melhor = fo_nova; \
            atualizavetorfloat(s_best, s, npilhas); \
            atualizamatrizint(nvis_best, nvisitas, npilhas, ncam); \
            atualizamelhorsolucaoalocarga(carga_best, carga, npilhas); \
            alpha_p = 0.2; falhas = 0; \
        } else { \
            falhas++; \
            bool deve_aumentar = (SMART_LOGIC) ? (falhas % 10 == 0) : true; \
            if (deve_aumentar) { \
                alpha_p += 0.2; \
                if (alpha_p > 1.01) alpha_p = 0.2; \
            } \
            atualizavetorfloat(s, s_best, npilhas); \
            atualizamatrizint(nvisitas, nvis_best, npilhas, ncam); \
            atualizamelhorsolucaoalocarga(carga, carga_best, npilhas); \
        } \
    } \
    liberavetorfloat(s_best); liberamatriz(nvis_best, npilhas); free(carga_best);

void ILS_Grasp_Normal(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem, float *QTDisp, int ncarga) {
    EXECUTAR_ILS_GRASP(VND, false)
}

void ILS_Grasp_Aleatorio(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist ,float *dispmec,int **nviagem,float *QTDisp,int ncarga) {
    EXECUTAR_ILS_GRASP(VND_Aleatorio,false)
}

void Smart_ILS_Grasp_Normal(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam,int wfdist,float *dispmec,int **nviagem,float *QTDisp,int ncarga) {
    EXECUTAR_ILS_GRASP(VND,true)
}

void Smart_ILS_Grasp_Aleatorio(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem, float *QTDisp, int ncarga) {
    EXECUTAR_ILS_GRASP(VND_Aleatorio, true)
}