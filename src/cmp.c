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
 * Algoritmo CRS.
 */
void CRS(ListaTracos *lista, float Aini, float Afin, float Ainc,
          float Bini, float Bfin, float Binc, float Cini,
          float Cfin, float Cinc, float wind, Traco* tracoEmpilhado,
          Traco* tracoSemblance, Traco* tracoA, Traco* tracoB, Traco* tracoC);

/*
 * Seta os campos do cabecalho para o empilhamento
 */
void SetCabecalhoCRS(Traco *traco);


/*
 * Algoritmo CMP.
 *
void CMP(ListaTracos *lista, float Cini, float Cfin, float incr, float wind, Traco* tracoEmpilhado, Traco* tracoSemblance, Traco* tracoC);


/*
 * Seta os campos do cabecalho para o empilhamento
 *
void SetCabecalhoCMP(Traco *traco);

/*
 * Libera memoria alocada para o programa.
 */
void LiberarMemoria(ListaTracos ***lista, int *tamanho);
/*
int main (int argc, char **argv)
{
    ListaTracos **listaTracos = NULL;
    int tamanhoLista = 0;
    float Cini, Cfin, Cinc, wind;
    int tracos;
    char saida[101], saidaEmpilhado[104], saidaSemblance[104], saidaC[104];
    FILE *arquivoEmpilhado, *arquivoSemblance, *arquivoC;
    Traco tracoSemblance, tracoC, tracoEmpilhado;

    if(argc < 6){
        printf("ERRO: ./main <dado sismico> C_INI C_FIN C_INC WIND\n");
        printf("\tARQUIVO: rodar susort <entrada.su >saida.su cdp offset (ordenar os traços em cdp e offset\n");
        printf("\tC_INI:  constante C inicial\n");
        printf("\tC_FIN:  constante C final\n");
        printf("\tC_INC:    quantidade de C avaliados\n");
        printf("\tWIND:    janela do semblance (em amostras)\n");
        exit(1);
    }

    //Leitura do arquivo
    if(!LeitorArquivoSU(argv[1], &listaTracos, &tamanhoLista)){
        printf("ERRO NA LEITURA\n");
        exit(1);
    }

    //Leitura dos parametros
    Cini = atof(argv[2]);
    Cfin = atof(argv[3]);
    Cinc = atof(argv[4]);
    wind = atof(argv[5]);

    //Criacao dos arquivos de saida
    argv[1][strlen(argv[1])-3] = '\0';
    strcpy(saida,argv[1]);
    strcpy(saidaEmpilhado,saida);
    strcat(saidaEmpilhado,"-empilhado.su");
    arquivoEmpilhado = fopen(saidaEmpilhado,"w");
    strcpy(saidaSemblance,saida);
    strcat(saidaSemblance,"-semblance.su");
    arquivoSemblance = fopen(saidaSemblance,"w");
    strcpy(saidaC,saida);
    strcat(saidaC,"-C.su");
    arquivoC = fopen(saidaC,"w");

    //Rodar o CMP para cada conjunto de tracos de mesmo cdp
    for(tracos=0; tracos<tamanhoLista; tracos++){
      //if(listaTracos[tracos]->cdp != 93) continue;
        //if(listaTracos[tracos]->tamanho <= 4) continue;
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
        //printf("\t%d[%d] (cdp= %d) de %d\n", tracos, listaTracos[tracos]->tamanho, listaTracos[tracos]->cdp, tamanhoLista);
        //break;
    }

    fclose(arquivoEmpilhado);
    fclose(arquivoSemblance);
    fclose(arquivoC);

    LiberarMemoria(&listaTracos, &tamanhoLista);

    return 1;
}
*/

