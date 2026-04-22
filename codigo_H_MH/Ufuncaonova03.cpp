// Sistema desenvolvido no PPGEM/EM/UFOP para a resolucao do
// sistema de transporte (estatico e dinamico) de material em uma mina a c√©u aberto
// visando ao atendimento de metas de qualidade e producao
// "Marcone Jamilson Freitas Souza e Felippe Pereira da Costa",
// Data mar√ßo/2003 - dezembro/2004 ,Ouro Preto/MG/BR

//---------------------------------------------------------------------------
//#include <vcl.h>
//#include <condefs.h>
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
#include "Hmovimento.h"
#include "Hsa.h"
#include "Hvnd.h"
#include "Hvns.h"
#include "hstruct.h"


//----------------------------------------------------------------------




#pragma hdrstop
#pragma argsused
/*******************************************************/
int main(int argc, char* argv[])
{

int metodo2=1;
for(int looper=1; looper<=120; looper++){		//////////////			INICIO FOR DO LOOPER		COLETA DE DADOS
srand(time(NULL));

int metodo=0;
unsigned long semente = 0;
int semente2 =0;

learqsemente("semente.dat",&semente,&metodo);
unsigned long seed = semente;




srand(seed);
semente2 = seed;


char nometeste[10]="PA";

if(estdin == 1)
        strcat(nometeste,"EC");

else
        strcat(nometeste,"DC");



strcat(nometeste,"02_");



char sementenome[6];
//itoa(semente,sementenome,10);
strcpy( sementenome, "1234");

char *inst = "samarco";

/*Informa√ß√µes das pilhas*/
int npilhas = 17;     // numero de pilhas
int npminerio = 12;   // numero de pilhas de min√©rio
int nvar = 10;        // numero de parametros a serem avaliados
float **teores;       // matriz (npminerio*nvar) contendo o teor dos parametros de controle contidos nas pilhas (%)
float *QTDisp;        // vetor contendo a quantidade de material disponivel nas pilhas (t)
int *nummax;          // numero maximo de caminhoes
int *nviagem;

/*Informa√ß√µes de produ√ß√£o*/
float remrec = 0.31;   // relacao esteril/minerio recomendada
float remmax = 0.4;   // relacao esteril/minerio recomendada
float remmin = 0.3;   // relacao esteril/minerio recomendada
float prodmax = 6800; // producao maxima (t)
float prodmin = 4800; // producao minima (t)
float prodrec = 5800; // producao recomendada (t)
float *ls;            // limite superior para o parametro de controle (%)
float *li;            // limite inferior para o parametro de controle (%)
float *rec;           // valor recomendado para o parametro de controle (%)
float tcontrole = 60.0; // tempo de controle (min)


/*Informa√ß√µes dos equipamentos*/
float *tdv;           // tempo de deslocamento vazio (min)
float *tdc;           // tempo de desllocamento carregado (min)
float *tcarga;        // tempo de carga (min)
float *tbasc;         // tempo de basculamento (min)
float *tciclo;        // tempo total de ciclo (min)
int **compatibilidade;// compatibilidade entre equipamento de carga e transporte

/*Carga*/
int ncarga = 8;       // numero de equipamentos de carga disponiveis
float *cargamax;      // capacidade maxima de carga (t/h)
float *cargamin;      // capacidade minima de carga (t/h)
int visitamin;        // numero minimo de visitas
int visitamax;        // numero maximo de vizitas

/*Transporte*/
int ncam = 30;        // numero de caminhoes disponiveis
float *capcam;        // capacidade do caminhao (t)
float *dispmec;


/*Variaveis de Sistema*/
float *s;              // vetor solucao com quantidades (t/h)
float *u;              //
float *cap;            //
int *penalidadesup,*penalidadeinf,*penalidadeteorinv;   // penalidades
float fo;              // funcao objetivo
struct alocarga *carga;            // vetor solucao com alocacao dos equipamentos de carga
int ncolunas;          //
int nviagemmax;        //
struct horarios **rotacam;         // matriz que retorna a sequencia de visitas dos caminhoes
int **nvisitas;        // matriz com o numero de visitas necessarias


/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/


/*Pesos da fun√ßao objetivo*/

int wfproducaoinv=10000;
int wfproducaoqualp=1000;
int wfproducaoqualn=1000;

int wfteor = 100;//as outras estao no iniciavetor *penalidade ...

int wfrem = 100;
int wfremqualp = 1;
int wfremqualn =1;

int wfprodutcaminv = 100;
int wfprodutcamqualp = 1;
int wfprodutcamqualn = 1;

int wfprodcargainv = 1000;
int wfprodcargaqual = 1;
int wfproducaocarga = 0;//n√£o √© utilizado

int wfproducaocamtotal=1;



int wfnumcam = 900000;
int wfestatico = 900000;
int wfnumcamutilizados =1;



int wfdist = 1;


/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

QTDisp = criavetorfloat(npilhas);
teores = criamatrizfloat(npilhas,nvar);
s = criavetorfloat(npilhas);
u = criavetorfloat(npilhas);
cap = criavetorfloat(npilhas);
ls = criavetorfloat(nvar);
li = criavetorfloat(nvar);
rec = criavetorfloat(nvar);
penalidadesup = criavetor(nvar);
penalidadeinf = criavetor(nvar);
penalidadeteorinv = criavetor(nvar);
carga = criavetoralocarga(npilhas);
cargamax = criavetorfloat(ncarga);
cargamin = criavetorfloat(ncarga);
tdv = criavetorfloat(npilhas);
tdc = criavetorfloat(npilhas);
tcarga = criavetorfloat(npilhas);
tbasc = criavetorfloat(npilhas);
tciclo = criavetorfloat(npilhas);
capcam = criavetorfloat(ncam);
dispmec = criavetorfloat(ncam);
compatibilidade = criamatriz(ncam,ncarga);
nvisitas = criamatriz(npilhas,ncam);
nummax = criavetor(npilhas);
nviagem = criavetor(npilhas);

/*****************************************************************************/

iniciavetoralocarga(carga,npilhas,disponivel,true);
iniciavetorfloat(cap,npilhas,1.0);
iniciavetorfloat(s,npilhas,0.0);
iniciamatrizfloat(teores,npilhas,nvar,0.0);
iniciavetorint(penalidadesup,nvar,1);
iniciavetorint(penalidadeinf,nvar,1);
iniciavetorint(penalidadeteorinv,nvar,100);
iniciamatrizint(compatibilidade,ncam,ncarga,0);
iniciamatrizint(nvisitas,npilhas,ncam,0);
iniciavetorint(nummax,npilhas,0);
iniciavetorfloat(dispmec,ncam,0.85);

//carga[5].op = false;
/*****************************************************************************/
learqmatriz("compcar.dat",compatibilidade);
learqvetorfloat("qtdcar.dat",QTDisp);
learqmatrizfloat("teorescar.dat",teores);
learqvetorfloat("tlcar.dat",li);
learqvetorfloat("tucar.dat",ls);
learqvetorfloat("trcar.dat",rec);
learqvetorfloat("cargamax.dat",cargamax);
learqvetorfloat("cargamin.dat",cargamin);
learqvetorfloat("tdv.dat",tdv);
learqvetorfloat("tdc.dat",tdc);
learqvetorfloat("tcarga.dat",tcarga);
learqvetorfloat("tbasc.dat",tbasc);
learqvetorfloat("capcam.dat",capcam);

/****************************************************************************/


/****************************************************************************/

carregatempociclo( npilhas, tciclo, tdv, tdc, tcarga, tbasc);
carreganummax( npilhas, nummax, tciclo, tcarga);
carreganviagem( npilhas, nviagem,tcontrole, tciclo);



nviagemmax   =  (int)( floor(tcontrole/menorfloat(tciclo,npilhas)));
visitamin = (int)( ceil(menorfloat(cargamin,ncarga)/maiorfloat(capcam,ncam)));
visitamax = (int)( floor(maiorfloat(cargamax,ncarga)/menorfloat(capcam,ncam)));

/*****************************************************************************/


clock_t inicioCPU, fimCPU; // marcador de tempo
        inicioCPU = clock();






        constroisolucaoinicialcarga(carga,s,ncarga,npilhas, nvisitas, capcam, compatibilidade,ncam);
//        constroisolucaoinicialvisitas02(s,carga,capcam,compatibilidade,nvisitas,nviagemmax,npilhas,ncam);

        constroisolucaoinicialvisitasale(s,carga,capcam,compatibilidade,nvisitas,nviagem,npilhas,ncam);

//55555555555555555555555555555555555	inicio
imprimerelatorio("teste.txt",metodo,seed,1,s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);


//getchar();
//55555555555555555555555555555555555 fim

/*
printf("VALOR DA NOVA FO %8d\n",favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem));
printf("TEMPERATURA %12.2f\n",temperatura(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem,nviagemmax,visitamax));

getchar();
*/


/*
for(int i=0;i<npilhas;i++)
    printf("Ates SA carga %2d op %2d  s[%2d] = %8.2f    -> carga maxima %8.2f\n",carga[i].equip,carga[i].op,i,s[i],cargamax[carga[i].equip]);



imprimerelatorio("VNDinv.rel",metodo,seed,s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

*/
//for(metodo = 1; metodo < 5; ++metodo){

//metodo = 4;
       switch(metodo2){
                //v1: Vers„o cl·ssica do VNS  combinada com a Vers„o cl·ssica do VND (VNDvC)
                case 1: VNSv1(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem
                                , metodo, inst);

                        strcat(nometeste,"VNSv1");

                break;
                //v2:  Vers„o Cl·ssica do VNS combinada com a Vers„o AleatÛria do VND (VNDvALE1)
                case 2: VNSv2(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem
                                , metodo, inst);

                        strcat(nometeste,"VNSv2");

                break;

                //v3: Vers„o aleatÛria do VNS combinada com a Vers„o cl·ssica do VND
                case 3: VNSv3(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem
                                , metodo, inst);

                        strcat(nometeste,"VNSv3");

                break;

                //v4: Vers„o aleatÛria do VNS combinada com a Vers„o AleatÛria do VND
                case 4: VNSv4(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem
                                , metodo, inst);

                        strcat(nometeste,"VNSv4");

                break;

                //v5: Vers„o aleatÛria com memoria do VNS combinada com a Vers„o cl·ssica do VND
                case 5: VNSv5(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem
                                , metodo, inst);

                        strcat(nometeste,"VNSv5");

                break;

                //v6: Vers„o aleatÛria com memoria do VNS combinada com a Vers„o aleatoria com memoria do VND
                case 6: VNSv6(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem
                                , metodo, inst);

                        strcat(nometeste,"VNSv6");

                break;

                //v7: Vers„o classica do VNS combinada com a Vers„o aleatoria com memoria do VND
                case 7: VNSv7(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem
                                , metodo, inst);

                        strcat(nometeste,"VNSv7");

                break;
                
                //v8: Vers„o aleatÛria do VNS combinada com Vers„o aleatoria com memoria do VND
                case 8: VNSv8(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem
                                , metodo, inst);

                        strcat(nometeste,"VNSv8");

                break;
                
                
                 //v9: Vers„o aleatÛria com memoria do VNS combinada com a Vers„o AleatÛria do VND
                case 9: VNSv9(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem
                                , metodo, inst);

                        strcat(nometeste,"VNSv9");

                break;
                
                
                 //v10: Vers„o adaptativa do VNS combinada com a Vers„o classica do VND
                case 10: VNSv10(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem
                                , metodo, inst);

                        strcat(nometeste,"VNSv10");

                break;
                
                
                 //v11: Vers„o adaptativa do VNS combinada com a Vers„o aleatoria do VND
                case 11: VNSv11(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem
                                , metodo, inst);

                        strcat(nometeste,"VNSv11");

                break;
                
                
                //v12: Vers„o adaptativa do VNS combinada com a Vers„o aleatoria do VND
                case 12: VNSv12(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
                                , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
                                , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
                                , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
                                , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
                                , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem
                                , metodo, inst);

                        strcat(nometeste,"VNSv12");

                break;

             /**/
             

       }
//}

strncat(nometeste,sementenome,10);
strcat(nometeste,".rel");

fimCPU = clock();
float tempo = (float)(fimCPU - inicioCPU)/CLOCKS_PER_SEC ;


imprimerelatorio(nometeste,metodo,semente,tempo,s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);
/**/
//getchar();//555555555555555555555555555555555555555555


//*****************************************************************************
//*****************************************************************************
//*****************************************************************************


liberavetorfloat(QTDisp);
liberamatrizfloat(teores,npilhas);
liberavetorfloat(cap);
liberavetorfloat(li);
liberavetorfloat(ls);
liberavetorfloat(rec);
liberavetor(penalidadesup);
liberavetor(penalidadeinf);
liberavetor(penalidadeteorinv);
liberavetoralocarga(carga);
liberavetorfloat(s);
liberavetorfloat(u);
liberavetorfloat(cargamax);
liberavetorfloat(cargamin);
liberavetor(nummax);
liberavetor(nviagem);

liberavetorfloat(tdv);
liberavetorfloat(tdc);
liberavetorfloat(tcarga);
liberavetorfloat(tbasc);
liberavetorfloat(tciclo);

liberavetorfloat(capcam);
liberavetorfloat(dispmec);

liberamatriz(compatibilidade, ncam);
liberamatriz(nvisitas, npilhas);
printf("\nfim......");
//getchar();

/////////////////////////////////////////////
if(looper%10 == 0){
	metodo2++;
}

}////////////////////////////////			FIM DO FOR DE LOOPER




return 0;
}
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
