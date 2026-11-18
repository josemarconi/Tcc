// Variable Neighbourhood Descent (VND) local search for the truck dispatch solution.
// Solution state, modified in place: s[f] = production of front f (pile);
// nvisitas[f][c] = number of trips of truck c on front f; carga[f].equip = loader
// (equipment) id of front f; carga[f].op = operating flag of front f (true = active).
// nviagem[f][c] = maximum number of trips allowed for (f, c); capcam[c] = capacity of truck c;
// compatibilidade[c][e] = compativel (1) when truck c can work with loader e.
// The objective comes from favaliacao() and is minimised (smaller is better).
// Each melhorVizinho* function scans one neighbourhood with first improvement: it applies
// one move, evaluates it, keeps it and returns true if the objective decreased, otherwise it
// undoes the move and continues. VND and VND-RLP combine these neighbourhoods.
#include "Hvnd.h"
#include "Hmovimento.h"
#include "Hfavaliacao.h"
#include "Hutil.h"
#include <stdlib.h>
#include "Hdefine.h"
#include "hstruct.h"
#include "Hutilcarga.h"

// Neighbourhood NT (number of trips): for every front f and truck c, tries +1 trip
// (only while nvisitas < nviagem; a switched-off front is turned on) and then -1 trip.
// The first move that lowers the objective is kept, *fo_atual is updated and true is returned.
// Otherwise every move is undone and false is returned.
bool melhorVizinhoNT(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                     int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                     float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                     int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                     int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                     int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                     int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                     int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem,
                     long long *fo_atual) 
{
    // Outer loop: every front f.
    // Walks each mining front (pile)
    for (int f = 0; f < npilhas; f++) {
        // Inner loop: every truck c, so each (front, truck) pair is tried.
        // Walks every truck in the fleet
        for (int c = 0; c < ncam; c++) {
            
            // --- ATTEMPT 1: Add one trip (+1) ---
            // Adding one trip is only possible while the trip count is below the nviagem limit.
            if (nvisitas[f][c] < nviagem[f][c]) {
                // Remember whether the front was switched off, so the activation rule can be undone.
                bool estava_desligada = (carga[f].op == false);
                // Apply the change
                nvisitas[f][c]++;
                s[f] += capcam[c];
                // A front that was switched off starts operating when it receives a trip (automatic activation).
                if (estava_desligada) carga[f].op = true; // Automatic activation rule

                // Evaluate the new solution with the full objective function.
                long long nova_fo = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

                // Accept the move and stop this neighbourhood if the objective decreased (first improvement).
                if (nova_fo < *fo_atual) {
                    *fo_atual = nova_fo;
                    return true; // Found an improvement; ends the search in this neighbourhood
                }

                // The move did not help: undo the trip, its production and the activation flag.
                // UNDO (Rollback) if it does not improve
                nvisitas[f][c]--;
                s[f] -= capcam[c];
                if (estava_desligada) carga[f].op = false;
            }

            // --- ATTEMPT 2: Remove one trip (-1) ---
            // Try removing one trip of truck c from front f (only if at least one exists).
            if (nvisitas[f][c] > 0) {
                // Apply the change
                nvisitas[f][c]--;
                s[f] -= capcam[c];

                // Evaluate the solution after removing the trip.
                long long nova_fo = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

                // Accept the move if the objective decreased; otherwise undo it below.
                if (nova_fo < *fo_atual) {
                    *fo_atual = nova_fo;
                    return true;
                }

                // Undo the removal, since it did not improve the objective.
                // UNDO (Rollback)
                nvisitas[f][c]++;
                s[f] += capcam[c];
            }
        }
    }
    return false;
}

