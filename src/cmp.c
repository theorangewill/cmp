/** @file cmp.c
 *  @brief Algoritmo CMP
 *
 *  @author William F. C. Tavares (theorangewill).
 */

#ifndef SEISMICUNIX_H
#include "seismicunix.h"
#define SEISMICUNIX_H
#endif

#ifndef SEMBLANCE_H
#include "semblance.h"
#define SEMBLANCE_H
#endif

#ifndef MATH_H
#include <math.h>
#define MATH_H
#endif

#ifndef STRING_H
#include <string.h>
#define STRING_H
#endif

/*
 * Algoritmo CMP.
 */
void CMP(ListaTracos *lista, float Cini, float Cfin, float incr, float wind, Traco* tracoEmpilhado, Traco* tracoSemblance, Traco* tracoC);


/*
 * Seta os campos do cabecalho para o empilhamento
 */
void SetCabecalhoCMP(Traco *traco);

/*
 * Libera memoria alocada para o programa.
 */
void LiberarMemoria(ListaTracos ***lista, int *tamanho);

int main (int argc, char **argv)
{
    ListaTracos **listaTracos = NULL;
    int tamanhoLista = 0;
    float Cini, Cfin, Cinc, wind, aph;
    int tracos;
    char saida[101], saidaEmpilhado[104], saidaSemblance[104], saidaC[104];
    FILE *arquivoEmpilhado, *arquivoSemblance, *arquivoC;
    Traco tracoSemblance, tracoC, tracoEmpilhado;

    if(argc < 7){
        printf("ERRO: ./main <dado sismico> C_INI C_FIN C_INC WIND APH\n");
        printf("\tARQUIVO: rodar susort <entrada.su >saida.su cdp offset (ordenar os traços em cdp e offset\n");
        printf("\tC_INI:  constante C inicial\n");
        printf("\tC_FIN:  constante C final\n");
        printf("\tC_INC:    quantidade de C avaliados\n");
        printf("\tWIND:    janela do semblance (em amostras)\n");
        printf("\tAPH:  aperture\n");
        exit(1);
    }


    //Leitura dos parametros
    Cini = atof(argv[2]);
    Cfin = atof(argv[3]);
    Cinc = atof(argv[4]);
    wind = atof(argv[5]);
    aph = atof(argv[6]);

    //Leitura do arquivo
    if(!LeitorArquivoSU(argv[1], &listaTracos, &tamanhoLista, aph)){
        printf("ERRO NA LEITURA\n");
        exit(1);
    }


    //Criacao dos arquivos de saida
    argv[1][strlen(argv[1])-3] = '\0';
    strcpy(saida,argv[1]);
    strcpy(saidaEmpilhado,saida);
    strcat(saidaEmpilhado,"-empilhado.out.su");
    arquivoEmpilhado = fopen(saidaEmpilhado,"w");
    strcpy(saidaSemblance,saida);
    strcat(saidaSemblance,"-semblance.out.su");
    arquivoSemblance = fopen(saidaSemblance,"w");
    strcpy(saidaC,saida);
    strcat(saidaC,"-C.out.su");
    arquivoC = fopen(saidaC,"w");

    //Rodar o CMP para cada conjunto de tracos de mesmo cdp
    for(tracos=0; tracos<tamanhoLista; tracos++){
        printf("\t%d[%d] (cdp= %d) de %d\n", tracos, listaTracos[tracos]->tamanho, listaTracos[tracos]->cdp, tamanhoLista);
        //PrintTracoSU(listaTracos[tracos]->tracos[0]);

        //Copiar cabecalho do conjunto dos tracos para os tracos de saida
        memcpy(&tracoEmpilhado,listaTracos[tracos]->tracos[0], SEISMIC_UNIX_HEADER);
        //E necessario setar os conteudos de offset e coordenadas de fonte e receptores
        SetCabecalhoCMP(&tracoEmpilhado);
        memcpy(&tracoSemblance,&tracoEmpilhado, SEISMIC_UNIX_HEADER);
        memcpy(&tracoC,&tracoEmpilhado, SEISMIC_UNIX_HEADER);

        //Execucao do CMP
        CMP(listaTracos[tracos],Cini,Cfin,Cinc,wind,&tracoEmpilhado,&tracoSemblance,&tracoC);

        //Copiar os tracos resultantes nos arquivos de saida
        fwrite(&tracoEmpilhado,SEISMIC_UNIX_HEADER,1,arquivoEmpilhado);
        fwrite(&(tracoEmpilhado.dados[0]),sizeof(float),tracoEmpilhado.ns,arquivoEmpilhado);
        fwrite(&tracoSemblance,SEISMIC_UNIX_HEADER,1,arquivoSemblance);
        fwrite(&(tracoSemblance.dados[0]),sizeof(float),tracoSemblance.ns,arquivoSemblance);
        fwrite(&tracoC,SEISMIC_UNIX_HEADER,1,arquivoC);
        fwrite(&(tracoC.dados[0]),sizeof(float),tracoC.ns,arquivoC);

        //Liberar memoria alocada nos dados do traco resultante
        free(tracoEmpilhado.dados);
        free(tracoSemblance.dados);
        free(tracoC.dados);
    }

    fclose(arquivoEmpilhado);
    fclose(arquivoSemblance);
    fclose(arquivoC);

    LiberarMemoria(&listaTracos, &tamanhoLista);

    return 1;
}



