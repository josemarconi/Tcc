// Neighbourhood moves used by the local searches to change a solution.
// Each function changes the solution in place: nvisitas (trips per front and truck),
// carga (per-front state: equip = loader index or disponivel, op = active flag) and
// s (production per front). Terms: front = mining face, truck = haul truck, trip = one
// truckload, capcam = truck capacity, nviagem = trip limit per front and truck.
// Called by Uvns.cpp, Uils.cpp and Uag.cpp.
// #include <vcl.h>
// #include <condefs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <limits.h>
#include <ctype.h>
#include <time.h>
#include <float.h>
#include <errno.h>
#include <math.h>
//----------------------------------------------------------------------
#include "Hutil.h"
#include "Hdefine.h"
#include "Hutilcarga.h"
#include "Hfavaliacao.h"
#include "Hsa.h"
#include "Hmovimento.h"

#include "hstruct.h"
//----------------------------------------------------------------------
//----------------------------------------------------------------------

// movimentoNumberTrips: random move that adds one trip or removes one trip for a random
// front f and a random truck c. Adding is allowed only while nvisitas[f][c] is below
// nviagem[f][c]. s[f] changes by +/- capcam[c].
void movimentoNumberTrips(int **nvisitas, int **nviagem, float *s, float *capcam, int npilhas, int ncam, struct alocarga *carga)
{
    // 1. Choose a random front and a random truck
    int f = rand() % npilhas;
    int c = rand() % ncam;

    // 2. Decide whether to add or subtract (0 = subtract, 1 = add)
    int decisao = rand() % 2;

    if (decisao == 0)
    { // SUBTRAIR
        //printf("Subtraindo uma viagem da frente %d e caminhão %d\n", f, c);
        if (nvisitas[f][c] > 0)
        {
            nvisitas[f][c] -= 1;
            s[f] -= capcam[c]; // Update the production of the front in vector s
        }
    }
    else
    { // SOMAR
        //printf("Somando uma viagem à frente %d e caminhão %d\n", f, c);
        // Check whether trips can still be added (shift limit)
        if (nvisitas[f][c] < nviagem[f][c])
        {
            nvisitas[f][c] += 1;
            s[f] += capcam[c]; // Update the production of the front in vector s

            // A front that receives a trip is switched on if it was inactive.
            if (carga[f].op == false)
            {
                carga[f].op = true;
            }
        }
    }
}

// movimentoLoad: random move that exchanges the loaders (carga[].equip) and the whole
// truck-trip rows nvisitas[f1][] and nvisitas[f2][] of two random fronts f1 and f2.
// Trips are then clamped to nviagem, and s[f1] and s[f2] are recomputed.
void movimentoLoad(struct alocarga *carga, int npilhas, int ncam, int **compatibilidade, int **nvisitas, float *s, float *capcam, int **nviagem)
{
    int f1 = rand() % npilhas;
    int f2 = rand() % npilhas;
    // Nothing to do when both random draws pick the same front.
    if (f1 == f2)
        return;

    // Swap the loaders between f1 and f2
    int aux_equip = carga[f1].equip;
    carga[f1].equip = carga[f2].equip;
    carga[f2].equip = aux_equip;

    // The trips are relocated together with the loaders
    // This means swapping the entire row of the visit matrix between f1 and f2
    for (int c = 0; c < ncam; c++)
    {
        int temp_v = nvisitas[f1][c];
        nvisitas[f1][c] = nvisitas[f2][c];
        nvisitas[f2][c] = temp_v;
    }
    // If the destination front received a loader and trips, it must be switched on
    // A front that now has a loader is switched on.
    if (carga[f1].equip != disponivel)
        carga[f1].op = true;
    if (carga[f2].equip != disponivel)
        carga[f2].op = true;

    // Check whether the truck can operate in the NEW front (distance/trip limit)
    // Limit the trips of every truck in f1 and f2 to the trip limit nviagem.
    for (int c = 0; c < ncam; c++)
    {
        if (nvisitas[f1][c] > nviagem[f1][c])
            nvisitas[f1][c] = nviagem[f1][c];
        if (nvisitas[f2][c] > nviagem[f2][c])
            nvisitas[f2][c] = nviagem[f2][c];
    }

    // Recompute the production (s) for both fronts
    // Recompute the production of both fronts from the new visit matrix (producaofrente).
    s[f1] = producaofrente(nvisitas, carga, capcam, compatibilidade, ncam, f1);
    s[f2] = producaofrente(nvisitas, carga, capcam, compatibilidade, ncam, f2);
}