// Neighbourhood TT (truck trip relocation): moves one trip of a truck c from its current
// front f1 to another front f2. Only operating destination fronts that are compatible with
// truck c's loader and still have room for one more trip are tried. First improvement;
// returns true and updates *fo_atual when the objective decreases, otherwise rolls back.
bool melhorVizinhoTT(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                     int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                     float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                     int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                     int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                     int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                     int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                     int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem,
                     long long *fo_atual) 
{
    // Walks every truck in the fleet
    // Outer loop: every truck c. The trip moved by this neighbourhood belongs to truck c.
    for (int c = 0; c < ncam; c++) {
        // Walks the ORIGIN front (f1)
        // Origin front f1: the trip is taken from here.
        for (int f1 = 0; f1 < npilhas; f1++) {
            
            // Only try to move if the truck already works on f1
            // Only a front where truck c has at least one trip can give a trip.
            if (nvisitas[f1][c] > 0) {
                
                // Walks the DESTINATION front (f2)
                // Destination front f2: every other front.
                for (int f2 = 0; f2 < npilhas; f2++) {
                    if (f1 == f2) continue; // Moving to the same front makes no sense

                    // Feasibility checks for the destination f2
                    // Destination must be operating, compatible with truck c's loader, and have room for a trip.
                    if (carga[f2].op == true && 
                        compatibilidade[c][carga[f2].equip] == compativel && 
                        nvisitas[f2][c] < nviagem[f2][c]) 
                    {
                        // 1. Apply the move (Relocate Trip)
                        // 1. Relocate: one trip leaves f1 and joins f2; their productions change by truck c's capacity.
                        nvisitas[f1][c]--;
                        nvisitas[f2][c]++;
                        s[f1] -= capcam[c];
                        s[f2] += capcam[c];

                        // 2. Evaluate the new solution
                        long long nova_fo = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

                        // Evaluate the move; accept it and return if the objective decreased.
                        if (nova_fo < *fo_atual) {
                            *fo_atual = nova_fo;
                            return true; // First Improvement: found an improvement, returns to VND
                        }

                        // 3. Rollback if it does not improve
                        nvisitas[f1][c]++;
                        nvisitas[f2][c]--;
                        s[f1] += capcam[c];
                        s[f2] -= capcam[c];
                    }
                }
            }
        }
    }
    return false; // No improvement found after testing all destination moves
}

// Neighbourhood TP (trip transfer inside a front): for every operating front f, moves one trip
// of truck c1 to truck c2 on the same front, when c2 is compatible with the loader of f and has
// room for one more trip. The production of f changes by the difference of the two capacities.
// First improvement; rolls back otherwise.
bool melhorVizinhoTP(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                     int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                     float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                     int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                     int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                     int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                     int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                     int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem,
                     long long *fo_atual) 
{
    // Walks each mining front (Pit)
    // Outer loop: every front f. Transfers only happen inside one front.
    for (int f = 0; f < npilhas; f++) {
        
        // Only meaningful if the front is active
        // Skip inactive fronts: they have no trips to transfer.
        if (carga[f].op == false) continue;

        // Walks the donor truck (c1)
        // Donor truck c1 must have at least one trip on front f.
        for (int c1 = 0; c1 < ncam; c1++) {
            
            // Checks whether truck c1 has trips to donate on this front
            if (nvisitas[f][c1] > 0) {
                
                // Walks the receiver truck (c2)
                // Receiver truck c2: every other truck.
                for (int c2 = 0; c2 < ncam; c2++) {
                    if (c1 == c2) continue; // Transferring to itself makes no sense

                    // Feasibility checks for the receiver c2 on front f
                    // Receiver must be compatible with the loader of front f and have room for one more trip.
                    if (compatibilidade[c2][carga[f].equip] == compativel && 
                        nvisitas[f][c2] < nviagem[f][c2]) 
                    {
                        // 1. Apply the move (internal transfer on the front)
                        // Move one trip from c1 to c2 on front f.
                        nvisitas[f][c1]--;
                        nvisitas[f][c2]++;
                        
                        // Update production: s[f] = s[f] - mass_c1 + mass_c2
                        // Production of f: remove c1's capacity and add c2's capacity.
                        s[f] = s[f] - capcam[c1] + capcam[c2];

                        // 2. Evaluate the new solution
                        // Evaluate the move; accept it and return if the objective decreased.
                        long long nova_fo = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

                        if (nova_fo < *fo_atual) {
                            *fo_atual = nova_fo;
                            return true; // Found an improvement (First Improvement)
                        }

                        // 3. Rollback if it does not improve
                        // Undo the transfer: trip counts and production return to their previous values.
                        nvisitas[f][c1]++;
                        nvisitas[f][c2]--;
                        s[f] = s[f] + capcam[c1] - capcam[c2];
                    }
                }
            }
        }
    }
    return false; // No improvement found after testing all internal transfers in the pits
}

