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

#ifndef OMP_H
#include "omp.h"
#define OMP_H
#endif

#define NUM_THREADS 8

/*
 * Algoritmo CMP.
 */
void CMP(ListaTracos *lista, float *Vvector, float *Cvector, float Vint, 
            float wind, float azimuth, Traco* tracoEmpilhado, 
            Traco* tracoSemblance, Traco* tracoV);

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
    float wind, aph, azimuth;
    float Vini, Vfin, Vinc;
    int Vint;
    float *Vvector, *Cvector;
    int tracos;
    int i;
    char saida[101], saidaEmpilhado[104], saidaSemblance[104], saidaV[104];
    FILE *arquivoEmpilhado, *arquivoSemblance, *arquivoV;
    Traco tracoSemblance, tracoEmpilhado, tracoV;

    if(argc < 8){
        printf("ERRO: ./main <dado sismico> V_INI V_FIN V_INT WIND APH AZIMUTH\n");
        printf("\tARQUIVO: arquivo dos tracos sismicos\n");
        printf("\tV_INI:  velocidade inicial\n");
        printf("\tV_FIN:  velocidade final\n");
        printf("\tV_INT:    quantidade de velocidades avaliadas\n");
        printf("\tWIND:    janela do semblance\n");
        printf("\tAPH:  aperture\n");
        printf("\tAZIMUTH:    azimuth\n");
        exit(1);
    }

    //Leitura dos parametros
    Vini = atof(argv[2]);
    Vfin = atof(argv[3]);
    Vint = atoi(argv[4]);
    wind = atof(argv[5]);
    aph = atof(argv[6]);
    azimuth = atof(argv[7]);

    //Leitura do arquivo
    if(!LeitorArquivoSU(argv[1], &listaTracos, &tamanhoLista, aph, azimuth)){
        printf("ERRO NA LEITURA\n");
        exit(1);
    }

    //Criacao dos arquivos de saida
    argv[1][strlen(argv[1])-3] = '\0';
    strcpy(saida,"results");
    for(i=strlen(argv[1])-1; argv[1][i] != '/' && i > 0; i--);
    strcat(saida,&(argv[1][i]));
    
    strcpy(saidaEmpilhado,saida);
    strcat(saidaEmpilhado,"-empilhado.out.su");
    arquivoEmpilhado = fopen(saidaEmpilhado,"w");
	if(arquivoEmpilhado == NULL){
        printf("ERRO NA ABERTURA DO ARQUIVO EMPILHADO\n");
		return 0;
	}
    strcpy(saidaSemblance,saida);
    strcat(saidaSemblance,"-semblance.out.su");
    arquivoSemblance = fopen(saidaSemblance,"w");
	if(arquivoSemblance == NULL){
        printf("ERRO NA ABERTURA DO ARQUIVO SEMBLANCE\n");
		return 0;
	}
    strcpy(saidaV,saida);
    strcat(saidaV,"-V.out.su");
    arquivoV = fopen(saidaV,"w");
	if(arquivoV == NULL){
        printf("ERRO NA ABERTURA DO ARQUIVO V\n");
		return 0;
	}

    //Calcular os valores de busca para V e C
    Vinc = (Vfin-Vini)/(Vint);
    Vvector = malloc(sizeof(float)*(Vint));
    Cvector = malloc(sizeof(float)*(Vint));
#ifdef OMP_H
#pragma omp parallel for num_threads(2) firstprivate(Vinc,Vini,Vint) shared(Cvector,Vvector)
#endif
    for(i=0; i<Vint; i++){
      Vvector[i] = Vinc*i+Vini;
      Cvector[i] = 4/Vvector[i]*1/Vvector[i];
    }

