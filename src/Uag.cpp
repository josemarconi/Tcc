// Genetic algorithm (AlgoritmoGenetico) for the loading and haulage plan: individual memory and selection,
// crossover, repair and mutation operators, and the generational driver. Prototypes are in Hag.h.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <stdexcept>
#include <vector>
// Standard headers: stdlib (rand, malloc, calloc, free), math (ceil), limits (LLONG_MAX), float (FLT_MAX).

#include "Hag.h"
#include "Hutil.h"
#include "Hdefine.h"
#include "hstruct.h"
#include "Hutilcarga.h"
#include "Hfavaliacao.h"
#include "Hvnd.h"
#include "Hmovimento.h"

// AVALIAR(IND): evaluates individual IND with favaliacao and returns the objective value (long long).
// The macro uses li, ls, rec, capcam, cargamax and the other data variables of the calling scope.
#define AVALIAR(IND) favaliacao(IND.s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, IND.nvisitas, IND.carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem)

// -----------------------------------------------------------------------------
// MEMORY MANAGEMENT AND SELECTION
// -----------------------------------------------------------------------------
// Allocates an individual for npilhas fronts and ncam trucks. All fronts start unassigned
// (equip = disponivel, op = false) with no trips, and fo is LLONG_MAX (not yet evaluated).
Individuo criar_individuo(int npilhas, int ncam) {
    Individuo ind;
    ind.s = criavetorfloat(npilhas);
    ind.nvisitas = criamatriz(npilhas, ncam);
    ind.carga = criavetoralocarga(npilhas);
    for(int f = 0; f < npilhas; f++) {
        ind.s[f] = 0.0;
        ind.carga[f].equip = disponivel;
        ind.carga[f].op = false;
        for(int c = 0; c < ncam; c++) ind.nvisitas[f][c] = 0;
    }
    ind.fo = LLONG_MAX;
    return ind;
}

// Frees the memory of an individual (s, nvisitas and carga). Only pointers are copied, so the arrays are freed.
void liberar_individuo(Individuo ind, int npilhas) {
    liberavetorfloat(ind.s);
    liberamatriz(ind.nvisitas, npilhas);
    free(ind.carga);
}

// Deep-copies src into dest: s, nvisitas, loader and operation status of each front, and fo.
// dest must already be allocated with the same sizes.
void copiar_individuo(Individuo &dest, Individuo &src, int npilhas, int ncam) {
    atualizavetorfloat(dest.s, src.s, npilhas);
    atualizamatrizint(dest.nvisitas, src.nvisitas, npilhas, ncam);
    for(int i = 0; i < npilhas; i++) {
        dest.carga[i].equip = src.carga[i].equip;
        dest.carga[i].op = src.carga[i].op;
    }
    dest.fo = src.fo;
}

// Binary tournament: draws two random individuals of pop and returns the index of the one with the lower fo.
int torneio(Individuo *pop, int tam_pop) {
    int cand1 = rand() % tam_pop;
    int cand2 = rand() % tam_pop;
    return (pop[cand1].fo < pop[cand2].fo) ? cand1 : cand2;
}

// Sorts the first tamanho individuals of pop by fo, ascending (best first), with an exchange sort of the structs.
void ordenar_populacao(Individuo *pop, int tamanho) {
    for(int i=0; i<tamanho-1; i++) {
        for(int j=i+1; j<tamanho; j++) {
            if(pop[i].fo > pop[j].fo) {
                Individuo temp = pop[i];
                pop[i] = pop[j];
                pop[j] = temp;
            }
        }
    }
}