// Neighbourhood ST (trip swap between cells): exchanges one trip from cell (f1, c1) with a
// free slot of cell (f2, c2), for any pair of (front, truck) cells. The destination must be an
// operating front, compatible with truck c2, with room for one more trip. Pairs of the same
// cell are skipped. First improvement; rolls back otherwise.
bool melhorVizinhoST(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                     int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                     float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                     int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                     int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                     int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                     int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                     int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem,
                     long long *fo_atual) 
{
    // Walks the origin: front f1 and truck c1
    // Origin cell loop: front f1 and truck c1.
    for (int f1 = 0; f1 < npilhas; f1++) {
        for (int c1 = 0; c1 < ncam; c1++) {
            
            // Checks whether there is a trip to take from (f1, c1)
            // Only a cell with at least one trip can give a trip.
            if (nvisitas[f1][c1] > 0) {
                
                // Walks the destination: front f2 and truck c2
                // Destination cell loop: front f2 and truck c2, over all cells.
                for (int f2 = 0; f2 < npilhas; f2++) {
                    for (int c2 = 0; c2 < ncam; c2++) {
                        
                        // Avoids the same cell (this would be neutral)
                        // Skip the same cell: the move would have no effect.
                        if (f1 == f2 && c1 == c2) continue;

                        // Feasibility checks for the destination (f2, c2)
                        // Destination must be operating, compatible with truck c2, and have room for one more trip.
                        if (carga[f2].op == true && 
                            compatibilidade[c2][carga[f2].equip] == compativel && 
                            nvisitas[f2][c2] < nviagem[f2][c2]) 
                        {
                            // Move one trip from (f1, c1) to (f2, c2); each production uses the capacity of its own truck.
                            // 1. Apply the cross exchange of one trip
                            nvisitas[f1][c1]--;
                            nvisitas[f2][c2]++;
                            s[f1] -= capcam[c1];
                            s[f2] += capcam[c2];

                            // 2. Evaluate the new solution
                            // Evaluate the move; accept it and return if the objective decreased.
                            long long nova_fo = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

                            if (nova_fo < *fo_atual) {
                                *fo_atual = nova_fo;
                                return true; // Found an improvement (First Improvement)
                            }

                            // 3. Rollback if it does not improve
                            nvisitas[f1][c1]++;
                            nvisitas[f2][c2]--;
                            s[f1] += capcam[c1];
                            s[f2] -= capcam[c2];
                        }
                    }
                }
            }
        }
    }
    return false; // No improvement found in this neighbourhood
}