// movimentoRelocateTripTruck: moves one trip of truck c from front f1 (origin) to front f2
// (destination). Applied only if c has a trip in f1, f2 is active, c is compatible with the
// loader of f2 and c is below its trip limit nviagem[f2][c]. s[f1] loses capcam[c], s[f2] gains it.
void movimentoRelocateTripTruck(int **nvisitas, int **nviagem, int f1, int f2, int c, float *s, float *capcam, int **compatibilidade, struct alocarga *carga)
{
    // f1: origin front (where the truck cancels the trip)
    // f2: destination front (where the truck makes the new trip)
    // c: the truck being relocated

    // 1. Feasibility checks
    // - Truck 'c' must have at least one visit in front 'f1'
    // - Truck 'c' must be compatible with the loader of front 'f2'
    // - Front 'f2' must be active (op == true)
    // - Truck 'c' must not exceed the trip limit allowed for front 'f2' (nviagem)

    if (nvisitas[f1][c] > 0)
    {
        if (carga[f2].op == true &&
            compatibilidade[c][carga[f2].equip] == compativel &&
            nvisitas[f2][c] < nviagem[f2][c])
        {

            // 2. Transfer the trip destination to the truck
            nvisitas[f1][c] -= 1; // Cancel in i
            nvisitas[f2][c] += 1; // Perform in j

            // 3. Update the production of the two fronts involved
            // As it is the same truck, the transferred mass is its capcam[c]
            s[f1] -= capcam[c];
            s[f2] += capcam[c];
        }
    }
}

// movimentoRelocateTripPit: moves one trip from truck c1 to truck c2 inside the same front f.
// Applied only if c1 has a trip in f, c2 is compatible with the loader of f and c2 is below
// nviagem[f][c2]. s[f] becomes s[f] - capcam[c1] + capcam[c2].
void movimentoRelocateTripPit(int **nvisitas, int **nviagem, int f, int c1, int c2, float *s, float *capcam, int **compatibilidade, struct alocarga *carga)
{
    // f: the mining front where both trucks work
    // c1: donor truck (loses one trip)
    // c2: receiving truck (gains one trip)

    // 1. Feasibility checks
    // - Truck c1 must have at least one visit in front f
    // - Truck c2 must be compatible with the loader allocated to front f
    // - Truck c2 must not exceed the maximum trip limit allowed for it (nviagem) in front f

    if (nvisitas[f][c1] > 0)
    {
        if (compatibilidade[c2][carga[f].equip] == compativel &&
            nvisitas[f][c2] < nviagem[f][c2])
        {

            // 2. Transfer the trip between the trucks in the same front
            nvisitas[f][c1] -= 1;
            nvisitas[f][c2] += 1;

            // 3. Update the production of front f
            // The mass changes if the trucks have different capacities (capcam)
            // Subtract the capacity of the truck that left and add that of the one that entered
            s[f] = s[f] - capcam[c1] + capcam[c2];
        }
    }
}

// movimentoPitOperation: toggles the active flag op of front f. Switching off clears all
// truck trips of f and sets s[f] to 0. Switching on only sets op to true (no trips are added).
void movimentoPitOperation(struct alocarga *carga, int f, int **nvisitas, float *s, int ncam)
{
    // 1. Toggle the operation status of front f
    if (carga[f].op == true)
    {
        // SWITCH OFF: the front stops operating
        carga[f].op = false;

        // Clear all truck visits in this front
        for (int c = 0; c < ncam; c++)
        {
            nvisitas[f][c] = 0;
        }
        // The production of the front is now zero
        s[f] = 0.0;
    }
    else
    {
        // SWITCH ON: the front is again able to operate
        // Note: when switched on, it starts with 0 visits.
        // Subsequent moves (such as N^NT or N^L) will fill the front.
        carga[f].op = true;
    }
}