// -----------------------------------------------------------------------------
// LOADER AND TRIP CROSSOVER
// -----------------------------------------------------------------------------
// Crossover: inherits the parent assignments, repairs duplicate loaders and truck time, then recomputes production.
// p1 and p2 are passed by value, but their arrays are shared with the caller.
void crossover_carga_viagens(Individuo p1, Individuo p2, Individuo &filho, int npilhas, int ncam, int ncarga,
                             float *capcam, int **compatibilidade, float **tciclo, float tcontrole, float *dispmec) 
{
    // For each front, the loader and the trip counts are inherited from p1 or p2 with probability 1/2.
    for (int f = 0; f < npilhas; f++) {
        if ((rand() % 2) == 0) {
            filho.carga[f].equip = p1.carga[f].equip;
            filho.carga[f].op   = p1.carga[f].op;
            for (int c = 0; c < ncam; c++) filho.nvisitas[f][c] = p1.nvisitas[f][c];
        } else {
            filho.carga[f].equip = p2.carga[f].equip;
            filho.carga[f].op   = p2.carga[f].op;
            for (int c = 0; c < ncam; c++) filho.nvisitas[f][c] = p2.nvisitas[f][c];
        }
    }

    // Loader repair: a loader may serve only one front. A front whose loader was already used by an
    // earlier front is cleared (no loader, no operation, no trips).
    bool *usado = (bool*)calloc(ncarga, sizeof(bool));
    for (int f = 0; f < npilhas; f++) {
        int eq = filho.carga[f].equip;
        if (eq != disponivel) {
            if (usado[eq]) {
                filho.carga[f].equip = disponivel;
                filho.carga[f].op = false;
                for (int c = 0; c < ncam; c++) filho.nvisitas[f][c] = 0;
            } else {
                usado[eq] = true;
            }
        }
    }
    free(usado);

    ajustarTempoCaminhoes(filho.nvisitas, filho.carga, compatibilidade, tciclo, tcontrole, npilhas, ncam, dispmec);
    carregavetors(filho.s, filho.nvisitas, filho.carga, capcam, compatibilidade, ncam, npilhas);
}

// -----------------------------------------------------------------------------
// FAST FEASIBILITY REPAIR
// -----------------------------------------------------------------------------
// Repair operator: converts the hourly ore target to tonnes over the horizon, then adds trips greedily.
void reparar_producao_rapido(Individuo &ind, int npilhas, int npminerio, int ncam, int ncarga, 
                             float *capcam, int **compatibilidade, float **tciclo, float tcontrole, 
                             float *dispmec, int **nviagem, float prodrec, float *QTDisp)
{
    const float meta_minerio_turno = prodrec * horasDoHorizonte(tcontrole);
    // Mass of ore transported by the individual.
    float prod_atual = 0.0f;
    for (int f = 0; f < npminerio; f++) prod_atual += ind.s[f];
    
    // The individual already meets the production target: nothing to repair.
    if (prod_atual >= meta_minerio_turno) return;

    // Loaders already assigned to some front.
    bool *maq_em_uso = (bool*)calloc(ncarga, sizeof(bool));
    for (int f = 0; f < npilhas; f++) {
        int eq = ind.carga[f].equip;
        if (eq != disponivel) maq_em_uso[eq] = true;
    }

    // Gives a free loader (the first one not in use) to each unassigned ore front.
    for (int f = 0; f < npminerio; f++) {
        if (ind.carga[f].equip == disponivel) {
            for (int eq = 0; eq < ncarga; eq++) {
                if (!maq_em_uso[eq]) {
                    ind.carga[f].equip = eq; ind.carga[f].op = true; maq_em_uso[eq] = true; break;
                }
            }
        }
    }
    free(maq_em_uso);

    double *tempo_cam = (double*)calloc(ncam, sizeof(double));
    for (int c = 0; c < ncam; c++) {
        tempo_cam[c] = tempoUtilizadoCaminhao(ind.nvisitas, ind.carga, compatibilidade, tciclo, npilhas, c);
    }

    // Greedy loop: adds one trip per iteration until the production target is reached.
    while (prod_atual < meta_minerio_turno) {
        int melhor_f = -1;
        int melhor_c = -1;
        float melhor_score = -FLT_MAX;

        // Scans ore fronts that have a loader and are operating, and all trucks, for the best feasible trip.
        for (int f = 0; f < npminerio; f++) {
            if (ind.carga[f].equip == disponivel || !ind.carga[f].op) continue;
            for (int c = 0; c < ncam; c++) {
                // Feasibility of a trip (f, c): compatible truck, trip limit nviagem[f][c], front quantity QTDisp[f],
                // and truck time limit.
                if (compatibilidade[c][ind.carga[f].equip] != compativel) continue;
                if (ind.nvisitas[f][c] >= nviagem[f][c]) continue;
                if (ind.s[f] + capcam[c] > QTDisp[f]) continue;
                const double tempo_max = tcontrole * dispmec[c];
                if (tempo_cam[c] + tciclo[f][c] > tempo_max) continue;

                // Score favours high-capacity trucks with short cycle times.
                float score = capcam[c] / (1.0f + tciclo[f][c]);
                if (score > melhor_score) {
                    melhor_score = score; melhor_f = f; melhor_c = c;
                }
            }
        }

        // No feasible trip is left: stops.
        if (melhor_f == -1) break; 
        // Adds the selected trip and updates the front, truck time and production.
        ind.nvisitas[melhor_f][melhor_c]++;
        ind.s[melhor_f] += capcam[melhor_c];
        tempo_cam[melhor_c] += tciclo[melhor_f][melhor_c];
        prod_atual += capcam[melhor_c];
    }
    free(tempo_cam);
}