// Neighbourhood L (loader swap): for every pair of fronts with different loaders, exchanges the
// loaders of the two fronts. The trip counts follow the loaders (each front receives the trips of
// the other), clamped to the nviagem limit of the new front. Both productions are recomputed.
// First improvement; otherwise loaders, operating flags, trip counts and productions are restored.
bool melhorVizinhoL(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                    int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                    float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                    int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                    int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                    int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                    int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                    int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem,
                    long long *fo_atual) 
{
    // Walks every pair of fronts (f1 and f2) to test the exchange of loaders
    // Outer loop over front pairs (f1, f2) with f1 < f2, each pair tested once.
    for (int f1 = 0; f1 < npilhas; f1++) {
        for (int f2 = f1 + 1; f2 < npilhas; f2++) {
            
            // Exchanging only makes sense if the loaders differ
            // Exchanging identical loaders changes nothing, so the pair is skipped.
            if (carga[f1].equip == carga[f2].equip) continue;

            // --- 1. APPLY THE MOVE (Swap Load) ---
            // Save the original state for a possible rollback
            int equip_f1_orig = carga[f1].equip;
            int equip_f2_orig = carga[f2].equip;
            bool op_f1_orig = carga[f1].op;
            bool op_f2_orig = carga[f2].op;
            
            // Temporary copies of the trip counts of both fronts, needed for the rollback.
            int *visitas_f1_temp = new int[ncam];
            int *visitas_f2_temp = new int[ncam];
            // Copy the trip counts of both fronts into the temporary arrays.
            for(int c=0; c<ncam; c++) {
                visitas_f1_temp[c] = nvisitas[f1][c];
                visitas_f2_temp[c] = nvisitas[f2][c];
            }

            // Perform the exchange (as in Souza 2010: trips follow the loader)
            // Exchange the loaders between the two fronts.
            carga[f1].equip = equip_f2_orig;
            carga[f2].equip = equip_f1_orig;

            // Trips follow the loader: each front receives the trip counts that were on the other front.
            for (int c = 0; c < ncam; c++) {
                nvisitas[f1][c] = visitas_f2_temp[c];
                nvisitas[f2][c] = visitas_f1_temp[c];
                
                // Limit check (nviagem) on the new front
                // Clamp: a front cannot keep more trips than its nviagem limit.
                if (nvisitas[f1][c] > nviagem[f1][c]) nvisitas[f1][c] = nviagem[f1][c];
                if (nvisitas[f2][c] > nviagem[f2][c]) nvisitas[f2][c] = nviagem[f2][c];
            }

            // A front that now has a loader is switched on. The flag is not cleared when it has no loader.
            // Automatic activation if there is a loader
            if (carga[f1].equip != disponivel) carga[f1].op = true;
            if (carga[f2].equip != disponivel) carga[f2].op = true;

            // Recompute the local productions s[f]
            // Recompute the production of both fronts from their new trip counts and loaders.
            s[f1] = producaofrente(nvisitas, carga, capcam, compatibilidade, ncam, f1);
            s[f2] = producaofrente(nvisitas, carga, capcam, compatibilidade, ncam, f2);

            // --- 2. EVALUATION ---
            // Evaluate the new solution.
            long long nova_fo = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

            // Accept the move and return if the objective decreased (first improvement).
            if (nova_fo < *fo_atual) {
                *fo_atual = nova_fo;
                // Free the temporary copies before returning.
                delete[] visitas_f1_temp; delete[] visitas_f2_temp;
                return true; // Found an improvement
            }

            // --- 3. ROLLBACK ---
            // Restore the loaders and operating flags of both fronts.
            carga[f1].equip = equip_f1_orig;
            carga[f2].equip = equip_f2_orig;
            carga[f1].op = op_f1_orig;
            carga[f2].op = op_f2_orig;
            // Restore the trip counts from the temporary copies.
            for(int c=0; c<ncam; c++) {
                nvisitas[f1][c] = visitas_f1_temp[c];
                nvisitas[f2][c] = visitas_f2_temp[c];
            }
            // Recompute the production of both fronts from the restored trip counts.
            s[f1] = producaofrente(nvisitas, carga, capcam, compatibilidade, ncam, f1);
            s[f2] = producaofrente(nvisitas, carga, capcam, compatibilidade, ncam, f2);
            
            // Free the temporary copies before the next pair.
            delete[] visitas_f1_temp; delete[] visitas_f2_temp;
        }
    }
    return false;
}

// Neighbourhood PO (pit operation): switches each front on or off in turn. Switching a front
// off removes all of its trips and sets its production to zero; switching it on only sets the
// flag (trips are assigned later by other moves). First improvement; otherwise everything is
// restored from a backup of the front's flag, production and trip counts.
bool melhorVizinhoPO(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                     int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                     float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                     int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                     int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                     int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                     int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                     int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem,
                     long long *fo_atual) 
{
    // Walks each mining front to test the operation toggle (On/Off)
    // Outer loop: every front is toggled once.
    for (int f = 0; f < npilhas; f++) {
        
        // --- 1. BACKUP OF THE CURRENT STATE ---
        // Save the flag, production and trip counts of front f for the rollback.
        bool status_original = carga[f].op;
        float s_original = s[f];
        int *visitas_backup = new int[ncam];
        for(int c = 0; c < ncam; c++) visitas_backup[c] = nvisitas[f][c];

        // --- 2. APPLY THE CHANGE (Toggle) ---
        // The front is on: switch it off, remove all its trips and set its production to zero.
        if (carga[f].op == true) {
            // Deactivate: remove all trips of the front
            carga[f].op = false;
            for (int c = 0; c < ncam; c++) nvisitas[f][c] = 0;
            s[f] = 0.0;
        } else {
            // Reactivate: only changes the status (trips come from other moves)
            // The front is off: switch it on. No trips are added here.
            carga[f].op = true;
        }

        // --- 3. EVALUATION ---
        // Evaluate the toggled solution.
        long long nova_fo = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

        // Accept the move and return if the objective decreased.
        if (nova_fo < *fo_atual) {
            *fo_atual = nova_fo;
            delete[] visitas_backup;
            return true; // Found an improvement (First Improvement)
        }

        // --- 4. ROLLBACK ---
        // Undo the toggle: restore the flag, the production and the trip counts.
        carga[f].op = status_original;
        s[f] = s_original;
        for (int c = 0; c < ncam; c++) nvisitas[f][c] = visitas_backup[c];
        
        delete[] visitas_backup;
    }
    return false; // No improvement found by switching fronts off/on
}