// movimentoTruckOperation: removes all trips of truck c_id from front f (nvisitas[f][c_id] = 0)
// and subtracts the removed mass (trips * capcam[c_id]) from s[f]. Does nothing if there are no trips.
void movimentoTruckOperation(int **nvisitas, int f, int c_id, float *s, float *capcam) 
{
    // f: front drawn at random before calling the function
    // c_id: truck drawn at random before calling the function

    // 1. Check whether the truck has trips in THIS specific front
    if (nvisitas[f][c_id] > 0) 
    {
        // 2. Compute the mass that will be removed from this front only
        float massa_removida = nvisitas[f][c_id] * capcam[c_id];

        // 3. Set the number of trips in this specific cell to zero
        nvisitas[f][c_id] = 0;

        // 4. Update the production of front f in vector s
        s[f] -= massa_removida;
    }
}

// movimentoSwapTrips: moves one trip from truck c1 in front f1 to truck c2 in front f2.
// Applied only if c1 has a trip in f1, f2 is active, c2 is compatible with the loader of f2
// and c2 is below nviagem[f2][c2]. s[f1] loses capcam[c1], s[f2] gains capcam[c2].
void movimentoSwapTrips(int **nvisitas, int f1, int f2, int c1, int c2, float *s, float *capcam, int **compatibilidade, struct alocarga *carga, int **nviagem) 
{
    // f1, c1: origin front and truck of the trip
    // f2, c2: destination front and truck of the trip

    // 1. Feasibility checks
    // - Truck c1 must have at least one trip in front f1 to donate
    // - Truck c2 must be compatible with the loader of front f2
    // - Truck c2 must not exceed its trip limit (nviagem) in front f2
    // - Front f2 must be active (op == true)

    if (nvisitas[f1][c1] > 0) 
    {
        if (carga[f2].op == true && 
            compatibilidade[c2][carga[f2].equip] == compativel && 
            nvisitas[f2][c2] < nviagem[f2][c2]) 
        {
            // 2. Transfer exactly 1 trip
            nvisitas[f1][c1] -= 1; // c1 loses one trip in f1
            nvisitas[f2][c2] += 1; // c2 gains one trip in f2

            // 3. Update the production of the fronts involved
            // s[f1] decreases by the capacity of truck c1
            // s[f2] increases by the capacity of truck c2
            s[f1] -= capcam[c1];
            s[f2] += capcam[c2];
        }
    }
}
// movimentoSwapShovels: exchanges the loaders (carga[].equip) of fronts f1 and f2. Trips of
// trucks that are not compatible with the new loader are set to 0, and s[f1] and s[f2] are
// recomputed with producaofrente. Does nothing if both fronts have no loader.
void movimentoSwapShovels(struct alocarga *carga, int f1, int f2, int **nvisitas, int ncam, int **compatibilidade, float *s, float *capcam)
{
    // CHECK
    // Swapping only makes sense if at least one of the fronts has a real loader.
    // If both are 'disponivel' (-1), the swap is null.
    if (carga[f1].equip == disponivel && carga[f2].equip == disponivel) {
        return;
    }

    // 1. Swap the equipment (shovels) between fronts f1 and f2
    int aux_equip = carga[f1].equip;
    carga[f1].equip = carga[f2].equip;
    carga[f2].equip = aux_equip;

    // 2. Compatibility check for front f1
    // The trucks that were in f1 must now be compatible with the new equipment
    for (int c = 0; c < ncam; c++)
    {
        if (nvisitas[f1][c] > 0 && compatibilidade[c][carga[f1].equip] != compativel)
        {
            nvisitas[f1][c] = 0; // Zero incompatible trips
        }
    }

    // 3. Compatibility check for front f2
    for (int c = 0; c < ncam; c++)
    {
        if (nvisitas[f2][c] > 0 && compatibilidade[c][carga[f2].equip] != compativel)
        {
            nvisitas[f2][c] = 0; // Zero incompatible trips
        }
    }

    // 4. Update the production of the two fronts in vector s
    s[f1] = producaofrente(nvisitas, carga, capcam, compatibilidade, ncam, f1);
    s[f2] = producaofrente(nvisitas, carga, capcam, compatibilidade, ncam, f2);
}