// -----------------------------------------------------------------------------
// 1. LIGHT MUTATION
// -----------------------------------------------------------------------------
// Light mutation operator: applies 1 to 3 random moves to ind. Below prodrec the moves are 0, 1 or 2;
// otherwise they are 1, 2, 4, 6 or 7. Modifies ind in place; ind.fo is not updated.
void mutacao_dirigida(Individuo &ind, int npilhas, int ncam, float *capcam, int **compatibilidade, int **nviagem, float prodrec) 
{
    // Production of the individual, used to choose the move set. It is computed once, before the moves.
    float producao = 0.0f;
    for (int f = 0; f < npilhas; f++) producao += ind.s[f];

    // One to three moves per call.
    int num_movs = (rand() % 3) + 1; 
    for (int i = 0; i < num_movs; i++) {
        int mov = 0;
        // Below the production target: moves 0, 1 or 2 (trip counts and trip relocation).
        if (producao < prodrec) {
            int r = rand() % 3;
            if (r == 0) mov = 0; if (r == 1) mov = 1; if (r == 2) mov = 2; 
        } else {
            // At or above the target: moves 1, 2, 4, 6 or 7 (relocations, pit operation status and swaps).
            int r = rand() % 5;
            if (r == 0) mov = 1; if (r == 1) mov = 2; if (r == 2) mov = 4; if (r == 3) mov = 6; if (r == 4) mov = 7; 
        }

        // Random front indices f1, f2 and truck indices c1, c2 for the chosen move.
        int f1 = rand() % npilhas; int f2 = rand() % npilhas;
        int c1 = rand() % ncam;    int c2 = rand() % ncam;

        // For moves that act on a front's loader, f1 is redrawn (at most 10 times) while that front has no loader.
        if (mov == 1 || mov == 2 || mov == 4 || mov == 5 || mov == 6 || mov == 7) {
            int loop = 0;
            while (ind.carga[f1].equip == disponivel && loop < 10) { f1 = rand() % npilhas; loop++; }
        }

        // Applies the chosen move; the move functions update nvisitas, s and carga of ind.
        switch (mov) {
            case 0: movimentoNumberTrips(ind.nvisitas, nviagem, ind.s, capcam, npilhas, ncam, ind.carga); break;
            case 1: movimentoRelocateTripTruck(ind.nvisitas, nviagem, f1, f2, c1, ind.s, capcam, compatibilidade, ind.carga); break;
            case 2: movimentoRelocateTripPit(ind.nvisitas, nviagem, f1, c1, c2, ind.s, capcam, compatibilidade, ind.carga); break;
            case 3: movimentoLoad(ind.carga, npilhas, ncam, compatibilidade, ind.nvisitas, ind.s, capcam, nviagem); break;
            case 4: movimentoPitOperation(ind.carga, f1, ind.nvisitas, ind.s, ncam); break;
            case 5: movimentoTruckOperation(ind.nvisitas, f1, c1, ind.s, capcam); break;
            case 6: movimentoSwapTrips(ind.nvisitas, f1, f2, c1, c2, ind.s, capcam, compatibilidade, ind.carga, nviagem); break;
            case 7: movimentoSwapShovels(ind.carga, f1, f2, ind.nvisitas, ncam, compatibilidade, ind.s, capcam); break;
        }
    }
}

