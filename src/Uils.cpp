// Iterated Local Search (ILS) and GRASP-based ILS variants for the loading and haulage plan.
// Classic variants: ILS-Referencia, ILS-RLP, Smart-ILS-Referencia, Smart-ILS-RLP.
// GRASP variants add a partial GRASP rebuild of ore fronts as an extra perturbation.
// ILS-RLP-Parametrizado is a separate ILS with configurable iteration and level limits.
// Prototypes are in Hils.h.
#include "Hils.h"
#include "Hvnd.h"
#include "Hfavaliacao.h"
#include "Hutil.h"
#include "Hmovimento.h"
#include "Hutilcarga.h"
// Standard C library: rand, malloc, calloc and free (stdlib.h) and ceil (math.h).
#include <string.h>
#include <stdlib.h>
#include <math.h>

/*
 * PERTURBATION
 */
// Applies nivel random moves to the current solution (the ILS perturbation).
// The arrays s, nvisitas, nviagem and carga are changed in place; nothing is returned.
void realizar_perturbacao_inteligente(int **nvisitas, int **nviagem, float *s, float *capcam, int npilhas, int ncam, 
                                      struct alocarga *carga, int **compatibilidade, int nivel) {
    
    // The 'level' sets the perturbation strength (how many moves we make)
    // nivel is the perturbation strength: this many random moves are applied, one per loop iteration.
    for (int i = 0; i < nivel; i++) {
        
        // Randomly draw one of the 8 moves developed in the project
        int mov = rand() % 8; 

        // Generate random front and truck indices for the moves that need targets
        // Random front indices f1, f2 and truck indices c1, c2. They are drawn for every move,
        // and only the moves that need targets use them.
        int f1 = rand() % npilhas;
        int f2 = rand() % npilhas;
        int c1 = rand() % ncam;
        int c2 = rand() % ncam;\

        // Applies the move selected by mov (each case calls one of the 8 neighbourhood moves).
        switch(mov) {
            case 0: // Change the number of trips on a route (Light)
                movimentoNumberTrips(nvisitas, nviagem, s, capcam, npilhas, ncam, carga); 
                break;
            case 1: // Move a truck's trips from one front to another
                movimentoRelocateTripTruck(nvisitas, nviagem, f1, f2, c1, s, capcam, compatibilidade, carga);
                break;
            case 2: // Change a truck's destination between fronts
                movimentoRelocateTripPit(nvisitas, nviagem, f1, c1, c2, s, capcam, compatibilidade, carga);
                break;
            case 3: // Swap the loader of a front (Strong/Disruptive)
                movimentoLoad(carga, npilhas, ncam, compatibilidade, nvisitas, s, capcam, nviagem);
                break;
            case 4: // Change the operation status of a front
                movimentoPitOperation(carga, f1, nvisitas, s, ncam);
                break;
            case 5: // Change the operation status of a truck
                movimentoTruckOperation(nvisitas, f1, c1, s, capcam);
                break;
            case 6: // Swap the routes between two trucks and two fronts (Complex)
                movimentoSwapTrips(nvisitas, f1, f2, c1, c2, s, capcam, compatibilidade, carga, nviagem);
                break;
            case 7: // Swap the loaders between two fronts (Full restructuring)
                movimentoSwapShovels(carga, f1, f2, nvisitas, ncam, compatibilidade, s, capcam);
                break;
        }
    }
}

/**
 * MASTER MACRO OF THE ILS (Iterated Local Search)
 * The macro creates the 4 variants by changing only the type of VND and the failure logic.
 */
