#include "Hvnd.h"
#include "Hmovimento.h"
#include "Hfavaliacao.h"
#include "Hutil.h"
#include <stdlib.h>
#include "Hdefine.h"
#include "hstruct.h"
#include "Hutilcarga.h"

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
    // Percorre cada frente de lavra
    for (int f = 0; f < npilhas; f++) {
        // Percorre cada caminhão da frota
        for (int c = 0; c < ncam; c++) {
            
            // --- TENTATIVA 1: Adicionar uma viagem (+1) ---
            if (nvisitas[f][c] < nviagem[f][c]) {
                bool estava_desligada = (carga[f].op == false);
                // Aplica a mudança
                nvisitas[f][c]++;
                s[f] += capcam[c];
                if (estava_desligada) carga[f].op = true; // Regra de ativação automática

                long long nova_fo = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

                if (nova_fo < *fo_atual) {
                    *fo_atual = nova_fo;
                    return true; // Encontrou melhoria, encerra a busca nesta vizinhança
                }

                // DESFAZ (Rollback) caso não melhore
                nvisitas[f][c]--;
                s[f] -= capcam[c];
                if (estava_desligada) carga[f].op = false;
            }

            // --- TENTATIVA 2: Subtrair uma viagem (-1) ---
            if (nvisitas[f][c] > 0) {
                // Aplica a mudança
                nvisitas[f][c]--;
                s[f] -= capcam[c];

                long long nova_fo = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

                if (nova_fo < *fo_atual) {
                    *fo_atual = nova_fo;
                    return true;
                }

                // DESFAZ (Rollback)
                nvisitas[f][c]++;
                s[f] += capcam[c];
            }
        }
    }
    return false;
}

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
    // Percorre cada caminhão da frota
    for (int c = 0; c < ncam; c++) {
        // Percorre a frente de ORIGEM (f1)
        for (int f1 = 0; f1 < npilhas; f1++) {
            
            // Só tenta mover se o caminhão já trabalha em f1
            if (nvisitas[f1][c] > 0) {
                
                // Percorre a frente de DESTINO (f2)
                for (int f2 = 0; f2 < npilhas; f2++) {
                    if (f1 == f2) continue; // Não faz sentido mover para a mesma frente

                    // Validações de viabilidade para o destino f2
                    if (carga[f2].op == true && 
                        compatibilidade[c][carga[f2].equip] == compativel && 
                        nvisitas[f2][c] < nviagem[f2][c]) 
                    {
                        // 1. Aplica o movimento (Relocate Trip)
                        nvisitas[f1][c]--;
                        nvisitas[f2][c]++;
                        s[f1] -= capcam[c];
                        s[f2] += capcam[c];

                        // 2. Avalia a nova solução
                        long long nova_fo = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

                        if (nova_fo < *fo_atual) {
                            *fo_atual = nova_fo;
                            return true; // First Improvement: encontrou melhoria, retorna ao VND
                        }

                        // 3. Rollback caso não melhore
                        nvisitas[f1][c]++;
                        nvisitas[f2][c]--;
                        s[f1] += capcam[c];
                        s[f2] -= capcam[c];
                    }
                }
            }
        }
    }
    return false; // Nenhuma melhoria encontrada após testar todas as trocas de destino
}

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
    // Percorre cada frente de lavra (Pit)
    for (int f = 0; f < npilhas; f++) {
        
        // Só faz sentido se a frente estiver ativa
        if (carga[f].op == false) continue;

        // Percorre o caminhão doador (c1)
        for (int c1 = 0; c1 < ncam; c1++) {
            
            // Verifica se o caminhão c1 tem viagens para doar nesta frente
            if (nvisitas[f][c1] > 0) {
                
                // Percorre o caminhão receptor (c2)
                for (int c2 = 0; c2 < ncam; c2++) {
                    if (c1 == c2) continue; // Não faz sentido transferir para si mesmo

                    // Validações de viabilidade para o receptor c2 na frente f
                    if (compatibilidade[c2][carga[f].equip] == compativel && 
                        nvisitas[f][c2] < nviagem[f][c2]) 
                    {
                        // 1. Aplica o movimento (Transferência interna na frente)
                        nvisitas[f][c1]--;
                        nvisitas[f][c2]++;
                        
                        // Atualiza produção: s[f] = s[f] - massa_c1 + massa_c2
                        s[f] = s[f] - capcam[c1] + capcam[c2];

                        // 2. Avalia a nova solução
                        long long nova_fo = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

                        if (nova_fo < *fo_atual) {
                            *fo_atual = nova_fo;
                            return true; // Encontrou melhoria (First Improvement)
                        }

                        // 3. Rollback caso não melhore
                        nvisitas[f][c1]++;
                        nvisitas[f][c2]--;
                        s[f] = s[f] + capcam[c1] - capcam[c2];
                    }
                }
            }
        }
    }
    return false; // Nenhuma melhoria encontrada após testar todas as trocas internas nos pits
}

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
    // Percorre a origem: Frente f1 e Caminhão c1
    for (int f1 = 0; f1 < npilhas; f1++) {
        for (int c1 = 0; c1 < ncam; c1++) {
            
            // Verifica se há viagem para retirar de (f1, c1)
            if (nvisitas[f1][c1] > 0) {
                
                // Percorre o destino: Frente f2 e Caminhão c2
                for (int f2 = 0; f2 < npilhas; f2++) {
                    for (int c2 = 0; c2 < ncam; c2++) {
                        
                        // Evita a mesma célula (isso seria neutro)
                        if (f1 == f2 && c1 == c2) continue;

                        // Validações de viabilidade para o destino (f2, c2)
                        if (carga[f2].op == true && 
                            compatibilidade[c2][carga[f2].equip] == compativel && 
                            nvisitas[f2][c2] < nviagem[f2][c2]) 
                        {
                            // 1. Aplica a troca cruzada de uma viagem
                            nvisitas[f1][c1]--;
                            nvisitas[f2][c2]++;
                            s[f1] -= capcam[c1];
                            s[f2] += capcam[c2];

                            // 2. Avalia a nova solução
                            long long nova_fo = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

                            if (nova_fo < *fo_atual) {
                                *fo_atual = nova_fo;
                                return true; // Encontrou melhoria (First Improvement)
                            }

                            // 3. Rollback caso não melhore
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
    return false; // Nenhuma melhoria encontrada nesta vizinhança
}

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
    // Percorre cada par de frentes (f1 e f2) para testar a troca de carregadeiras
    for (int f1 = 0; f1 < npilhas; f1++) {
        for (int f2 = f1 + 1; f2 < npilhas; f2++) {
            
            // Só faz sentido trocar se os equipamentos forem diferentes
            if (carga[f1].equip == carga[f2].equip) continue;

            // --- 1. APLICA O MOVIMENTO (Swap Load) ---
            // Salva estado original para possível rollback
            int equip_f1_orig = carga[f1].equip;
            int equip_f2_orig = carga[f2].equip;
            bool op_f1_orig = carga[f1].op;
            bool op_f2_orig = carga[f2].op;
            
            int *visitas_f1_temp = new int[ncam];
            int *visitas_f2_temp = new int[ncam];
            for(int c=0; c<ncam; c++) {
                visitas_f1_temp[c] = nvisitas[f1][c];
                visitas_f2_temp[c] = nvisitas[f2][c];
            }

            // Realiza a troca (conforme Souza 2010: viagens acompanham a máquina)
            carga[f1].equip = equip_f2_orig;
            carga[f2].equip = equip_f1_orig;

            for (int c = 0; c < ncam; c++) {
                nvisitas[f1][c] = visitas_f2_temp[c];
                nvisitas[f2][c] = visitas_f1_temp[c];
                
                // Validação de limites (nviagem) na nova frente
                if (nvisitas[f1][c] > nviagem[f1][c]) nvisitas[f1][c] = nviagem[f1][c];
                if (nvisitas[f2][c] > nviagem[f2][c]) nvisitas[f2][c] = nviagem[f2][c];
            }

            // Ativação automática se houver máquina
            if (carga[f1].equip != disponivel) carga[f1].op = true;
            if (carga[f2].equip != disponivel) carga[f2].op = true;

            // Recalcula as produções locais s[f]
            s[f1] = producaofrente(nvisitas, carga, capcam, compatibilidade, ncam, f1);
            s[f2] = producaofrente(nvisitas, carga, capcam, compatibilidade, ncam, f2);

            // --- 2. AVALIAÇÃO ---
            long long nova_fo = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

            if (nova_fo < *fo_atual) {
                *fo_atual = nova_fo;
                delete[] visitas_f1_temp; delete[] visitas_f2_temp;
                return true; // Encontrou melhoria
            }

            // --- 3. ROLLBACK ---
            carga[f1].equip = equip_f1_orig;
            carga[f2].equip = equip_f2_orig;
            carga[f1].op = op_f1_orig;
            carga[f2].op = op_f2_orig;
            for(int c=0; c<ncam; c++) {
                nvisitas[f1][c] = visitas_f1_temp[c];
                nvisitas[f2][c] = visitas_f2_temp[c];
            }
            s[f1] = producaofrente(nvisitas, carga, capcam, compatibilidade, ncam, f1);
            s[f2] = producaofrente(nvisitas, carga, capcam, compatibilidade, ncam, f2);
            
            delete[] visitas_f1_temp; delete[] visitas_f2_temp;
        }
    }
    return false;
}

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
    // Percorre cada frente de lavra para testar a inversão de operação (Ligar/Desligar)
    for (int f = 0; f < npilhas; f++) {
        
        // --- 1. BACKUP DO ESTADO ATUAL ---
        bool status_original = carga[f].op;
        float s_original = s[f];
        int *visitas_backup = new int[ncam];
        for(int c = 0; c < ncam; c++) visitas_backup[c] = nvisitas[f][c];

        // --- 2. APLICA A MUDANÇA (Inversão) ---
        if (carga[f].op == true) {
            // Desativar: remove todas as viagens da frente
            carga[f].op = false;
            for (int c = 0; c < ncam; c++) nvisitas[f][c] = 0;
            s[f] = 0.0;
        } else {
            // Reativar: apenas muda o status (as viagens virão via outros movimentos)
            carga[f].op = true;
        }

        // --- 3. AVALIAÇÃO ---
        long long nova_fo = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

        if (nova_fo < *fo_atual) {
            *fo_atual = nova_fo;
            delete[] visitas_backup;
            return true; // Encontrou melhoria (First Improvement)
        }

        // --- 4. ROLLBACK ---
        carga[f].op = status_original;
        s[f] = s_original;
        for (int c = 0; c < ncam; c++) nvisitas[f][c] = visitas_backup[c];
        
        delete[] visitas_backup;
    }
    return false; // Nenhuma melhoria encontrada desativando/ativando frentes
}

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
    // Percorre cada frente de lavra
    for (int f = 0; f < npilhas; f++) {
        
        // Só faz sentido processar frentes que estão operando
        if (carga[f].op == false) continue;

        // Percorre cada caminhão da frota
        for (int c = 0; c < ncam; c++) {
            
            // Verifica se o caminhão possui viagens nesta frente específica
            if (nvisitas[f][c] > 0) {
                
                // --- 1. BACKUP DO ESTADO ---
                int visitas_originais = nvisitas[f][c];
                float s_original = s[f];

                // --- 2. APLICA O MOVIMENTO (Zerar viagens do caminhão na frente f) ---
                float massa_removida = visitas_originais * capcam[c];
                nvisitas[f][c] = 0;
                s[f] -= massa_removida;

                // --- 3. AVALIAÇÃO ---
                long long nova_fo = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

                if (nova_fo < *fo_atual) {
                    *fo_atual = nova_fo;
                    return true; // Encontrou melhoria (First Improvement)
                }

                // --- 4. ROLLBACK ---
                nvisitas[f][c] = visitas_originais;
                s[f] = s_original;
            }
        }
    }
    return false; // Nenhuma melhoria encontrada reduzindo a frota por frente
}

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
    // Percorre cada par de frentes para testar a troca de carregadeiras
    for (int f1 = 0; f1 < npilhas; f1++) {
        for (int f2 = f1 + 1; f2 < npilhas; f2++) {
            
            // Verificação simples: ignora se ambas as frentes não tiverem máquinas
            if (carga[f1].equip == disponivel && carga[f2].equip == disponivel) continue;
            // Ignora se os equipamentos forem idênticos
            if (carga[f1].equip == carga[f2].equip) continue;

            // --- 1. BACKUP DO ESTADO ---
            int equip_f1_orig = carga[f1].equip;
            int equip_f2_orig = carga[f2].equip;
            float s_f1_orig = s[f1];
            float s_f2_orig = s[f2];
            
            int *visitas_f1_backup = new int[ncam];
            int *visitas_f2_backup = new int[ncam];
            for(int c = 0; c < ncam; c++) {
                visitas_f1_backup[c] = nvisitas[f1][c];
                visitas_f2_backup[c] = nvisitas[f2][c];
            }

            // --- 2. APLICA O MOVIMENTO (Swap Shovels) ---
            carga[f1].equip = equip_f2_orig;
            carga[f2].equip = equip_f1_orig;

            // Limpa viagens incompatíveis em f1
            for (int c = 0; c < ncam; c++) {
                if (nvisitas[f1][c] > 0 && compatibilidade[c][carga[f1].equip] != compativel) {
                    nvisitas[f1][c] = 0;
                }
            }
            // Limpa viagens incompatíveis em f2
            for (int c = 0; c < ncam; c++) {
                if (nvisitas[f2][c] > 0 && compatibilidade[c][carga[f2].equip] != compativel) {
                    nvisitas[f2][c] = 0;
                }
            }

            // Atualiza produções locais s[f]
            s[f1] = producaofrente(nvisitas, carga, capcam, compatibilidade, ncam, f1);
            s[f2] = producaofrente(nvisitas, carga, capcam, compatibilidade, ncam, f2);

            // --- 3. AVALIAÇÃO ---
            long long nova_fo = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

            if (nova_fo < *fo_atual) {
                *fo_atual = nova_fo;
                delete[] visitas_f1_backup; delete[] visitas_f2_backup;
                return true; // Encontrou melhoria
            }

            // --- 4. ROLLBACK ---
            carga[f1].equip = equip_f1_orig;
            carga[f2].equip = equip_f2_orig;
            for (int c = 0; c < ncam; c++) {
                nvisitas[f1][c] = visitas_f1_backup[c];
                nvisitas[f2][c] = visitas_f2_backup[c];
            }
            s[f1] = s_f1_orig;
            s[f2] = s_f2_orig;
            
            delete[] visitas_f1_backup; delete[] visitas_f2_backup;
        }
    }
    return false;
}


int VND(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
        int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
        float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
        int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
        int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
        int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
        int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
        int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem) 
{
    int k = 1;
    long long fo_atual;

    // Avaliação inicial da solução vinda do Shaking
    fo_atual = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, 
        prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole,
        ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn,
        wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga,
        wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);

    while (k <= 8) {
        bool encontrou_melhoria = false;

        // Tenta encontrar a melhor melhora local na vizinhança k
        switch(k) {
            case 1: 
                encontrou_melhoria = melhorVizinhoNT(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem, &fo_atual); 
                break;
            case 2: 
                encontrou_melhoria = melhorVizinhoTT(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem, &fo_atual); 
                break;
            case 3: 
                encontrou_melhoria = melhorVizinhoTP(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem, &fo_atual); 
                break;
            case 4: 
                encontrou_melhoria = melhorVizinhoST(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem, &fo_atual); 
                break;
            case 5: 
                encontrou_melhoria = melhorVizinhoL(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem, &fo_atual); 
                break;
            case 6: 
                encontrou_melhoria = melhorVizinhoPO(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem, &fo_atual); 
                break;
            case 7: 
                encontrou_melhoria = melhorVizinhoTO(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem, &fo_atual); 
                break;
            case 8: 
                encontrou_melhoria = melhorVizinhoSS(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole, ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn, wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga, wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem, &fo_atual); 
                break;
        }

        if (encontrou_melhoria) {
            k = 1; // Melhorou, volta para a vizinhança mais simples para refinar
        } else {
            k++; // Não melhorou, tenta a próxima vizinhança mais complexa
        }
    }
    return 0;
}