// -----------------------------------------------------------------------------
// 2. HEAVY MUTATION (GRASP)
// -----------------------------------------------------------------------------
// Heavy mutation operator: destroys 10% to 20% of the ore fronts and rebuilds them with a GRASP-style
// construction. The candidate list has relative size alpha. ind is passed by value, but its arrays are
// shared with the caller, so the caller's individual is changed.
void mutacao_GRASP_minerio(Individuo ind, int npilhas, int npminerio, int ncam, int ncarga, int nvar, 
                           float *capcam, int **compatibilidade, float **teores, float *rec, 
                           float *QTDisp, float *cargamax, int **nviagem, float prodrec, float alpha) 
{
    // Nothing to destroy when there are no ore fronts.
    if (npminerio <= 0) return;

    // Number of ore fronts to destroy: 10% to 20%, at least 1 and at most npminerio.
    int qtd_matar = ceil(npminerio * (rand() % 11 + 10) / 100.0);
    if(qtd_matar < 1) qtd_matar = 1;
    if(qtd_matar > npminerio) qtd_matar = npminerio;

    // Flags of the destroyed ore fronts.
    bool *destruido = (bool*)calloc(npminerio, sizeof(bool));
    int mortos = 0;
    
    // Destroys random ore fronts until qtd_matar are destroyed. Each one loses its loader, operation flag,
    // production and trips.
    while(mortos < qtd_matar) {
        int f = rand() % npminerio;
        if(!destruido[f]) {
            destruido[f] = true;
            ind.carga[f].equip = disponivel;
            ind.carga[f].op = false;
            ind.s[f] = 0.0;
            for(int c=0; c<ncam; c++) ind.nvisitas[f][c] = 0;
            mortos++;
        }
    }

    // Loaders still used by fronts that were not destroyed (not free for the rebuild).
    bool *loader_used = (bool*)calloc(ncarga, sizeof(bool));
    for(int i=0; i<npilhas; i++) {
        if((i >= npminerio || !destruido[i]) && ind.carga[i].equip != disponivel) {
            if (ind.carga[i].equip >= 0 && ind.carga[i].equip < ncarga) loader_used[ind.carga[i].equip] = true;
        }
    }

    // Production of the ore fronts that survived the destruction.
    float prod_min_atual = 0.0;
    for(int i=0; i<npminerio; i++) if(!destruido[i]) prod_min_atual += ind.s[i];

    // Rebuild only when at least one ore front was destroyed.
    if (mortos > 0) {
        // Destroyed fronts and their grade deviation from rec: sum over the variables of |teor - rec|.
        int *ore_faces = (int*)malloc(mortos * sizeof(int));
        float *deviations = (float*)malloc(mortos * sizeof(float));
        int idx = 0;
        for(int i=0; i<npminerio; i++) {
            if(destruido[i]) {
                ore_faces[idx] = i;
                deviations[idx] = 0;
                for(int j=0; j<nvar; j++) deviations[idx] += absfloat(teores[i][j] - rec[j]);
                idx++;
            }
        }

        // Exchange sort of the destroyed fronts by deviation, ascending.
        for(int i=0; i<mortos-1; i++) {
            for(int j=i+1; j<mortos; j++) {
                if(deviations[i] > deviations[j]) {
                    float td = deviations[i]; deviations[i] = deviations[j]; deviations[j] = td;
                    int tf = ore_faces[i]; ore_faces[i] = ore_faces[j]; ore_faces[j] = tf;
                }
            }
        }

        // Loaders sorted by cargamax, ascending.
        int *sorted_loaders = (int*)malloc(ncarga * sizeof(int));
        for(int i=0; i<ncarga; i++) sorted_loaders[i] = i;
        for(int i=0; i<ncarga-1; i++) {
            for(int j=i+1; j<ncarga; j++) {
                if(cargamax[sorted_loaders[i]] > cargamax[sorted_loaders[j]]) {
                    int temp = sorted_loaders[i]; sorted_loaders[i] = sorted_loaders[j]; sorted_loaders[j] = temp;
                }
            }
        }

        // Trucks sorted by capcam, descending.
        int *sorted_trucks = (int*)malloc(ncam * sizeof(int));
        for(int i=0; i<ncam; i++) sorted_trucks[i] = i;
        for(int i=0; i<ncam-1; i++) {
            for(int j=i+1; j<ncam; j++) {
                if(capcam[sorted_trucks[i]] < capcam[sorted_trucks[j]]) {
                    int temp = sorted_trucks[i]; sorted_trucks[i] = sorted_trucks[j]; sorted_trucks[j] = temp;
                }
            }
        }

        // Index of the next candidate loader, starting at the largest one. Loaders in use are skipped.
        int l_idx = ncarga - 1;
        // Rebuilds the destroyed fronts from the most similar grade first. Each step draws a front from a
        // restricted candidate list (RCL) whose size is ceil(remaining * alpha).
        for(int i=0; i<mortos; i++) {
            // Stops when the production target is reached.
            if (prod_min_atual >= prodrec) break;
            int restante = mortos - i;
            int range_lrc = (int)ceil(restante * alpha);
            if (range_lrc < 1) range_lrc = 1;
            // Random pick within the restricted candidate list.
            int escolhido_idx = i + (rand() % range_lrc);

            int f = ore_faces[escolhido_idx];
            // Moves the chosen front to position i so the remaining candidates stay contiguous.
            int temp_f = ore_faces[i]; ore_faces[i] = ore_faces[escolhido_idx]; ore_faces[escolhido_idx] = temp_f;

            // Next free loader, starting from the largest.
            while(l_idx >= 0 && loader_used[sorted_loaders[l_idx]]) l_idx--;
            if(l_idx >= 0) {
                int l_id = sorted_loaders[l_idx];
                ind.carga[f].equip = l_id;
                ind.carga[f].op = true;
                loader_used[l_id] = true;

                // Trucks from smallest to largest capacity, only those compatible with the chosen loader.
                for(int j=ncam-1; j>=0; j--) {
                    int c = sorted_trucks[j];
                    if(compatibilidade[c][l_id] == compativel) {
                        float falta = prodrec - prod_min_atual;
                        // Stops when the production target is covered.
                        if (falta <= 0) break;
                        // Trips start at the limit nviagem[f][c] and are capped by QTDisp[f] and by the production still missing.
                        int v = nviagem[f][c];
                        if(ind.s[f] + v*capcam[c] > QTDisp[f]) v = (int)((QTDisp[f] - ind.s[f]) / capcam[c]);
                        if(v * capcam[c] > falta) v = (int)ceil(falta / capcam[c]);

                        // Sets the trips for (f, c) and updates the front and surviving ore production.
                        ind.nvisitas[f][c] = v;
                        ind.s[f] += v * capcam[c];
                        prod_min_atual += v * capcam[c];
                    }
                }
            }
        }
        // Releases the temporary arrays.
        free(ore_faces); free(deviations); free(sorted_loaders); free(sorted_trucks);
    }
    free(destruido); free(loader_used); 
}

