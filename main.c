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
void CMP(ListaTracos *lista, float velini, float velfin, float incr, float wind);

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

    if(argc < 6){
        printf("ERRO: ./main <dado sismico> VELINI VELFIN INCR WIND\n");
        printf("\tARQUIVO: rodar susort <entrada.su >saida.su cdp offset (ordenar os traços em cdp e offset\n");
        printf("\tVELINI:  velocidade inicial\n");
        printf("\tVELFIN:  velocidade final\n");
        printf("\tINCR:    increasement na velocidade\n");
        printf("\tWIND:    janela do semblance\n");
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

    //Rodar o CMP para cada conjunto de tracos de mesmo cdp
    for(tracos=0; tracos<tamanhoLista; tracos++){
        CMP(listaTracos[tracos],velini,velfin,incr,wind);
    }

    LiberarMemoria(&listaTracos, &tamanhoLista);

    return 1;
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

float Semblance(ListaTracos *lista, float C, float t0, float wind, float seg)
{
    int traco;
    float t, h;
    int amostra, k;
    int w = (int) ceil(wind/seg);
    int janela = 2*w+1;
    int N;
    float numerador[janela], denominador[janela];
    float num, dem;
    float valor; 
    int j;

    //Numerador e denominador da funcao semblance zerados
    memset(&numerador[0],0,sizeof(numerador));
    memset(&denominador[0],0,sizeof(denominador));

    //Para cada traco do conjunto
    N = 0;
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
                InterpolacaoLinear(&valor,lista->tracos[traco]->dados[k],lista->tracos[traco]->dados[k+1], t/seg-w+j, k, k+1);
                numerador[j] += valor;
                denominador[j] += valor*valor;
            }
            N++;
        }
        else return 0;
    }

    num = 0; dem = 0;
    for(j=0; j<janela; j++){
        num += numerador[j]*numerador[j];
        dem += denominador[j];
    }

    return num / N / dem;

}

void CMP(ListaTracos *lista, float velini, float velfin, float incr, float wind)
{
    int amostra, amostras;
    float vel;
    float seg, t0; 
    float h;
    float C, bestC;
    float s, bestS;

    //Tempo entre amostras, convertido para segundos
    seg = ((float) lista->tracos[0]->dt)/1000000;
    //Numero de amostras
    amostras = lista->tracos[0]->ns;

    //Para cada amostra do primeiro traco
    for(amostra=0; amostra<amostras; amostra++){
        //Calcula o segundo inicial
        t0 = amostra*seg;

        bestC = velini;
        bestS = 0;
        //Para cada velocidade
        for(vel=velini; vel<=velfin; vel+=incr){
            //Calcular o termo do calculo da hiperbole
            C = 4/(vel*vel);
            //Calcular semblance
            s = Semblance(lista,C,t0,wind,seg);
            if(s > bestS){
                bestS = s;
                bestC = C;
            }
        }
        printf("%d %f %f\n", amostra, bestS, bestC);
    }
    getchar();
}