bool VND_Aleatorio(float *s, float *li, float *ls, float *rec, int *penalidadesup, int *penalidadeinf, int *penalidadeteorinv, 
                    int npilhas, int nvar, float **teores, float prodmax, float prodmin, float prodrec, int npminerio, 
                    float remrec, float remmax, float remmin, int **nvisitas, struct alocarga *carga, float *capcam, 
                    int **compatibilidade, float **tciclo, float tcontrole, int ncam, float *cargamax, float *cargamin, 
                    int *nummax, int wfproducaoinv, int wfproducaoqualp, int wfproducaoqualn, int wfteor, int wfrem, 
                    int wfremqualp, int wfremqualn, int wfprodutcaminv, int wfprodutcamqualp, int wfprodutcamqualn, 
                    int wfprodcargainv, int wfprodcargaqual, int wfproducaocamtotal, int wfproducaocarga, 
                    int wfnumcamutilizados, int wfestatico, int wfnumcam, int wfdist, float *dispmec, int **nviagem) 
{
    long long fo_atual = favaliacao(s, li, ls, rec, penalidadesup, penalidadeinf, penalidadeteorinv, npilhas, nvar, teores, prodmax, 
        prodmin, prodrec, npminerio, remrec, remmax, remmin, nvisitas, carga, capcam, compatibilidade, tciclo, tcontrole,
        ncam, cargamax, cargamin, nummax, wfproducaoinv, wfproducaoqualp, wfproducaoqualn, wfteor, wfrem, wfremqualp, wfremqualn,
        wfprodutcaminv, wfprodutcamqualp, wfprodutcamqualn, wfprodcargainv, wfprodcargaqual, wfproducaocamtotal, wfproducaocarga,
        wfnumcamutilizados, wfestatico, wfnumcam, wfdist, dispmec, nviagem);
    bool melhoria_global = false;
    
    // Vetor com os IDs das vizinhanças (1 a 8 conforme seu switch original)
    int vizinhancas[] = {1, 2, 3, 4, 5, 6, 7, 8};
    float pesos[] = {10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0};
    int falhas[] = {0, 0, 0, 0, 0, 0, 0, 0};

    bool encontrou_no_ciclo = true;

    while (encontrou_no_ciclo) {
        encontrou_no_ciclo = false;

        // --- EMBARALHAMENTO BASEADO EM PESOS ---
        // Aqui você sorteia a ordem das vizinhancas baseado nos pesos
        for (int i = 0; i < 8; i++) {
            int r = i + rand() % (8 - i);
            int temp = vizinhancas[i];
            vizinhancas[i] = vizinhancas[r];
            vizinhancas[r] = temp;
        }

        for (int i = 0; i < 8; i++) {
            int v = vizinhancas[i];
            bool melhorou = false;

            // Executa a vizinhança v (chamando suas funções melhorVizinhoNT, melhorVizinhoTO, etc)
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

            if (melhorou) {
                pesos[v-1] += 2.0;    // Peso positivo por melhorar
                falhas[v-1] = 0;      // Reseta contador de falhas
                encontrou_no_ciclo = true;
                melhoria_global = true;
                break; // Volta para a primeira vizinhança da lista embaralhada
            } else {
                falhas[v-1]++;
                if (falhas[v-1] > 3) { // Se falhar X vezes
                    pesos[v-1] -= 0.5; // Penalidade no peso
                    if (pesos[v-1] < 1.0) pesos[v-1] = 1.0; // Peso mínimo
                }
            }
        }
    }

    return melhoria_global;
}