// -----------------------------------------------------------------------------
// MASTER STRUCTURE OF THE GENETIC ALGORITHM (GA)
// -----------------------------------------------------------------------------
// Builds tam_pop parents by GRASP construction and local search, then runs 50 generations.
// Each generation creates tam_pop offspring and keeps the best tam_pop individuals as parents.
// Mutation is heavy (GRASP) after 5 generations without improvement, otherwise light (directed).
// forma_vnd == 1 selects VND as local search, any other value selects VND-RLP.
// The best individual is copied to s_main, nvis_main and carga_main. Returns the number of generations executed.
int AlgoritmoGenetico(
         float *s_main, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv,
         int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, float *QTDisp, int ncarga,
         float remrec, float remmax, float remmin, int **nvis_main, struct alocarga *carga_main, float *capcam,
         int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin,
         int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem,
         int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn,
         int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga,
         int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem, int forma_vnd,
         int tam_pop, float alpha)
{
    if (tam_pop < 2 || tam_pop > INT_MAX / 2 || !(alpha >= 0.0f && alpha <= 1.0f))
        throw std::invalid_argument("Invalid population size or alpha");

    const int tam_total = 2 * tam_pop;
    const int max_geracoes = 50;
    int geracoes_executadas = 0;
    const float meta_minerio_turno = prodrec * horasDoHorizonte(tcontrole);

    std::vector<Individuo> pop(tam_total);
    for (Individuo &ind : pop) ind = criar_individuo(npilhas, ncam);

    // Initialization
    // Initial parent population: a GRASP construction followed by local search for each individual.
    for(int i=0; i<tam_pop; i++) {
        // Builds a GRASP initial solution for this individual.
        constroisolucaoGRASP(pop[i].s, pop[i].carga, capcam, compatibilidade, pop[i].nvisitas, nviagem, npilhas, ncam, nvar, teores, rec, npminerio, QTDisp, cargamax, cargamin, ncarga, meta_minerio_turno, remrec, alpha);
        
        // Local search: VND if forma_vnd == 1, otherwise VND-RLP.
        if(forma_vnd == 1) VND_Referencia(pop[i].s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, pop[i].nvisitas, pop[i].carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);
        else VND_RLP(pop[i].s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, pop[i].nvisitas, pop[i].carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);
        
        // Evaluates the individual. fo is the objective value (lower is better).
        pop[i].fo = AVALIAR(pop[i]);
    }
    // Sorts the parents so the best one is at index 0.
    ordenar_populacao(pop.data(), tam_pop);

    // Best objective value found so far.
    long long melhor_fo_global = pop[0].fo;
    int geracoes_sem_melhora = 0;
    // Generations without improvement before the heavy GRASP mutation is used.
    int limite_estagnacao = 5;

    // Main loop over the generations.
    for(int ger = 0; ger < max_geracoes; ger++) 
    {
        geracoes_executadas++;
        // Builds tam_pop offspring, written into slots tam_pop .. tam_total - 1.
        for(int i=0; i<tam_pop; i++) {
            // Two different parents chosen by binary tournament.
            int p1 = torneio(pop.data(), tam_pop);
            int p2 = torneio(pop.data(), tam_pop);
            while(p1 == p2) p2 = torneio(pop.data(), tam_pop);

            // Crossover of loader assignments and trips from p1 and p2 into the offspring slot.
            crossover_carga_viagens(pop[p1], pop[p2], pop[tam_pop + i], npilhas, ncam, ncarga, capcam, compatibilidade, tciclo, tcontrole, dispmec);
            // Repairs the offspring ore production up to prodrec.
            reparar_producao_rapido(pop[tam_pop + i], npilhas, npminerio, ncam, ncarga, capcam, compatibilidade, tciclo, tcontrole, dispmec, nviagem, prodrec, QTDisp);
            
            // Mutation: heavy GRASP mutation after 5 generations without improvement, otherwise the light directed mutation.
            if (geracoes_sem_melhora >= limite_estagnacao) {
                mutacao_GRASP_minerio(pop[tam_pop + i], npilhas, npminerio, ncam, ncarga, nvar, capcam, compatibilidade, teores, rec, QTDisp, cargamax, nviagem, meta_minerio_turno, alpha);
            } else {
                mutacao_dirigida(pop[tam_pop + i], npilhas, ncam, capcam, compatibilidade, nviagem, meta_minerio_turno);
            }
            
            // Local search on the offspring (VND or VND-RLP, as above).
            if(forma_vnd == 1) VND_Referencia(pop[tam_pop + i].s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, pop[tam_pop + i].nvisitas, pop[tam_pop + i].carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);
            else VND_RLP(pop[tam_pop + i].s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, pop[tam_pop + i].nvisitas, pop[tam_pop + i].carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

            // Evaluates the offspring.
            pop[tam_pop + i].fo = AVALIAR(pop[tam_pop + i]);
        }
        // Sorts parents and offspring by fo, keeping the best tam_pop at the front.
        ordenar_populacao(pop.data(), tam_total);

        // Updates the best objective value and the count of generations without improvement.
        if (pop[0].fo < melhor_fo_global) {
            melhor_fo_global = pop[0].fo;
            geracoes_sem_melhora = 0;
        } else {
            geracoes_sem_melhora++;
        }
    }

    // Copies the best individual (index 0) to the output arrays s, nvisitas and carga.
    atualizavetorfloat(s_main, pop[0].s, npilhas);
    atualizamatrizint(nvis_main, pop[0].nvisitas, npilhas, ncam);
    for(int i = 0; i < npilhas; i++) {
        carga_main[i].equip = pop[0].carga[i].equip;
        carga_main[i].op = pop[0].carga[i].op;
    }

    // Frees all individuals.
    for (const Individuo &ind : pop) liberar_individuo(ind, npilhas);

    // Number of generations executed. The loop has no early exit, so this equals max_geracoes.
    return geracoes_executadas;
}