int main (int argc, char **argv)
{
    ListaTracos **listaTracos = NULL;
    int tamanhoLista = 0;
    float Aini, Afin, Ainc;
    float Bini, Bfin, Binc;
    float Cini, Cfin, Cinc;
    float wind, md;
    int tracos;
    char saida[101], saidaEmpilhado[104], saidaSemblance[104], saidaA[104], saidaB[104], saidaC[104];
    FILE *arquivoEmpilhado, *arquivoSemblance, *arquivoA, *arquivoB, *arquivoC;
    Traco tracoSemblance, tracoA, tracoB, tracoC, tracoEmpilhado;

    if(argc < 13){
        printf("ERRO: ./main <dado sismico> A_INI A_FIN A_INC B_INI B_FIN B_INC C_INI C_FIN C_INC MD WIND\n");
        printf("\tARQUIVO: rodar susort <entrada.su >saida.su cdp offset (ordenar os traços em cdp e offset\n");
        printf("\tA_INI:  constante A inicial\n");
        printf("\tA_FIN:  constante A final\n");
        printf("\tA_INC:    quantidade de A avaliados\n");
        printf("\tB_INI:  constante B inicial\n");
        printf("\tB_FIN:  constante B final\n");
        printf("\tB_INC:    quantidade de B avaliados\n");
        printf("\tC_INI:  constante C inicial\n");
        printf("\tC_FIN:  constante C final\n");
        printf("\tC_INC:    quantidade de C avaliados\n");
        printf("\tMD:    distancia dos vizinhos\n");
        printf("\tWIND:    janela do semblance (em amostras)\n");
        exit(1);
    }

    //Leitura do arquivo
    if(!LeitorArquivoSU(argv[1], &listaTracos, &tamanhoLista)){
        printf("ERRO NA LEITURA\n");
        exit(1);
    }

    //Leitura dos parametros
    Aini = atof(argv[2]);
    Afin = atof(argv[3]);
    Ainc = atof(argv[4]);
    Bini = atof(argv[5]);
    Bfin = atof(argv[6]);
    Binc = atof(argv[7]);
    Cini = atof(argv[8]);
    Cfin = atof(argv[9]);
    Cinc = atof(argv[10]);
    md = atof(argv[11]);
    wind = atof(argv[12]);

    //Criacao dos arquivos de saida
    argv[1][strlen(argv[1])-3] = '\0';
    strcpy(saida,argv[1]);
    strcpy(saidaEmpilhado,saida);
    strcat(saidaEmpilhado,"-empilhado.out.su");
    arquivoEmpilhado = fopen(saidaEmpilhado,"w");
    strcpy(saidaSemblance,saida);
    strcat(saidaSemblance,"-semblance.out.su");
    arquivoSemblance = fopen(saidaSemblance,"w");
    strcpy(saidaA,saida);
    strcat(saidaA,"-A.out.su");
    arquivoA = fopen(saidaA,"w");
    strcpy(saidaB,saida);
    strcat(saidaB,"-B.out.su");
    arquivoB = fopen(saidaB,"w");
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
        SetCabecalhoCRS(&tracoEmpilhado);
        memcpy(&tracoSemblance,&tracoEmpilhado, SEISMIC_UNIX_HEADER);
        memcpy(&tracoA,&tracoEmpilhado, SEISMIC_UNIX_HEADER);
        memcpy(&tracoB,&tracoEmpilhado, SEISMIC_UNIX_HEADER);
        memcpy(&tracoC,&tracoEmpilhado, SEISMIC_UNIX_HEADER);

        //Computar os vizinhos do cdp
        ComputarVizinhos(listaTracos,tamanhoLista,tracos,md);
        //Execucao do CRS
        CRS(listaTracos[tracos],Aini,Afin,Ainc,Bini,Bfin,Binc,Cini,Cfin,Cinc,wind,&tracoEmpilhado,&tracoSemblance,&tracoA,&tracoB,&tracoC);

        //Copiar os tracos resultantes nos arquivos de saida
        fwrite(&tracoEmpilhado,SEISMIC_UNIX_HEADER,1,arquivoEmpilhado);
        fwrite(&(tracoEmpilhado.dados[0]),sizeof(float),tracoEmpilhado.ns,arquivoEmpilhado);
        fwrite(&tracoSemblance,SEISMIC_UNIX_HEADER,1,arquivoSemblance);
        fwrite(&(tracoSemblance.dados[0]),sizeof(float),tracoSemblance.ns,arquivoSemblance);
        fwrite(&tracoA,SEISMIC_UNIX_HEADER,1,arquivoA);
        fwrite(&(tracoA.dados[0]),sizeof(float),tracoA.ns,arquivoA);
        fwrite(&tracoB,SEISMIC_UNIX_HEADER,1,arquivoB);
        fwrite(&(tracoB.dados[0]),sizeof(float),tracoB.ns,arquivoB);
        fwrite(&tracoC,SEISMIC_UNIX_HEADER,1,arquivoC);
        fwrite(&(tracoC.dados[0]),sizeof(float),tracoC.ns,arquivoC);

        //Liberar memoria alocada nos dados do traco resultante
        free(tracoEmpilhado.dados);
        free(tracoSemblance.dados);
        free(tracoA.dados);
        free(tracoB.dados);
        free(tracoC.dados);
        //printf("\t%d[%d] (cdp= %d) de %d\n", tracos, listaTracos[tracos]->tamanho, listaTracos[tracos]->cdp, tamanhoLista);
        //break;
    }

    fclose(arquivoEmpilhado);
    fclose(arquivoSemblance);
    fclose(arquivoA);
    fclose(arquivoB);
    fclose(arquivoC);

    LiberarMemoria(&listaTracos, &tamanhoLista);

    return 1;
}