// Neighbourhood TO (truck operation): for every operating front f and every truck c with trips
// there, removes all trips of truck c on front f and subtracts their mass from the production
// of f. First improvement; otherwise the trips and production are restored.
bool melhorVizinhoTO(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                     int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                     float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                     int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                     int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                     int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                     int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                     int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem,
                     long long *fo_atual) 
{
    // Walks each mining front
    // Outer loop: every front f.
    for (int f = 0; f < npilhas; f++) {
        
        // Only operating fronts are worth processing
        if (carga[f].op == false) continue;

        // Walks every truck in the fleet
        // Inner loop: every truck c.
        for (int c = 0; c < ncam; c++) {
            
            // Checks whether the truck has trips on this specific front
            // Only if truck c has trips on front f.
            if (nvisitas[f][c] > 0) {
                
                // --- 1. BACKUP OF THE STATE ---
                // Backup of the trip count and production of front f for the rollback.
                int visitas_originais = nvisitas[f][c];
                float s_original = s[f];

                // --- 2. APPLY THE MOVE (zero the trips of the truck on front f) ---
                // Mass removed = number of trips times the capacity of truck c.
                float massa_removida = visitas_originais * capcam[c];
                // Zero the trips of truck c on front f and subtract the removed mass from the production.
                nvisitas[f][c] = 0;
                s[f] -= massa_removida;

                // --- 3. EVALUATION ---
                // Evaluate the solution.
                long long nova_fo = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

                // Accept the move and return if the objective decreased.
                if (nova_fo < *fo_atual) {
                    *fo_atual = nova_fo;
                    return true; // Found an improvement (First Improvement)
                }

                // --- 4. ROLLBACK ---
                nvisitas[f][c] = visitas_originais;
                s[f] = s_original;
            }
        }
    }
    return false; // No improvement found by reducing the fleet front by front
}

// Neighbourhood SS (swap shovels): for every pair of fronts with different loaders (and not both
// without a loader), exchanges the loaders. Trips of trucks that are no longer compatible with
// the new loader are removed, and both productions are recomputed. First improvement; otherwise
// loaders, trip counts and productions are restored from backups.
bool melhorVizinhoSS(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                     int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                     float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                     int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                     int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                     int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                     int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                     int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem,
                     long long *fo_atual) 
{
    // Walks every pair of fronts to test the exchange of loaders
    // Outer loop over front pairs (f1, f2) with f1 < f2, each pair tested once.
    for (int f1 = 0; f1 < npilhas; f1++) {
        for (int f2 = f1 + 1; f2 < npilhas; f2++) {
            
            // Simple check: skip if neither front has a loader
            // Nothing to swap when neither front has a loader.
            if (carga[f1].equip == disponivel && carga[f2].equip == disponivel) continue;
            // Skip if the loaders are identical
            // Swapping identical loaders changes nothing, so the pair is skipped.
            if (carga[f1].equip == carga[f2].equip) continue;

            // --- 1. BACKUP OF THE STATE ---
            // Save the loaders and production of both fronts for the rollback.
            int equip_f1_orig = carga[f1].equip;
            int equip_f2_orig = carga[f2].equip;
            float s_f1_orig = s[f1];
            float s_f2_orig = s[f2];
            
            // Backup of the trip counts of both fronts.
            int *visitas_f1_backup = new int[ncam];
            int *visitas_f2_backup = new int[ncam];
            for(int c = 0; c < ncam; c++) {
                visitas_f1_backup[c] = nvisitas[f1][c];
                visitas_f2_backup[c] = nvisitas[f2][c];
            }

            // --- 2. APPLY THE MOVE (Swap Shovels) ---
            // Exchange the loaders of the two fronts.
            carga[f1].equip = equip_f2_orig;
            carga[f2].equip = equip_f1_orig;

            // Clear trips that are incompatible on f1
            // Remove trips of trucks on f1 that are not compatible with its new loader.
            for (int c = 0; c < ncam; c++) {
                if (nvisitas[f1][c] > 0 && compatibilidade[c][carga[f1].equip] != compativel) {
                    nvisitas[f1][c] = 0;
                }
            }
            // Clear trips that are incompatible on f2
            // Same as above, for front f2.
            for (int c = 0; c < ncam; c++) {
                if (nvisitas[f2][c] > 0 && compatibilidade[c][carga[f2].equip] != compativel) {
                    nvisitas[f2][c] = 0;
                }
            }

            // Update the local productions s[f]
            // Recompute the production of both fronts from the new loaders and trip counts.
            s[f1] = producaofrente(nvisitas, carga, capcam, compatibilidade, ncam, f1);
            s[f2] = producaofrente(nvisitas, carga, capcam, compatibilidade, ncam, f2);

            // --- 3. EVALUATION ---
            // Evaluate the new solution.
            long long nova_fo = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

            // Accept the move and return if the objective decreased.
            if (nova_fo < *fo_atual) {
                *fo_atual = nova_fo;
                delete[] visitas_f1_backup; delete[] visitas_f2_backup;
                return true; // Found an improvement
            }

            // --- 4. ROLLBACK ---
            // Restore the loaders of both fronts.
            carga[f1].equip = equip_f1_orig;
            carga[f2].equip = equip_f2_orig;
            // Restore the trip counts of both fronts from the backups.
            for (int c = 0; c < ncam; c++) {
                nvisitas[f1][c] = visitas_f1_backup[c];
                nvisitas[f2][c] = visitas_f2_backup[c];
            }
            // Restore the production of both fronts.
            s[f1] = s_f1_orig;
            s[f2] = s_f2_orig;
            
            // Free the backup arrays before the next pair.
            delete[] visitas_f1_backup; delete[] visitas_f2_backup;
        }
    }
    return false;
}