// EXECUTAR_CORE_ILS: macro holding the body of the classic ILS loop, used by the four classic ILS functions.
// VND_FUNC is the local search (VND or VND-RLP). SMART_LOGIC selects the escalation rule:
// true raises the perturbation level every 5 failures, false raises it at every failure.
// It runs 100 iterations and frees its temporary arrays at the end.
#define EXECUTAR_CORE_ILS(VND_FUNC, SMART_LOGIC) \
    /* 1. Memory allocation to save the 'Best Elite Solution' found */ \
    float *s_best = criavetorfloat(npilhas); \
    int **nvis_best = criamatriz(npilhas, ncam); \
    struct alocarga *carga_best = criavetoralocarga(npilhas); \
    \
    /* 2. Evaluation of the initial solution (usually coming from GRASP) */ \
    long long fo_melhor = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem); \
    \
    /* Make the initial backup */ \
    atualizavetorfloat(s_best, s, npilhas); \
    atualizamatrizint(nvis_best, nvisitas, npilhas, ncam); \
    atualizamelhorsolucaoalocarga(carga_best, carga, npilhas); \
    \
    int nivel = 1;  /* Perturbation strength */ \
    int falhas = 0; /* Counter for the Smart logic */ \
    \
    /* Main ILS loop (100 iterations of Perturbation + Local Search) */ \
    for (int iter = 0; iter < 100; iter++) { \
        \
        /* PHASE A: Perturbation - gives a "push" to the current solution */ \
        realizar_perturbacao_inteligente(nvisitas, nviagem, s, capcam, npilhas, ncam, carga, compatibilidade, nivel); \
        \
        /* PHASE B: Local Search (VND) - refines the perturbed solution up to a local optimum */ \
        VND_FUNC(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem); \
        \
        /* PHASE C: Evaluation and Acceptance */ \
        long long fo_nova = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem); \
        \
        if (fo_nova < fo_melhor) { \
            /* If improved: save as the new elite solution and reset the strength */ \
            fo_melhor = fo_nova; \
            atualizavetorfloat(s_best, s, npilhas); \
            atualizamatrizint(nvis_best, nvisitas, npilhas, ncam); \
            atualizamelhorsolucaoalocarga(carga_best, carga, npilhas); \
            nivel = 1; \
            falhas = 0; \
        } else { \
            /* If not improved: increment failures and adjust the perturbation level */ \
            falhas++; \
            if (SMART_LOGIC) { \
                /* SMART logic: only raise the perturbation level every 5 accumulated failures */ \
                if (falhas % 5 == 0) nivel++; \
            } else { \
                /* Non-smart logic: raise the perturbation immediately at every failure */ \
                nivel++; \
            } \
            \
            /* Safety limit so the solution is not destroyed (level reset) */ \
            if (nivel > 8) nivel = 1; \
            \
            /* Rollback: restore the solution to the best known state before trying a new perturbation */ \
            atualizavetorfloat(s, s_best, npilhas); \
            atualizamatrizint(nvisitas, nvis_best, npilhas, ncam); \
            atualizamelhorsolucaoalocarga(carga, carga_best, npilhas); \
        } \
    } \
    /* End of loop: ensures the final returned solution is the best of all iterations */ \
    atualizavetorfloat(s, s_best, npilhas); \
    atualizamatrizint(nvisitas, nvis_best, npilhas, ncam); \
    atualizamelhorsolucaoalocarga(carga, carga_best, npilhas); \
    \
    /* Memory cleanup */ \
    liberavetorfloat(s_best); \
    liberamatriz(nvis_best, npilhas); \
    free(carga_best);

// --- IMPLEMENTATION OF THE 4 VARIANTS ---

// 1. Classic ILS (ILS-Referencia): uses the fixed-order VND (VND-Referencia) and raises the perturbation level at every failure.
// ILS-Referencia: EXECUTAR_CORE_ILS with the VND local search and SMART_LOGIC = false.
void ILS_Referencia(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem) {
    EXECUTAR_CORE_ILS(VND_Referencia, false)
}

// 2. Random ILS (ILS-RLP): uses the VND with random neighbourhood order (VND-RLP) and learning weights.
// ILS-RLP: EXECUTAR_CORE_ILS with the VND-RLP local search and SMART_LOGIC = false.
void ILS_RLP(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem) {
    EXECUTAR_CORE_ILS(VND_RLP, false)
}