/*
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
            s = Semblance(lista,C,t0,wind,seg,&pilhaTemp);
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

*/
void CRS(ListaTracos *lista, float Aini, float Afin, float Ainc,
            float Bini, float Bfin, float Binc, float Cini,
             float Cfin, float Cinc, float wind, Traco* tracoEmpilhado,
             Traco* tracoSemblance, Traco* tracoA, Traco* tracoB, Traco* tracoC)
{
    int amostra, amostras;
    float seg, t0;
    float A, bestA;
    float B, bestB;
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
    tracoA->dados = malloc(sizeof(float)*amostras);
    tracoB->dados = malloc(sizeof(float)*amostras);
    tracoC->dados = malloc(sizeof(float)*amostras);

    Ainc = (Afin-Aini)/Ainc;
    Binc = (Bfin-Bini)/Binc;
    Cinc = (Cfin-Cini)/Cinc;

    //Para cada amostra do primeiro traco
    for(amostra=0; amostra<amostras; amostra++){
        printf("%d\n", amostra);
        //Calcula o segundo inicial
        t0 = amostra*seg;

        //Inicializar variaveis antes da busca
        pilha = lista->tracos[0]->dados[amostra];

        bestA = Aini;
        bestB = Bini;
        bestC = Cini;
        bestS = 0;
        //Para cada constante A, B e C
        for(A=Aini; A<=Afin; A+=Ainc){
        for(B=Bini; B<=Bfin; B+=Binc)
        for(C=Cini; C<=Cfin; C+=Cinc){
            //Calcular semblance
            pilhaTemp = 0;
            s = Semblance(lista,A,B,C,t0,wind,seg,&pilhaTemp);
            if(s<0 && s!=-1) {printf("S NEGATIVO\n"); exit(1);}
            if(s>1) {printf("S MAIOR Q UM %.20f\n", s); exit(1);}
            else if(s > bestS){
                bestS = s;
                bestA = A;
                bestB = B;
                bestC = C;
                pilha = pilhaTemp;
            }
        }
      }
        tracoEmpilhado->dados[amostra] = pilha;
        tracoSemblance->dados[amostra] = bestS;
        tracoA->dados[amostra] = bestA;
        tracoB->dados[amostra] = bestB;
        tracoC->dados[amostra] = bestC;
        //printf("\n%d S=%.10f C=%.20f Pilha=%.10f\n", amostra, bestS, bestC, pilha);
    }
    printf("\n");
}

void SetCabecalhoCRS(Traco *traco)
{
    float mx, my;
    MidpointSU(traco,&mx,&my);
    //Offset zerado pois receptores e fonte estao na mesma coordenada
    traco->offset = 0;
    //Fonte e receptores na mesma coordenada
    traco->sx = (int) mx;
    traco->sy = (int) my;
    traco->gx = (int) mx;
    traco->gy = (int) my;
}
/*
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
*/
void LiberarMemoria(ListaTracos ***lista, int *tamanho)
{
    LiberarMemoriaSU(lista,tamanho);
}