// Variable Neighbourhood Descent with a fixed neighbourhood order. Evaluates the starting
// solution, then tries neighbourhoods k = 1..8 in order: 1 NT, 2 TP, 3 TO, 4 PO, 5 TT, 6 ST,
// 7 L, 8 SS. After an improvement the search restarts at k = 1; otherwise k advances, and the
// descent stops when k passes 8. The solution arrays and *fo_atual are updated in place.
// Always returns 0; callers do not use the return value.
int VND_Referencia(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
        int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
        float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
        int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
        int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
        int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
        int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
        int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem) 
{
    // Index of the current neighbourhood (1 to 8).
    int k = 1;
    // Best objective value found so far (minimised); shared with the neighbourhood functions.
    long long fo_atual;

    // Initial evaluation of the solution coming from the Shaking step
    // Evaluate the starting solution to obtain the reference objective.
    fo_atual = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, 
        prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole,
        ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn,
        wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga,
        wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

    // Descent loop: keep going until neighbourhood 8 has also failed.
    while (k <= 8) {
        // True when the neighbourhood run in this step found an improvement.
        bool encontrou_melhoria = false;

        // Try to find the best local improvement in neighbourhood k
        // Run neighbourhood k (1 NT, 2 TP, 3 TO, 4 PO, 5 TT, 6 ST, 7 L, 8 SS).
        switch(k) {
            case 1: 
                encontrou_melhoria = melhorVizinhoNT(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem, &fo_atual); 
                break;
            case 2: 
                encontrou_melhoria = melhorVizinhoTP(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem, &fo_atual);                 
                break;
            case 3: 
                encontrou_melhoria = melhorVizinhoTO(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem, &fo_atual); 
                break;
            case 4: 
                encontrou_melhoria = melhorVizinhoPO(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem, &fo_atual); 
                break;
            case 5: 
                encontrou_melhoria = melhorVizinhoTT(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem, &fo_atual); 
                break;
            case 6: 
                encontrou_melhoria = melhorVizinhoST(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem, &fo_atual); 
                break;
            case 7: 
                encontrou_melhoria = melhorVizinhoL(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem, &fo_atual); 
                break;
            case 8: 
                encontrou_melhoria = melhorVizinhoSS(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem, &fo_atual); 
                break;
        }

        // Improvement found: restart from the simplest neighbourhood. Otherwise move on to k + 1.
        if (encontrou_melhoria) {
            k = 1; // Improved: go back to the simplest neighbourhood to refine
        } else {
            k++; // Did not improve: try the next, more complex neighbourhood
        }
    }
    return 0;
}