#ifdef OMP_H
omp_set_num_threads(NUM_THREADS);
#endif

    //Rodar o CMP para cada conjunto de tracos de mesmo cdp
    for(tracos=0; tracos<tamanhoLista; tracos++){
        printf("\t%d[%d] (cdp= %d) de %d\n", tracos, listaTracos[tracos]->tamanho, listaTracos[tracos]->cdp, tamanhoLista);

        //Copiar cabecalho do conjunto dos tracos para os tracos de saida
        memcpy(&tracoEmpilhado,listaTracos[tracos]->tracos[0], SEISMIC_UNIX_HEADER);
        
        //E necessario setar os conteudos de offset e coordenadas de fonte e receptores
        SetCabecalhoCMP(&tracoEmpilhado);
        memcpy(&tracoSemblance,&tracoEmpilhado, SEISMIC_UNIX_HEADER);
        memcpy(&tracoV,&tracoEmpilhado, SEISMIC_UNIX_HEADER);
  
        //Execucao do CMP
        CMP(listaTracos[tracos],Vvector,Cvector,Vint,wind,azimuth,&tracoEmpilhado,&tracoSemblance,&tracoV);
        
        //Copiar os tracos resultantes nos arquivos de saida
        fwrite(&tracoEmpilhado,SEISMIC_UNIX_HEADER,1,arquivoEmpilhado);
        fwrite(&(tracoEmpilhado.dados[0]),sizeof(float),tracoEmpilhado.ns,arquivoEmpilhado);
        fwrite(&tracoSemblance,SEISMIC_UNIX_HEADER,1,arquivoSemblance);
        fwrite(&(tracoSemblance.dados[0]),sizeof(float),tracoSemblance.ns,arquivoSemblance);
        fwrite(&tracoV,SEISMIC_UNIX_HEADER,1,arquivoV);
        fwrite(&(tracoV.dados[0]),sizeof(float),tracoV.ns,arquivoV);

        //Liberar memoria alocada nos dados do traco resultante
        free(tracoEmpilhado.dados);
        free(tracoSemblance.dados);
        free(tracoV.dados);
    }

    fclose(arquivoEmpilhado);
    fclose(arquivoSemblance);
    fclose(arquivoV);

    LiberarMemoria(&listaTracos, &tamanhoLista);
    printf("SALVO NOS ARQUIVOS:\n\t%s\n\t%s\n\t%s\n",saidaEmpilhado,saidaSemblance,saidaV);
    return 1;
}

void CMP(ListaTracos *lista, float *Vvector, float *Cvector, float Vint, float wind, float azimuth, Traco* tracoEmpilhado, Traco* tracoSemblance, Traco* tracoV)
{
    int amostra, amostras;
    float seg, t0;
    float bestV;
    float s, bestS;
    float pilha, pilhaTemp;
    int i;
    int w;
    int janela;

    //Tempo entre amostras, convertido para segundos
    seg = ((float) lista->tracos[0]->dt)/1000000;
    w = (int) (wind/seg);
    janela = 2*w+1;
    //Numero de amostras
    amostras = lista->tracos[0]->ns;
    //Alocar memoria para os dados dos tracos resultantes
    tracoEmpilhado->dados = malloc(sizeof(float)*amostras);
    tracoSemblance->dados = malloc(sizeof(float)*amostras);
    tracoV->dados = malloc(sizeof(float)*amostras);
    //Para cada amostra do primeiro traco
#ifdef OMP_H
#pragma omp parallel for schedule(static) firstprivate(lista,amostras,Vint,seg,wind,azimuth,Cvector,Vvector) private(bestV,bestS,i,pilha,pilhaTemp,t0,amostra,s)  shared(tracoEmpilhado,tracoSemblance,tracoV)
#endif
    for(amostra=0; amostra<amostras; amostra++){
        //Calcula o segundo inicial
        t0 = amostra*seg;

        //Inicializar variaveis antes da busca
        pilha = lista->tracos[0]->dados[amostra];
        bestS = 0;
        bestV = 0.0;
        //Para cada velocidade
        for(i=0; i<Vint; i++){
            //Calcular semblance
            pilhaTemp = 0;
            s = Semblance(lista,0.0,0.0,Cvector[i],t0,w,janela,seg,&pilhaTemp,azimuth);
            if(s<0 && s!=-1) {printf("S NEGATIVO\n"); exit(1);}
            if(s>1) {printf("S MAIOR Q UM %.20f\n", s); exit(1);}
            else if(s > bestS){
                bestS = s;
                bestV = Cvector[i];
                pilha = pilhaTemp;
            }
        }
        tracoEmpilhado->dados[amostra] = pilha;
        tracoSemblance->dados[amostra] = bestS;
        tracoV->dados[amostra] = bestV;
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
