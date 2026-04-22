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
#include "Hsa.h"
#include "Hmovimento.h"

#include "hstruct.h"

//----------------------------------------------------------------------
struct alocarga *criavetoralocarga(int tam)
{
  struct alocarga *vetor;

  vetor = (struct alocarga *) malloc(tam*sizeof(struct alocarga));
  if (!vetor) Erro("Falta memoria para alocar o vetor de ponteiros");
  return vetor;
}
//----------------------------------------------------------------------
void carregavetors(float *s, int **nvisitas, struct alocarga *carga, float *capcam,int **compatibilidade,int ncam, int npilhas)
{
        for(int i=0;i<npilhas;i++){
                s[i]= producaofrente(nvisitas,carga,capcam,compatibilidade, ncam, i);
        }
}

//----------------------------------------------------------------------
void constroisolucaoinicialcarga(struct alocarga *carga,float *s, int ncarga,int npilhas,int **nvisitas,float *capcam,int **compatibilidade,int ncam)
{
//       printf("Construindo solucao inicial aleatoria para a alocação de equipamentos de carga...\n");

        for(int i=0;i<ncarga;i++){
                carga[i].equip=i;
        }

//        for(int i=0;i<npilhas;i++){
//                printf("%d\n",carga[i].equip);
//        }getchar();


        for(int i=0;i<npilhas;i++){
                int p1 = randint(0,npilhas-1);
                int p2 = randint(0,npilhas-1);
                carga[i].op = true;
                trocar(carga,s,p1,p2, nvisitas, capcam,compatibilidade,ncam);

        }

//        for(int i=0;i<npilhas;i++){
//                printf("%d\n",carga[i].equip);
//        }getchar();



        carregavetors(s, nvisitas, carga, capcam, compatibilidade, ncam, npilhas);

}
//----------------------------------------------------------------------
void constroisolucaoinicialvisitasale(float *s, struct alocarga *carga, float *capcam, int **compatibilidade, int **nvisitas,int *nviagem, int npilhas, int ncam)
{
        for(int i=0;i<npilhas;i++)
                for(int j=0;j<ncam;j++)
                        nvisitas[i][j]= randint(0,nviagem[i]);

        carregavetors(s, nvisitas, carga, capcam, compatibilidade, ncam, npilhas);

}
void constroisolucaoinicialvisitas02(float *s, struct alocarga *carga, float *capcam, int **compatibilidade, int **nvisitas,int nviagemmax, int npilhas, int ncam)
{

        for(int i=0;i<npilhas;i++){
                printf("%d - %f\n",carga[i].equip,s[i]);

        }getchar();

int nvdisp = 0;
for(int j=0;j<ncam;j++){
        nvdisp = nviagemmax;
        for(int i=0;i<npilhas;i++){
                if(carga[i].equip !=disponivel && carga[i].op == true){
                        if(compatibilidade[j][carga[i].equip]==compativel){
                                nvisitas[i][j]= randint(0,nvdisp);
                                nvdisp -= nvisitas[i][j];
                                if(nvdisp == 0)break;
                        }
                }
        }
}

        carregavetors(s, nvisitas, carga, capcam, compatibilidade, ncam, npilhas);


        for(int i=0;i<npilhas;i++){
                printf("%d - %f\n",carga[i].equip,s[i]);
        }getchar();

}

//----------------------------------------------------------------------
void constroisolucaoinicialvisitas(float *s, struct alocarga *carga, float *capcam, int **compatibilidade,int **nvisitas,int *nviagem, int npilhas, int ncam, int nviagemax)
{
int escolha;
bool ok = false;
                for(int j=0;j<ncam;j++){
                        do{
                        escolha = randint(0,npilhas-1);
                        ok = false;
                        if(carga[escolha].equip!=disponivel && carga[escolha].op ==true)
                                nvisitas[escolha][j]= nviagemax;
                                ok=true;
                        }while(!ok);
                }
        carregavetors(s, nvisitas, carga, capcam, compatibilidade, ncam, npilhas);

}
//----------------------------------------------------------------------
//----------------------------------------------------------------------

float encontrado(float *s, float **teores,int npminerio, int varj)
{
float soma1=0;
float soma2=0;

for(int i=0;i<npminerio;i++){
        soma1 += teores[i][varj]*s[i];
        soma2 += s[i];
}
if (soma2 == 0)
    return 0;

return soma1/soma2;

}