// 3. Smart ILS: VND-Referencia, but takes longer to increase the aggressiveness of the perturbation.
// Smart-ILS-Referencia: EXECUTAR_CORE_ILS with the VND local search and SMART_LOGIC = true.
void Smart_ILS_Referencia(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem) {
    EXECUTAR_CORE_ILS(VND_Referencia, true)
}

// 4. Smart random ILS: the most advanced variant of the project, combining VND-RLP with the Smart perturbation.
// Smart-ILS-RLP: EXECUTAR_CORE_ILS with the VND-RLP local search and SMART_LOGIC = true.
void Smart_ILS_RLP(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem) {
    EXECUTAR_CORE_ILS(VND_RLP, true)
}

// --- NEW PARTIAL GRASP PERTURBATION FUNCTION (10% to 20%) ---
// perturbacao_micro_GRASP: destroys between 10% and 20% of the ore fronts (at least one, chosen at random)
// and rebuilds them with a GRASP construction. The candidate list has relative size alpha_p and the
// rebuild stops once the surviving ore production reaches prodrec.
// Updates s, nvisitas and carga in place; nviagem is only read.
void perturbacao_micro_GRASP(int **nvisitas, int **nviagem, float *s, float *capcam, int npilhas, int ncam, 
                             struct alocarga *carga, int **compatibilidade, float **teores, float *rec, 
                             int npminerio, float *QTDisp, float *cargamax, int ncarga, int nvar, 
                             float prodrec, float alpha_p) 
{
    // 1. Define the destruction rate: 10% to 20% of the ore fronts
    int qtd_matar = ceil(npminerio * (rand() % 11 + 10) / 100.0);
    if(qtd_matar < 1) qtd_matar = 1;
    if(qtd_matar > npminerio) qtd_matar = npminerio;

    // Flags the ore fronts destroyed in this call; the loop below picks distinct fronts.
    bool *destruido = (bool*)calloc(npminerio, sizeof(bool));
    int mortos = 0;
    
    // 2. Destroy the randomly drawn fronts
    while(mortos < qtd_matar) {
        int f = rand() % npminerio;
        if(!destruido[f]) {
            destruido[f] = true;
            carga[f].equip = -1;
            carga[f].op = false;
            s[f] = 0.0;
            for(int c = 0; c < ncam; c++) nvisitas[f][c] = 0;
            mortos++;
        }
    }

    // 3. Map the loaders still in use (on sterile and on intact ore fronts)
    // Loaders still assigned to surviving fronts (sterile fronts, or ore fronts not destroyed).
    // These loaders are not free for the rebuild.
    bool *loader_used = (bool*)calloc(ncarga, sizeof(bool));
    for(int i = 0; i < npilhas; i++) {
        if((i >= npminerio || !destruido[i]) && carga[i].equip != -1) {
            loader_used[carga[i].equip] = true;
        }
    }

    // 4. Compute the production of the ore that survived
    // Production of the ore fronts that survived the destruction; the rebuild stops when it reaches prodrec.
    float prod_min_atual = 0.0;
    for(int i = 0; i < npminerio; i++) {
        if(!destruido[i]) prod_min_atual += s[i];
    }

    // 5. Sort the destroyed fronts by grade deviation (Greedy)
    // Destroyed fronts and their grade deviation: sum over the variables of |teor - rec|.
    int *ore_faces = (int*)malloc(mortos * sizeof(int));
    float *deviations = (float*)malloc(mortos * sizeof(float));
    int idx = 0;
    for(int i = 0; i < npminerio; i++) {
        if(destruido[i]) {
            ore_faces[idx] = i;
            deviations[idx] = 0;
            for(int j = 0; j < nvar; j++) deviations[idx] += absfloat(teores[i][j] - rec[j]);
            idx++;
        }
    }

    // Exchange sort of the destroyed fronts by deviation, ascending (most similar grade first).
    for(int i = 0; i < mortos - 1; i++) {
        for(int j = i + 1; j < mortos; j++) {
            if(deviations[i] > deviations[j]) {
                float td = deviations[i]; deviations[i] = deviations[j]; deviations[j] = td;
                int tf = ore_faces[i]; ore_faces[i] = ore_faces[j]; ore_faces[j] = tf;
            }
        }
    }

    // 6. Sort loaders and fleet
    // Loaders sorted by cargamax, ascending. The rebuild takes the largest free loader first.
    int *sorted_loaders = (int*)malloc(ncarga * sizeof(int));
    for(int i = 0; i < ncarga; i++) sorted_loaders[i] = i;
    for(int i = 0; i < ncarga - 1; i++) {
        for(int j = i + 1; j < ncarga; j++) {
            if(cargamax[sorted_loaders[i]] > cargamax[sorted_loaders[j]]) {
                int temp = sorted_loaders[i]; sorted_loaders[i] = sorted_loaders[j]; sorted_loaders[j] = temp;
            }
        }
    }

    // Trucks sorted by capcam, descending (largest capacity first).
    int *sorted_trucks = (int*)malloc(ncam * sizeof(int));
    for(int i = 0; i < ncam; i++) sorted_trucks[i] = i;
    for(int i = 0; i < ncam - 1; i++) {
        for(int j = i + 1; j < ncam; j++) {
            if(capcam[sorted_trucks[i]] < capcam[sorted_trucks[j]]) {
                int temp = sorted_trucks[i]; sorted_trucks[i] = sorted_trucks[j]; sorted_trucks[j] = temp;
            }
        }
    }

    // 7. Rebuild the destroyed fronts using GRASP (RCL based on alpha_p)
    // Index of the next candidate loader in sorted_loaders. It starts at the largest loader and
    // moves down past loaders already in use.
    int l_idx = ncarga - 1;
    // Rebuilds the destroyed fronts one at a time. Each step draws a front from a restricted candidate
    // list (RCL): the first ceil(remaining * alpha_p) fronts of the deviation-ordered list.
    for(int i = 0; i < mortos; i++) {
        // Stops when the surviving plus rebuilt production reaches the target.
        if (prod_min_atual >= prodrec) break;

        int restante = mortos - i;
        int range_lrc = (int)ceil(restante * alpha_p);
        if (range_lrc < 1) range_lrc = 1;
        int escolhido_idx = i + (rand() % range_lrc);

        int f = ore_faces[escolhido_idx];
        // Moves the chosen front to position i so the remaining candidates stay contiguous.
        int temp_f = ore_faces[i]; ore_faces[i] = ore_faces[escolhido_idx]; ore_faces[escolhido_idx] = temp_f;

        // Takes the largest loader that is not in use. If none is free, the front stays unassigned.
        while(l_idx >= 0 && loader_used[sorted_loaders[l_idx]]) l_idx--;
        if(l_idx >= 0) {
            int l_id = sorted_loaders[l_idx];
            carga[f].equip = l_id;
            carga[f].op = true;
            loader_used[l_id] = true;

            // Assigns trips to the front from trucks in ascending capacity order, using only trucks
            // compatible with the chosen loader.
            for(int j = ncam - 1; j >= 0; j--) {
                int c = sorted_trucks[j];
                if(compatibilidade[c][l_id] == 1) {
                    // Production still missing to reach prodrec. No more trips are added once it is covered.
                    float falta = prodrec - prod_min_atual;
                    if (falta <= 0) break;

                    // Trips for (f, c) start at the limit nviagem[f][c] and are capped below.
                    int v = nviagem[f][c];
                    // Caps the trips by the front quantity QTDisp[f] that is still available.
                    if(s[f] + v * capcam[c] > QTDisp[f]) v = (int)((QTDisp[f] - s[f]) / capcam[c]);
                    // Caps the trips so the added production does not exceed what is missing (rounded up to whole trips).
                    if(v * capcam[c] > falta) v = (int)ceil(falta / capcam[c]);

                    // Records the trips and updates the front production and the surviving ore production.
                    nvisitas[f][c] = v;
                    s[f] += v * capcam[c];
                    prod_min_atual += v * capcam[c];
                }
            }
        }
    }

    // Releases the temporary arrays used by the rebuild.
    free(destruido); free(loader_used); free(ore_faces); free(deviations); free(sorted_loaders); free(sorted_trucks);
}