// From here to the closing comment marker at the end of the file, the code is disabled
// (commented out) and not compiled. It keeps older versions of the moves (movimentovisita,
// movimentooperacao, movimentocarga, movimentoviagemfrente, movimentoviagemcam,
// movimentooperacaocam) and the best-neighbour searches (melhorvizinhocarga,
// melhorvizinhogeral, melhorvizinhonvisitas, melhorvizinhooperacao, melhorvizinhonviagemfrente,
// melhorvizinhonviagemcam, melhorvizinhooperacaocam).
/*
void movimentovisita(int **nvisitas,int **compatibilidade,struct alocarga *carga, int passo, int npilhas,int ncam, int *nviagem, int *pilha,int *caminhao, bool *tipomov,float *s, float *capcam)
{

bool ok;
int cont=0;

do{
*pilha = randint(0,npilhas-1);
*caminhao = randint(0,ncam-1);
*tipomov = randint(0,1);
passo = randint(1,passo);

ok = false;
cont++;

if(carga[*pilha].op == true)
if((carga[*pilha].equip != disponivel) && (compatibilidade[*caminhao][carga[*pilha].equip ] == compativel)){
       trocarnvisitas(&ok,nvisitas,  nviagem,  passo, *tipomov, *pilha, *caminhao);

}
}while((!ok)&&(cont<100000));
carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

}

//----------------------------------------------------------------------
void movimentooperacao(struct alocarga *carga,float *s,int npilhas,int *pilhaop,bool *tipomovop, int **nvisitas, float *capcam,int **compatibilidade,int ncam)
{
bool ok;
int cont=0;

do{
        *pilhaop = randint(0,npilhas-1);
        ok = false;
        cont++;
        if(carga[*pilhaop].equip != disponivel)
                ok =true;
}while((!ok)&&(cont<100000));

trocaroperacao( carga, s, *pilhaop, nvisitas, capcam, compatibilidade, ncam, npilhas);
carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

}
//----------------------------------------------------------------------
void movimentocarga(struct alocarga *carga,float *s,int npilhas,int *pilha1, int *pilha2, int **nvisitas, float *capcam,int **compatibilidade,int ncam)
{
int cont=0;
bool ok;
do{
*pilha1 = randint(0,npilhas-1);
*pilha2 = randint(0,npilhas-1);
cont++;
ok = false;
if(carga[*pilha1].equip != carga[*pilha2].equip ){
        if((carga[*pilha1].op != false) || (carga[*pilha2].op != false )){
                trocar(carga,s,*pilha1,*pilha2, nvisitas, capcam,compatibilidade,ncam);
                ok = true;
        }
}
}while((!ok)&&(cont<100000));
carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

}
//----------------------------------------------------------------------
int melhorvizinhocarga(float *s,float *li,float *ls,float *rec,int *penalidadesup
                             ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
                             ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
                             ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
                             ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
                             ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
                             , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem)
{
    int k1,k2;
    int   fomin, fooriginal, fovizinho;

    fooriginal = fomin = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);


    for (int i=0;i<npilhas;i++){
            for (int j=i+1;j<npilhas;j++){

                        trocar(carga,s,i,j, nvisitas, capcam,compatibilidade,ncam);
                fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

                    if (fovizinho < fomin){
                            k1 = i;
                                k2 = j;
                                fomin = fovizinho;
                    }
                    trocar(carga,s,j,i, nvisitas, capcam,compatibilidade,ncam);
                }

    }
    if (fomin < fooriginal)  trocar(carga,s,k1,k2, nvisitas, capcam,compatibilidade,ncam);
    return fomin;
}
//*******************************************************************************
int melhorvizinhogeral(float *s,float *li,float *ls,float *rec,int *penalidadesup
                             ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
                             ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
                             ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
                             ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
                             ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
                             , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem)
{
    int pilha1,pilha2;
    int   fomin, fooriginal, fovizinho;
    int pilha,caminhao,tipomov;
    int passo =1;
    bool ok;



    fooriginal = fomin = fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);



     for(int p1=0;p1<npilhas;p1++){
     for(int p2=p1+1;p2<npilhas;p2++){
     trocar(carga,s,p1,p2, nvisitas, capcam,compatibilidade,ncam);

    for (int i=0;i<npilhas;i++){
        if(carga[i].equip != disponivel)
            for (int j=0;j<ncam;j++){
                if(compatibilidade[j][carga[i].equip] == compativel)
                        for(int k=0;k<2;k++){
                                trocarnvisitas(&ok,nvisitas,  nviagem,  passo, k, i, j);
                                fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);


                            if (fovizinho < fomin && ok == true){
                                        pilha1 = p1;
                                        pilha2 = p2;
                                        pilha = i;
                                    caminhao = j;
                                        tipomov = k;
                                    fomin = fovizinho;
                                       // printf("***P1 %2d P2%2d Pilha%2d Cam%2d Tipomov %2d\n",pilha1,pilha2,pilha,caminhao,tipomov);
                            }

                                if(k==0&& ok==true)
                                    trocarnvisitas(&ok,nvisitas,  nviagem,  passo, 1, i, j);
                                if(k==1&& ok==true)
                                        trocarnvisitas(&ok,nvisitas,  nviagem,  passo, 0, i, j);
                        }
                }

    }

    trocar(carga,s,p2,p1, nvisitas, capcam,compatibilidade,ncam);
    }
    }
    if (fomin < fooriginal){
    trocar(carga,s,pilha1,pilha2, nvisitas, capcam,compatibilidade,ncam);
    trocarnvisitas(&ok,nvisitas,  nviagem,  passo, tipomov, pilha, caminhao);
    printf("***P1 %2d P2%2d Pilha%2d Cam%2d Tipomov %2d\n",pilha1,pilha2,pilha,caminhao,tipomov);
    }
    return fomin;


}
//----------------------------------------------------------------------

int melhorvizinhonvisitas(float *s,float *li,float *ls,float *rec,int *penalidadesup
                             ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
                             ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
                             ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
                             ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
                             ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
                             , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem)
{
    int fomin, fooriginal, fovizinho;
    int pilha,caminhao,tipomov;
    int passo = 1;
    bool ok;

    fooriginal = fomin = fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);


    for (int i=0;i<npilhas;i++){
        if(carga[i].equip != disponivel)
            for (int j=0;j<ncam;j++){
                if(compatibilidade[j][carga[i].equip] == compativel)
                        for(int k=0;k<2;k++){
                                trocarnvisitas(&ok,nvisitas,  nviagem,  passo, k, i, j);
                                fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

                            if (fovizinho < fomin && ok == true){

                                    pilha = i;
                                    caminhao = j;
                                        tipomov = k;
                                    fomin = fovizinho;
                                        //printf("***Pilha%2d Cam%2d Tipomov %2d\n",pilha,caminhao,tipomov);
                            }

                                if(k==0&& ok==true)
                                    trocarnvisitas(&ok,nvisitas,  nviagem,  passo, 1, i, j);
                                if(k==1&& ok==true)
                                        trocarnvisitas(&ok,nvisitas,  nviagem,  passo, 0, i, j);
                        }
                }

    }
    if (fomin < fooriginal){
        trocarnvisitas(&ok,nvisitas,  nviagem,  passo, tipomov, pilha, caminhao);
        //printf("***Pilha%2d Cam %2d Tipomov %2d\n",pilha,caminhao,tipomov);
    }
    return fomin;
}
//----------------------------------------------------------------------
int melhorvizinhooperacao(float *s,float *li,float *ls,float *rec,int *penalidadesup
                             ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
                             ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
                             ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
                             ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
                             ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
                             , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem)
{
    int k1;
    int   fomin, fooriginal, fovizinho;

    fooriginal = fomin = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);


    for (int i=0;i<npilhas;i++){

                        trocaroperacao( carga, s, i, nvisitas, capcam, compatibilidade, ncam, npilhas);

                fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

                    if (fovizinho < fomin){
                            k1 = i;
                                fomin = fovizinho;
                    }
                    trocaroperacao( carga, s, i, nvisitas, capcam, compatibilidade, ncam, npilhas);


    }
    if (fomin < fooriginal)  trocaroperacao( carga, s, k1, nvisitas, capcam, compatibilidade, ncam, npilhas);
    return fomin;
}
//----------------------------------------------------------------------
void movimentoviagemfrente(int **nvisitas,int **compatibilidade,struct alocarga *carga, int passo, int npilhas,int ncam, int *nviagem, int *pilha,int *caminhao1,int *caminhao2,float *s, float *capcam)
{

bool ok;
int cont =0;

do{
*pilha = randint(0,npilhas-1);
*caminhao1 = randint(0,ncam-1);
*caminhao2 = randint(0,1);
passo = randint(1,passo);
cont++;

ok = false;

if(carga[*pilha].op == true)
if((carga[*pilha].equip != disponivel) && (*caminhao1 != *caminhao2))
        if( compatibilidade[*caminhao1][carga[*pilha].equip ] == compativel&& compatibilidade[*caminhao2][carga[*pilha].equip ] == compativel)
                if(((nvisitas[*pilha][*caminhao1] + passo) <= nviagem[*pilha]) && ((nvisitas[*pilha][*caminhao2] - passo) >= 0)){
                        trocarviagemcaminhaofrente(nvisitas, *pilha, *caminhao1, *caminhao2,passo);
                        ok = true;

                }
}while((!ok) && (cont<100000));
carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

}

//----------------------------------------------------------------------
void movimentoviagemcam(int **nvisitas,int **compatibilidade,struct alocarga *carga, int passo, int npilhas,int ncam, int *nviagem, int *cam,int *pilha1,int *pilha2,float *s, float *capcam)
{

bool ok;
int cont=0;

do{
*pilha1 = randint(0,npilhas-1);
*pilha2 = randint(0,npilhas-1);
*cam = randint(0,ncam-1);
passo = randint(1,passo);
cont++;
ok = false;

if((carga[*pilha1].op == true) && (carga[*pilha2].op == true))
if(*pilha1 != *pilha2)
if((carga[*pilha1].equip != disponivel) && (carga[*pilha2].equip != disponivel))
        if(( compatibilidade[*cam][carga[*pilha1].equip ] == compativel) && (compatibilidade[*cam][carga[*pilha2].equip ] == compativel))
                if(((nvisitas[*pilha1][*cam] + passo) <= nviagem[*pilha1]) && ((nvisitas[*pilha2][*cam] - passo) >= 0)){
                        trocarviagemcaminhaocam(nvisitas, *cam, *pilha1, *pilha2,passo);
                        ok = true;

                }
}while((!ok)&&(cont<100000));
carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

}
//----------------------------------------------------------------------
void movimentooperacaocam(int **nvisitas,int **compatibilidade,struct alocarga *carga, int passo, int npilhas,int ncam, int *nviagem, int *cam,int *pilha,float *s, float *capcam)
{

bool ok;
int cont =0;

do{
*pilha = randint(0,npilhas-1);
*cam = randint(0,ncam-1);
cont++;
ok = false;

if((carga[*pilha].op == true) && (carga[*pilha].equip != disponivel))
        if( compatibilidade[*cam][carga[*pilha].equip ] == compativel && nvisitas[*pilha][*cam] != 0){
                        nvisitas[*pilha][*cam] = 0;
                        ok = true;

                }
}while((!ok) && (cont < 100000));
carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);

}
//----------------------------------------------------------------------
int melhorvizinhonviagemfrente(float *s,float *li,float *ls,float *rec,int *penalidadesup
                             ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
                             ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
                             ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
                             ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
                             ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
                             , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem)
{
    int fomin, fooriginal, fovizinho;
    int pilha,caminhao1,caminhao2;
    int passo = 1;

    fooriginal = fomin = fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);


    for (int i=0;i<npilhas;i++){
        if(carga[i].equip != disponivel && carga[i].op == true)
            for (int j=0;j<ncam;j++){
                        for(int k = 0; k<ncam;k++){
                                if(j==k)break;
                                if(compatibilidade[j][carga[i].equip] == compativel && compatibilidade[k][carga[i].equip] == compativel)
                                if((nvisitas[i][j] + passo) <= nviagem[i] && (nvisitas[i][k] - passo) >= 0){
                                //printf("*** %2d  -> %2d\n", nvisitas[i][j],nvisitas[i][k]);
                                trocarviagemcaminhaofrente(nvisitas, i, j, k,passo);
                                fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

                            if (fovizinho < fomin){

                                    pilha = i;
                                    caminhao1 = j;
                                    caminhao2 = k;
                                    fomin = fovizinho;

                            }
                                //printf("*** %2d  -> %2d\n", nvisitas[i][j],nvisitas[i][k]);
                                trocarviagemcaminhaofrente(nvisitas, i, k, j,passo);
                                //printf("aaa %2d  -> %2d\n", nvisitas[i][j],nvisitas[i][k]);getchar();
                                }
                        }
                }

    }
    if (fomin < fooriginal){
                trocarviagemcaminhaofrente(nvisitas, pilha, caminhao1, caminhao2,passo);
//                printf("FINAL  Pilha %2d Cam1 %2d Cam2 %2d ------- %2d  -> %2d\n",pilha,caminhao1,caminhao2,nvisitas[pilha][caminhao1],nvisitas[pilha][caminhao2]);getchar();
    }
    return fomin;
}
//----------------------------------------------------------------------
//----------------------------------------------------------------------
int melhorvizinhonviagemcam(float *s,float *li,float *ls,float *rec,int *penalidadesup
                             ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
                             ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
                             ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
                             ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
                             ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
                             , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem)
{
    int fomin, fooriginal, fovizinho;
    int cam,pilha1,pilha2;
    int passo = 1;

    fooriginal = fomin = fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);


    for (int i=0;i<ncam;i++){
                for (int j=0;j<npilhas;j++){
                        if(carga[j].equip != disponivel && carga[j].op == true)
                                for(int k = 0; k<npilhas;k++){
                                if(j==k)break;
                                if(compatibilidade[i][carga[j].equip] == compativel && compatibilidade[i][carga[k].equip] == compativel)
                                if(carga[k].equip != disponivel && carga[k].op == true){
                                        if((nvisitas[j][i] + passo) <= nviagem[j] && (nvisitas[k][i] - passo) >= 0){
                                trocarviagemcaminhaocam(nvisitas, i, j, k,passo);
                                fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

                            if (fovizinho < fomin){

                                    cam = i;
                                    pilha1 = j;
                                    pilha2 = k;
                                    fomin = fovizinho;
                                        //printf("***Pilha%2d Cam%2d Tipomov %2d\n",pilha,caminhao,tipomov);
                            }
                                trocarviagemcaminhaocam(nvisitas, i, k, j,passo);
                                }
                                }
                        }
                }

    }
    if (fomin < fooriginal){
                trocarviagemcaminhaocam(nvisitas, cam, pilha1, pilha2,passo);
//        printf("***Cam%2d Pilha1 %2d Pilha2 %2d\n",cam,pilha1,pilha2);getchar();
    }
    return fomin;
}
//----------------------------------------------------------------------
int melhorvizinhooperacaocam(float *s,float *li,float *ls,float *rec,int *penalidadesup
                             ,int *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
                             ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
                             ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
                             ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
                             ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
                             , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem)
{
    int fomin, fooriginal, fovizinho;
    int cam,pilha,aux;

    fooriginal = fomin = fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);


    for (int i=0;i<npilhas;i++){
                if(carga[i].equip != disponivel && carga[i].op == true)
                                for(int j = 0; j<ncam;j++){
                                        if(compatibilidade[j][carga[i].equip] == compativel && nvisitas[i][j] != 0){
                                        aux = nvisitas[i][j];
                                        nvisitas[i][j] = 0;
                                fovizinho = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

                            if (fovizinho < fomin){

                                        pilha = i;
                                    cam = j;
                                    fomin = fovizinho;
                                        //printf("***Pilha%2d Cam%2d Tipomov %2d\n",pilha,caminhao,tipomov);
                            }
                                nvisitas[i][j] = aux;
                                }
                                }



    }
    if(fomin < fooriginal){
                nvisitas[pilha][cam] = 0;
//        printf("***Cam% 2d Pilha %2d \n",cam,pilha);getchar();
    }
    return fomin;
}

*/