// Variable Neighbourhood Descent with a random neighbourhood order and adaptive weights.
// Repeats cycles until a cycle finds no improvement or 1000 cycles have run. Each cycle shuffles
// the neighbourhoods and tries them in that order, stopping at the first one that improves
// (first improvement), then starts a new cycle. Returns true if any improvement occurred.
// Solution arrays and the best objective are updated in place. pesos[] (weights) and falhas[]
// (consecutive failures) are updated during the search; the shuffle itself is uniform.
bool VND_RLP(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                    int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                    float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                    int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                    int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                    int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                    int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                    int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem) 
{
    // Evaluate the starting solution; this is the reference objective of the descent.
    long long fo_atual = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, 
        prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole,
        ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn,
        wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga,
        wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);
    // Set to true if any improvement occurs during the call (this is the return value).
    bool melhoria_global = false;
    
    // Vector with the neighbourhood IDs (1 to 8, as in the original switch)
    // Neighbourhood ids before shuffling, in this driver's numbering: 1 NT, 2 TT, 3 TP, 4 ST,
    // 5 L, 6 PO, 7 TO, 8 SS.
    int vizinhancas[] = {1, 2, 3, 4, 5, 6, 7, 8};
    // Selection weights, initially 10 for each neighbourhood.
    float pesos[] = {10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0};
    // Consecutive failure count of each neighbourhood.
    int falhas[] = {0, 0, 0, 0, 0, 0, 0, 0};

    // True while the last cycle still found an improvement.
    bool encontrou_no_ciclo = true;
    
    // Safety limit on the number of cycles (iter_count counts them).
    int max_iter_vnd = 1000; // Safety limit
    int iter_count = 0;

    // Repeat cycles while the last one improved and the cycle limit has not been reached.
    while (encontrou_no_ciclo && iter_count < max_iter_vnd) {
        // Assume no improvement in this cycle until one is found.
        encontrou_no_ciclo = false;

        // --- SHUFFLE BASED ON WEIGHTS ---
        // Here the neighbourhood order is drawn based on the weights
        // Random shuffle of the neighbourhood order: position i swaps with a random position in [i, 7].
        for (int i = 0; i < 8; i++) {
            int r = i + rand() % (8 - i);
            int temp = vizinhancas[i];
            vizinhancas[i] = vizinhancas[r];
            vizinhancas[r] = temp;
        }

        // Try the shuffled neighbourhoods in order.
        for (int i = 0; i < 8; i++) {
            int v = vizinhancas[i];
            bool melhorou = false;

            // Run neighbourhood v (calling its functions melhorVizinhoNT, melhorVizinhoTO, etc.)
            // Dispatch neighbourhood v (1 NT, 2 TT, 3 TP, 4 ST, 5 L, 6 PO, 7 TO, 8 SS).
            switch (v) {
                case 1: 
                melhorou = melhorVizinhoNT(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem, &fo_atual); 
                break;
            case 2: 
                melhorou = melhorVizinhoTT(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem, &fo_atual); 
                break;
            case 3: 
                melhorou = melhorVizinhoTP(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem, &fo_atual); 
                break;
            case 4: 
                melhorou = melhorVizinhoST(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem, &fo_atual); 
                break;
            case 5: 
                melhorou = melhorVizinhoL(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem, &fo_atual); 
                break;
            case 6: 
                melhorou = melhorVizinhoPO(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem, &fo_atual); 
                break;
            case 7: 
                melhorou = melhorVizinhoTO(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem, &fo_atual); 
                break;
            case 8: 
                melhorou = melhorVizinhoSS(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem, &fo_atual); 
                break;
            }

            // Improvement: reward this neighbourhood, reset its failure count, flag the cycle, and start
            // a new cycle with a new shuffle.
            if (melhorou) {
                pesos[v-1] += 2.0;    // Positive weight for improving
                falhas[v-1] = 0;      // Reset failure counter
                encontrou_no_ciclo = true;
                melhoria_global = true;
                break; // Go back to the first neighbourhood in the shuffled list
            } else {
                // No improvement: count one more consecutive failure for this neighbourhood.
                falhas[v-1]++;
                // After more than 3 consecutive failures, lower its weight by 0.5 (never below 1.0).
                if (falhas[v-1] > 3) { // If it fails X times
                    pesos[v-1] -= 0.5; // Penalty on the weight
                    if (pesos[v-1] < 1.0) pesos[v-1] = 1.0; // Minimum weight
                }
            }
        }
        // One cycle has finished.
        iter_count++;
    }

    return melhoria_global;
}