//----------------------------------------------------------------------
void iniciavetoralocarga(struct alocarga *vetor,int tam,int valor1, bool valor2)
{
        for(int i=0;i<tam;i++){
                vetor[i].equip = valor1;
                vetor[i].op = valor2;
        }
}

//----------------------------------------------------------------------
void liberavetoralocarga(struct alocarga *vetor)

{
  free(vetor);
}
//----------------------------------------------------------------------
float produtcam(int **nvisitas, struct alocarga *carga, float *capcam,int **compatibilidade, float *tciclo,float tcontrole,int npilhas, int cam)
{
float soma = 0.0;

        for(int i=0;i<npilhas;i++)
                if(carga[i].equip!=disponivel && carga[i].op == true)
                        if(compatibilidade[cam][carga[i].equip])
                                          soma+=nvisitas[i][cam]*tciclo[i];
       if(soma==0)return 0.0;
       if(tcontrole==0)return 0.0;


return  soma/tcontrole;
}
//----------------------------------------------------------------------
int dinamico(int **nvisitas, struct alocarga *carga, int **compatibilidade, int npilhas,int cam)
{
int soma=0;
for(int i=0;i< npilhas; i++){
        if(carga[i].equip != disponivel && carga[i].op == true)
        if(compatibilidade[cam][carga[i].equip] == compativel){
                if(nvisitas[i][cam] > 0){
                        soma++;

                }
        }
}
if(soma>0)
        return (soma-1);
else
        return 0;

}
//----------------------------------------------------------------------
float producaofrente(int **nvisitas, struct alocarga *carga, float *capcam,int **compatibilidade,int ncam, int frente)
{
float soma = 0.0;

        if(carga[frente].op == false)
                return 0.0;

        if(carga[frente].equip == disponivel)
                return 0.0;
        else{
                for(int k=0;k<ncam;k++)
                        if(compatibilidade[k][carga[frente].equip]==compativel)
                                soma+=nvisitas[frente][k]*capcam[k];

        }
return  soma;
}
//----------------------------------------------------------------------
void trocar(struct alocarga *carga, float *s,int pilha1,int pilha2, int **nvisitas, float *capcam,int **compatibilidade,int ncam)
{

int aux = carga[pilha1].equip;
int auxop = carga[pilha1].op;
int auxs = s[pilha1];
int *vetor = criavetor(ncam);
for(int k=0;k<ncam;k++)
        vetor[k]=nvisitas[pilha1][k];

carga[pilha1].equip = carga[pilha2].equip;
carga[pilha1].op = carga[pilha2].op;
s[pilha1]=s[pilha2];
for(int k=0;k<ncam;k++)
        nvisitas[pilha1][k]=nvisitas[pilha2][k];

carga[pilha2].equip=aux;
carga[pilha2].op=auxop;
s[pilha2]=auxs;
for(int k=0;k<ncam;k++)
        nvisitas[pilha2][k]=vetor[k];

liberavetor(vetor);

}
//----------------------------------------------------------------------
void trocarnvisitas(bool *ok, int **nvisitas, int *nviagem, int passo,bool tipomov,int pilha, int caminhao)
{
*ok=false;
if(tipomov == false){
    if(nvisitas[pilha][caminhao] + passo <= nviagem[pilha]){
        nvisitas[pilha][caminhao]+= passo;
     *ok = true;
    }
}
else {
    if(nvisitas[pilha][caminhao]-passo >= 0){
        nvisitas[pilha][caminhao]+= -passo;
        *ok =true;
    }

}
}
//----------------------------------------------------------------------
void trocaroperacao(struct alocarga *carga, float *s,int pilha, int **nvisitas, float *capcam,int **compatibilidade,int ncam,int npilhas)
{

        if(carga[pilha].op == false)
                carga[pilha].op = true;
        else
                carga[pilha].op = false;

        carregavetors(s, nvisitas, carga, capcam ,compatibilidade,ncam, npilhas);


}
//----------------------------------------------------------------------
void trocarviagemcaminhaofrente(int **nvisitas, int pilha, int caminhao1, int caminhao2,int passo)
{
        nvisitas[pilha][caminhao1] += passo;
        nvisitas[pilha][caminhao2] -= passo;

}
//----------------------------------------------------------------------
void trocarviagemcaminhaocam(int **nvisitas, int cam, int pilha1, int pilha2,int passo)
{
        nvisitas[pilha1][cam] += passo;
        nvisitas[pilha2][cam] -= passo;

}
//----------------------------------------------------------------------
void atualizamelhorsolucaoalocarga(struct alocarga *s,struct alocarga *star,int tam)
{
        for(int i=0;i<tam;i++){
                s[i].equip = star[i].equip;
                s[i].op = star[i].op;
}

}
//----------------------------------------------------------------------
int numcamfrente(int **nvisitas, struct alocarga *carga, int **compatibilidade, int ncam,int pilha, int *nummax)
{
int soma=0;
if(carga[pilha].equip == disponivel)return 0;
for(int k=0;k< ncam; k++){
        if(compatibilidade[k][carga[pilha].equip]== compativel && nvisitas[pilha][k]!=0){
                soma++;
        }
}
return soma;
}
//----------------------------------------------------------------------
void imprimerelatorio(char nomearq[],int metodo,unsigned long seed,float tempo,float *s,float *li,float *ls,float *rec,int *penalidadesup,int
        *penalidadeinf,int *penalidadeteorinv,int npilhas, int nvar,float **teores,float prodmax
        ,float prodmin, float prodrec,int npminerio,float remrec,float remmax,float remmin,int **nvisitas, struct alocarga *carga, float *capcam
        ,int **compatibilidade, float *tciclo,float tcontrole, int ncam, float *cargamax, float *cargamin, int *nummax
        ,int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn,int wfteor
        ,int wfrem,int wfremqualp,int wfremqualn,int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn
        , int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal,int wfproducaocarga,int wfnumcamutilizados,int wfestatico,int wfnumcam, int wfdist, float *dispmec, int *nviagem)
{
  register int i,j,k;
  FILE *arquivo;
  int *camutil = criavetor(ncam);
  bool viavel;
  float enc=0.0;
  float auxcam = 0.0;
  int totalcam = 0;
  float soma1=0.00000001;
  float soma2=0;
  float utilizacao;
        arquivo = fopen(nomearq,"w");
        if (!arquivo) Erro("O arquivo nome nao pode ser aberto");
  float producaomin = prodtotal(s,npminerio);
  float producaototal = prodtotal(s,npilhas);
  float rem;
  int fo=0;
  float soma3 =  0.0;
  float soma4 = 0.0;

  fo = favaliacao(s,li,ls,rec,penalidadesup, penalidadeinf,penalidadeteorinv, npilhas, nvar, teores, prodmax
        , prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam
        , compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax
        , wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor
        , wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn
        , wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec,nviagem);

for(j=0;j<nvar;j++){
        for(i=0;i<npminerio;i++){
                soma3+=((teores[i][j]-rec[j])*s[i]);

    }
    soma4+=absfloat(soma3);
    soma3=0;

}







if(estdin==1)
fprintf(arquivo,"Relatório para a alocacao ESTATICA de caminhões utilizando o metodo ");
else
fprintf(arquivo,"Relatório para a alocacao DINAMICA de caminhões utilizando o metodo ");

if(metodo==1)
fprintf(arquivo,"VND");
else
fprintf(arquivo,"VNS");

//fprintf(arquivo,"Seed: %d\nAbs: %d\nSoma: %f\nTempo: %f\nFo: %d",seed,abs(producaomin-prodrec),soma4,tempo,fo);
//fprintf(arquivo,"\n\n\nSeed: %d\n",seed;
//fprintf(arquivo,"Abs: %d",seed,abs(producaomin-prodrec),soma4,tempo,fo);
//fprintf(arquivo,"Soma: %f\n",seed,abs(producaomin-prodrec),soma4,tempo,fo);
fprintf(arquivo,"\n\n\nTempo: %f\n",tempo);
fprintf(arquivo,"Fo: %d\n\n\n",fo);


if(producaomin!=0){
        fprintf(arquivo,"Producao de Minerio %8.2f  [%8.1f -%8.1f -%8.1f]\n",producaomin,prodmin,prodrec,prodmax);
        fprintf(arquivo,"Producao de Esteril %8.2f\n",producaototal-producaomin);
        rem = (producaototal-producaomin)/producaomin;
        fprintf(arquivo,"                REM %8.2f  [%8.2f -%8.2f -%8.2f]\n",rem,remmin,remrec,remmax);
}
/*else{
            fprintf(arquivo,"Não foi produzido minerio....\n");
            liberavetor(camutil);
            fclose(arquivo);
            exit(1);
}*/

fprintf(arquivo," \nComponentes da Funcao de Avaliacao\n\n\n");

fprintf(arquivo,"\t Favaliacao  %12d\n",fo);
fprintf(arquivo,"\t--------------------------\n");

fprintf(arquivo,"\t fa_producao %12d\n",fa_producao(producaomin,prodmax,prodmin,prodrec,wfproducaoinv,wfproducaoqualp,wfproducaoqualn));
int auxteor = 0;
        for(int j=0;j<nvar;j++){
                auxteor += fa_teor(encontrado(s,teores,npminerio,j), ls[j], li[j], rec[j], penalidadeteorinv[j], penalidadesup[j], penalidadeinf[j]);
        }
fprintf(arquivo,"\t fa_teor     %12d\n",auxteor);

fprintf(arquivo,"\t fa_rem      %12d\n", fa_rem( rem, remmax, remmin, remrec, wfrem, wfremqualp, wfremqualn));

int auxpcam = 0;
        for(int l=0;l<ncam;l++){
                auxpcam += fa_produtcam(produtcam(nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, npilhas, l)
                                  ,1.0, 0.0, dispmec[l], wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn);

        }
fprintf(arquivo,"\t fa_produtcam%12d\n",auxpcam);


int auxcarga = 0;
        for(int i=0;i<npilhas;i++){

                auxcarga += fa_prodcarga(s[i], cargamax[carga[i].equip], cargamin[carga[i].equip], wfprodcargainv, wfprodcargaqual);
        }
fprintf(arquivo,"\t fa_prodcarga%12d\n",auxcarga);
fprintf(arquivo,"\t fa_totalcam %12d\n",fa_numcam(nvisitas, carga, compatibilidade, npilhas, ncam,wfnumcamutilizados));

fprintf(arquivo,"\t fa_estatico %12d\n",fa_estatico(nvisitas, carga, compatibilidade, npilhas, ncam, wfestatico));
fprintf(arquivo,"\t fa_numcamfre%12d\n",fa_numcamfrente( nvisitas, carga, compatibilidade, npilhas, ncam, wfnumcam, nummax));
fprintf(arquivo,"\t--------------------------\n");








//******************************************************************************
fprintf(arquivo,"\n\n\nEquipamentos de Carga\n\n\n");

fprintf(arquivo,"\t Nº    Frente   [Prod Min - Prod Max]   Ritmo de Lavra     Pontuacao   STATUS\n");
fprintf(arquivo,"\t------------------------------------------------------------------------------\n");

for( i=0;i<npilhas;i++){
        if(carga[i].equip!= disponivel && s[i] != 0){
                fprintf(arquivo,"\t %2d       %2d    [%8.1f  -%8.1f]     %8.1f    %10d",carga[i].equip,i,cargamin[carga[i].equip],cargamax[carga[i].equip],s[i],fa_prodcarga(s[i], cargamax[carga[i].equip], cargamin[carga[i].equip], wfprodcargainv, wfprodcargaqual));
                if(s[i]>=cargamin[carga[i].equip] && s[i] <= cargamax[carga[i].equip])
                        fprintf(arquivo,"         ok!\n");
                else{
                        viavel = false;
                        fprintf(arquivo,"         INV\n");
                }
        }

}
fprintf(arquivo,"\t------------------------------------------------------------------------------\n");

//******************************************************************************
fprintf(arquivo,"Parâmetros de Controle\n\n\n");
fprintf(arquivo,"\t Parametro [  Minimo -   Meta  -  Maximo]   Encontrado    Diferença    Pontuacao  STATUS\n");
fprintf(arquivo,"\t-----------------------------------------------------------------------------------------\n");

for(j=0;j<nvar;j++){
        enc = encontrado(s, teores,npminerio, j);
    fprintf(arquivo,"\t  %2d       [%8.4f -%8.4f -%8.4f]   %8.4f     %8.4f %10d",j,li[j],rec[j],ls[j],enc,enc-rec[j],fa_teor(encontrado(s,teores,npminerio,j), ls[j], li[j], rec[j], penalidadeteorinv[j], penalidadesup[j], penalidadeinf[j]));
    if(enc>=li[j] && enc <= ls[j])
                        fprintf(arquivo,"        ok!\n");
                else{
                        viavel = false;
                        fprintf(arquivo,"        INV\n");
                }

}
fprintf(arquivo,"\t-----------------------------------------------------------------------------------------\n");

/******************************************************************************
fprintf(arquivo,"\n\n\nNúmero de viagens realizados às frentes pelos caminhões\n\n\n");

for(int l = 0; l< ncam; l++ )
        camutil[l]=camutilizados(nvisitas,carga,compatibilidade, npilhas, l);



fprintf(arquivo,"\n\tCaminhao |");

for(int i=0;i<npilhas;i++){
        if(carga[i].equip!=disponivel && carga[i].op != false)
        if(s[i] != 0){
                fprintf(arquivo,"%4d",i);
        }
}
fprintf(arquivo," |  Utilizacao  disponibilidade  Pontuacao  STATUS");

fprintf(arquivo,"\n\t----------");
for(int i=0;i<npilhas;i++){
        if(carga[i].equip!=disponivel && carga[i].op != false)
        if(s[i] != 0){
                fprintf(arquivo,"----");
        }
}
fprintf(arquivo,"---------------------------------------------------");
fprintf(arquivo,"\n");
for(int k=0;k<ncam;k++){
        totalcam += camutil[k];
}

for(int k=0;k<ncam;k++){
        if(camutil[k]!=0){
                fprintf(arquivo,"\t%4d     |", k);
                for(int i=0;i<npilhas;i++){
                        if(carga[i].equip!=disponivel && carga[i].op != false)
                        if(s[i] != 0){
                                if(compatibilidade[k][carga[i].equip]!=compativel ){
                                        fprintf(arquivo,"%4s", ".");
                                }else{
                                        if(nvisitas[i][k]==0 ){
                                                fprintf(arquivo,"%4s", ".");
                                        }else{
                                                fprintf(arquivo,"%4d", nvisitas[i][k]);}
                                        }
                                }
                        }
                        auxcam = produtcam(nvisitas,carga, capcam,compatibilidade, tciclo, tcontrole, npilhas, k);
                        fprintf(arquivo," |    %5.1f",100*auxcam);
                        fprintf(arquivo,"         %5.1f",100*dispmec[k]);
                        fprintf(arquivo,"     %8d",fa_produtcam(auxcam,1.0, 0.0, dispmec[k], wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn));

                       if(auxcam <= 1.0)
                                fprintf(arquivo,"        ok!");
                       else{
                                viavel = false;
                                fprintf(arquivo,"        INV");
                        }

                        fprintf(arquivo," \n");

        }
}
fprintf(arquivo,"\t----------");
for(int i=0;i<npilhas;i++){
        if(carga[i].equip!=disponivel && carga[i].op != false)
        if(s[i] != 0){
                fprintf(arquivo,"----");
        }
}
fprintf(arquivo,"---------------------------------------------------");
fprintf(arquivo,"\n");
fprintf(arquivo,"\t Total de caminhões %8d\n",totalcam);

**********************************************************************/




liberavetor(camutil);

fclose(arquivo);
}
//----------------------------------------------------------------------
int camutilizados(int **nvisitas, struct alocarga *carga,int **compatibilidade, int npilhas, int cam)
{
for(int i=0;i<npilhas;i++){
        if(carga[i].equip != disponivel && carga[i].op == true)
                if( compatibilidade[cam][carga[i].equip]==compativel){
                if(nvisitas[i][cam] > 0){
                        return 1;
                }
        }
}
return 0;
}
//----------------------------------------------------------------------
void carregatempociclo(int npilhas, float *tciclo, float *tdv, float *tdc, float *tcarga, float *tbasc)
{
for(int i=0;i<npilhas;i++){
       tciclo[i]= tdv[i]+tdc[i]+tcarga[i]+tbasc[i];
}
}

//----------------------------------------------------------------------
void carreganummax(int npilhas, int *nummax, float *tciclo, float *tcarga)
{
for(int i=0;i<npilhas;i++)
                nummax[i]= (int)(floor(tciclo[i]/tcarga[i]));
}
//----------------------------------------------------------------------
void carreganviagem(int npilhas, int *nviagem,float tcontrole, float *tciclo)
{
for(int i=0;i<npilhas;i++)
               nviagem[i]= (int)(floor(tcontrole/tciclo[i]));
}
//----------------------------------------------------------------------