// ILS - GRASP: variant that integrates the ILS with GRASP, using the partial GRASP construction as perturbation. // 
// EXECUTAR_ILS_GRASP: macro holding the body of the GRASP-ILS loop, used by the four GRASP-ILS functions.
// VND_FUNC is the local search and SMART_LOGIC the escalation rule. Each of the 100 iterations applies
// realizar_perturbacao_inteligente, then perturbacao_micro_GRASP with alpha_p, then the local search.
// The new solution is kept only if its objective improves; otherwise the best solution is restored.
// On failure alpha_p grows by 0.2 (wraps to 0.2 above 1.01) and nivel_movimentos grows (wraps to 1 above 4).
// Both are reset on improvement.
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
    int nivel_movimentos = 1; \
    \
    for (int iter = 0; iter < 100; iter++) { \
        \
        /* 1. HYBRID PERTURBATION: mixes smart moves with micro-reconstruction */ \
        realizar_perturbacao_inteligente(nvisitas, nviagem, s, capcam, npilhas, ncam, carga, compatibilidade, nivel_movimentos); \
        perturbacao_micro_GRASP(nvisitas, nviagem, s, capcam, npilhas, ncam, carga, compatibilidade, teores, rec, npminerio, QTDisp, cargamax, ncarga, nvar, prodrec * horasDoHorizonte(tcontrole), alpha_p); \
        \
        /* 2. LOCAL SEARCH */ \
        VND_FUNC(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem); \
        \
        long long fo_nova = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem); \
        \
        if (fo_nova < fo_melhor) { \
            fo_melhor = fo_nova; \
            atualizavetorfloat(s_best, s, npilhas); \
            atualizamatrizint(nvis_best, nvisitas, npilhas, ncam); \
            atualizamelhorsolucaoalocarga(carga_best, carga, npilhas); \
            alpha_p = 0.2; falhas = 0; nivel_movimentos = 1; \
        } else { \
            falhas++; \
            bool deve_aumentar = (SMART_LOGIC) ? (falhas % 5 == 0) : true; \
            if (deve_aumentar) { \
                alpha_p += 0.2; \
                nivel_movimentos++; \
                if (alpha_p > 1.01) alpha_p = 0.2; \
                if (nivel_movimentos > 4) nivel_movimentos = 1; /* Do not let the perturbation destroy too much */ \
            } \
            atualizavetorfloat(s, s_best, npilhas); \
            atualizamatrizint(nvisitas, nvis_best, npilhas, ncam); \
            atualizamelhorsolucaoalocarga(carga, carga_best, npilhas); \
        } \
    } \
    atualizavetorfloat(s, s_best, npilhas); \
    atualizamatrizint(nvisitas, nvis_best, npilhas, ncam); \
    atualizamelhorsolucaoalocarga(carga, carga_best, npilhas); \
    liberavetorfloat(s_best); liberamatriz(nvis_best, npilhas); free(carga_best);