void CMP(ListaTracos *lista, float Cini, float Cfin, float Cinc, float wind, Traco* tracoEmpilhado, Traco* tracoSemblance, Traco* tracoC)
{
    int amostra, amostras;
    float seg, t0;
    float C, bestC;
    float s, bestS;
    float pilha, pilhaTemp;

    //Tempo entre amostras, convertido para segundos
    seg = ((float) lista->tracos[0]->dt)/1000000;
    //Numero de amostras
    amostras = lista->tracos[0]->ns;
    //Alocar memoria para os dados dos tracos resultantes
    tracoEmpilhado->dados = malloc(sizeof(float)*amostras);
    tracoSemblance->dados = malloc(sizeof(float)*amostras);
    tracoC->dados = malloc(sizeof(float)*amostras);

    Cinc = (Cfin-Cini)/Cinc;

    //Para cada amostra do primeiro traco
    for(amostra=0; amostra<amostras; amostra++){
        //Calcula o segundo inicial
        t0 = amostra*seg;

        //Inicializar variaveis antes da busca
        pilha = lista->tracos[0]->dados[amostra];

        bestC = Cini;
        bestS = 0;
        //Para cada constante C
        for(C=Cini; C<=Cfin; C+=Cinc){
            //Calcular semblance
            pilhaTemp = 0;
            s = Semblance(lista,0,0,C,t0,wind,seg,&pilhaTemp);
            if(s<0 && s!=-1) {printf("S NEGATIVO\n"); exit(1);}
            if(s>1) {printf("S MAIOR Q UM %.20f\n", s); exit(1);}
            else if(s > bestS){
                bestS = s;
                bestC = C;
                pilha = pilhaTemp;
            }
        }
        tracoEmpilhado->dados[amostra] = pilha;
        tracoSemblance->dados[amostra] = bestS;
        tracoC->dados[amostra] = bestC;
        //printf("\n%d S=%.10f C=%.20f Pilha=%.10f\n", amostra, bestS, bestC, pilha);
    }
}

void SetCabecalhoCMP(Traco *traco)
{
    int mx, my;
    mx = (traco->sx + traco->gx) / 2;
    my = (traco->sy + traco->gy) / 2;
    //Offset zerado pois receptores e fonte estao na mesma coordenada
    traco->offset = 0;
    //Fonte e receptores na mesma coordenada
    traco->sx = mx;
    traco->sy = my;
    traco->gx = mx;
    traco->gy = my;
}

void LiberarMemoria(ListaTracos ***lista, int *tamanho)
{
    LiberarMemoriaSU(lista,tamanho);
}
