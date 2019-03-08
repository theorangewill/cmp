/** @file main.c
 *  @brief Algoritmo CMP
 *
 *  @author William F. C. Tavares (theorangewill).
 */

#include "seismicunix.h"
#include <math.h>
#include <string.h>

/*
 * Libera memoria alocada para o programa.
 */
void LiberarMemoria(ListaTracos ***lista, int *tamanho);

/*
 * Algoritmo CMP.
 */
void CMP(ListaTracos *lista, float velini, float velfin, float incr, float wind, Traco* tracoEmpilhado, Traco* tracoSemblance, Traco* tracoC);


void SetCabecalhoCMP(Traco *traco);

/*
 * Calcula a metade do offset.
 */
float HalfOffset(Traco *traco);

int main (int argc, char **argv)
{
    ListaTracos **listaTracos = NULL;
    int tamanhoLista = 0;
    float velini, velfin, incr, wind;
    int tracos;
    char saida[101], saidaEmpilhado[104], saidaSemblance[104], saidaC[104];
    FILE *arquivoEmpilhado, *arquivoSemblance, *arquivoC;
    Traco tracoSemblance, tracoC, tracoEmpilhado;

    if(argc < 6){
        printf("ERRO: ./main <dado sismico> VELINI VELFIN INCR WIND\n");
        printf("\tARQUIVO: rodar susort <entrada.su >saida.su cdp offset (ordenar os traços em cdp e offset\n");
        printf("\tVELINI:  velocidade inicial\n");
        printf("\tVELFIN:  velocidade final\n");
        printf("\tINCR:    increasement na velocidade\n");
        printf("\tWIND:    janela do semblance (em amostras)\n");
        exit(1);
    }

    //Leitura do arquivo
    if(!LeitorArquivoSU(argv[1], &listaTracos, &tamanhoLista)){
        printf("ERRO NA LEITURA\n");
        exit(1);
    }

    //Leitura dos parametros
    velini = atof(argv[2]);
    velfin = atof(argv[3]);
    incr = atof(argv[4]);
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
        //Copiar cabecalho do conjunto dos tracos para os tracos de saida
        memcpy(&tracoEmpilhado,listaTracos[tracos]->tracos[0], SEISMIC_UNIX_HEADER);
        //E necessario setar os conteudos de offset e coordenadas de fonte e receptores
        SetCabecalhoCMP(&tracoEmpilhado);
        memcpy(&tracoSemblance,&tracoEmpilhado, SEISMIC_UNIX_HEADER);
        memcpy(&tracoC,&tracoEmpilhado, SEISMIC_UNIX_HEADER);

        //Execucao do CMP
        CMP(listaTracos[tracos],velini,velfin,incr,wind,&tracoEmpilhado,&tracoSemblance,&tracoC);

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

void SetCabecalhoCMP(Traco *traco)
{
    int mx, my;
    mx = (traco->sx + traco->gx) / 2;
    my = (traco->sy + traco->gy) / 2;
    traco->offset = 0;
    traco->sx = mx;
    traco->sy = my;
    traco->gx = mx;
    traco->gy = my;
}

void LiberarMemoria(ListaTracos ***lista, int *tamanho)
{
    LiberarMemoriaSU(lista,tamanho);
}

float HalfOffset(Traco *traco)
{
    float hx, hy;
    //Calcula os eixos x e y
    OffsetSU(traco,&hx,&hy);
    //Metade
    hx/=2;
    hy/=2;
    //Retorna a raiz quadrada do quadrado dos eixos
    return sqrt(hx*hx + hy*hy);
}

void InterpolacaoLinear(float *x, float x0, float x1, float y, float y0, float y1)
{
    *x = x0 + (x1- x0) * (y - y0) / (y1 - y0);
}

float Semblance(ListaTracos *lista, float C, float t0, float wind, float seg, float *pilha)
{
    int traco;
    float t, h;
    int amostra, k;
    //int w = (int) ceil(wind/seg);
    int w = (int) wind;
    int janela = 2*w+1;
    int N;
    float numerador[janela], denominador[janela];
    float num, dem;
    float valor; 
    int j;
    int erro;

    //Numerador e denominador da funcao semblance zerados
    memset(&numerador,0,sizeof(numerador));
    memset(&denominador,0,sizeof(denominador));

    //Para cada traco do conjunto
    N = 0;
    erro = 0;
    for(traco=0; traco<lista->tamanho; traco++){
        //Calcular metade do offset do traco
        h = HalfOffset(lista->tracos[traco]);
        //Calcular o tempo de acordo com a funcao da hiperbole
        t = sqrt(t0*t0 + C*h*h);
        //Calcular a amostra equivalente ao tempo calculado
        amostra = (int) t/seg;
        //Se a janela da amostra cobre os dados sismicos
        if(amostra - w >= 0 && amostra + w < lista->tracos[traco]->ns){
            //Para cada amostra dentro da janela
            for(j=0; j<janela; j++){
                k = amostra - w + j;
                //Interpolacao linear entre as duas amostras
                InterpolacaoLinear(&valor,lista->tracos[traco]->dados[k],lista->tracos[traco]->dados[k+1], t/seg-w+j, k, k+1);
                numerador[j] += valor;
                denominador[j] += valor*valor;
                *pilha += valor;
            }
            N++;
        }
        else{
            erro++;
        }
        if(erro == 2) return -1;
    }

    num = 0; dem = 0;
    for(j=0; j<janela; j++){
        num += numerador[j]*numerador[j];
        dem += denominador[j];
    }

    *pilha = (*pilha)/N/janela;

    return num / N / dem;

}

void CMP(ListaTracos *lista, float velini, float velfin, float incr, float wind, Traco* tracoEmpilhado, Traco* tracoSemblance, Traco* tracoC)
{
    int amostra, amostras;
    float vel, bestVel;
    float seg, t0; 
    float h;
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

    //Para cada amostra do primeiro traco
    for(amostra=0; amostra<amostras; amostra++){
        //Calcula o segundo inicial
        t0 = amostra*seg;

        //Inicializar variaveis antes da busca
        pilha = lista->tracos[0]->dados[amostra];
        bestVel = velini;
        bestC = 4/(velini);
        bestS = 0;
        //Para cada velocidade
        for(vel=velini; vel<=velfin; vel+=incr){
            //Calcular o termo do calculo da hiperbole
            C = 4/(vel*vel);
            //Calcular semblance
            pilhaTemp = 0;
            s = Semblance(lista,C,t0,wind,seg,&pilhaTemp);
            if(s<0 && s!=-1) printf("S NEGATIVO\n");
            if(s>1) printf("S MAIOR Q UM\n");
            if(s == -1){
                break;
            }
            else if(s > bestS){  
                bestS = s;
                bestC = C;
                bestVel = vel;
                pilha = pilhaTemp;
            }
        }
        tracoEmpilhado->dados[amostra] = pilha;
        tracoSemblance->dados[amostra] = bestS;
        tracoC->dados[amostra] = bestC;
        //printf("%d S=%.20f C=%.20f Vel=%.20f Pilha=%.20f\n", amostra, bestS, bestC, bestVel, pilha);
    }
}