// ILS-GRASP-Referencia: EXECUTAR_ILS_GRASP with the VND local search and SMART_LOGIC = false.
// QTDisp (quantity limit per front) and ncarga (number of loaders) are used by the GRASP rebuild.
void ILS_GRASP_Referencia(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem, float *QTDisp, int ncarga) {
    EXECUTAR_ILS_GRASP(VND_Referencia, false)
}

// ILS-GRASP-RLP: EXECUTAR_ILS_GRASP with the VND-RLP local search and SMART_LOGIC = false.
void ILS_GRASP_RLP(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist ,float *dispmec,int **nviagem,float *QTDisp,int ncarga) {
    EXECUTAR_ILS_GRASP(VND_RLP,false)
}

// Smart-ILS-GRASP-Referencia: EXECUTAR_ILS_GRASP with the VND local search and SMART_LOGIC = true.
void Smart_ILS_GRASP_Referencia(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam,int wfdist,float *dispmec,int **nviagem,float *QTDisp,int ncarga) {
    EXECUTAR_ILS_GRASP(VND_Referencia,true)
}

// Smart-ILS-GRASP-RLP: EXECUTAR_ILS_GRASP with the VND-RLP local search and SMART_LOGIC = true.
void Smart_ILS_GRASP_RLP(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem, float *QTDisp, int ncarga) {
    EXECUTAR_ILS_GRASP(VND_RLP, true)
}

// ILS-RLP-Parametrizado: ILS with VND-RLP and the smart escalation (level raised every 5 failures).
// Runs max_iter iterations instead of 100, and the level wraps to 1 above max_nivel.
// It does not use the GRASP rebuild.
void ILS_RLP_Parametrizado(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem, int max_iter, int max_nivel) 
{
    // Memory for the best (elite) solution: s_best, nvis_best and carga_best.
    float *s_best = criavetorfloat(npilhas);
    int **nvis_best = criamatriz(npilhas, ncam);
    struct alocarga *carga_best = criavetoralocarga(npilhas);
    
    // Evaluates the initial solution. fo_melhor is the best objective value so far (lower is better).
    long long fo_melhor = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);
    
    // Stores the initial solution as the best one.
    atualizavetorfloat(s_best, s, npilhas);
    atualizamatrizint(nvis_best, nvisitas, npilhas, ncam);
    atualizamelhorsolucaoalocarga(carga_best, carga, npilhas);
    
    // nivel: current perturbation strength. falhas: failures since the last improvement.
    int nivel = 1;
    int falhas = 0;
    
    // Main loop: max_iter iterations, each a perturbation followed by the VND-RLP local search.
    for (int iter = 0; iter < max_iter; iter++) {
        // Perturbs the current solution with nivel random moves.
        realizar_perturbacao_inteligente(nvisitas, nviagem, s, capcam, npilhas, ncam, carga, compatibilidade, nivel);
        // Local search on the perturbed solution.
        VND_RLP(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);
        
        // Evaluates the new solution.
        long long fo_nova = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);
        
        // Improvement: keeps the solution as the new best and resets the level and the failure count.
        if (fo_nova < fo_melhor) {
            fo_melhor = fo_nova;
            atualizavetorfloat(s_best, s, npilhas);
            atualizamatrizint(nvis_best, nvisitas, npilhas, ncam);
            atualizamelhorsolucaoalocarga(carga_best, carga, npilhas);
            nivel = 1;
            falhas = 0;
        } else {
            // No improvement: counts the failure and raises the level once every 5 failures.
            falhas++;
            if (falhas % 5 == 0) nivel++;
            // Keeps the level within 1..max_nivel (wraps to 1 when exceeded).
            if (nivel > max_nivel) nivel = 1;
            
            // Rollback: restores the best solution before the next perturbation.
            atualizavetorfloat(s, s_best, npilhas);
            atualizamatrizint(nvisitas, nvis_best, npilhas, ncam);
            atualizamelhorsolucaoalocarga(carga, carga_best, npilhas);
        }
    }
    // After the loop, returns the best solution found.
    atualizavetorfloat(s, s_best, npilhas);
    atualizamatrizint(nvisitas, nvis_best, npilhas, ncam);
    atualizamelhorsolucaoalocarga(carga, carga_best, npilhas);
    
    // Releases the best-solution memory.
    liberavetorfloat(s_best);
    liberamatriz(nvis_best, npilhas);
    free(carga_best);
}
