/** @file semblance.c
 *  @brief Implementação da biblioteca semblance.h
 *
 *  @author William F. C. Tavares (theorangewill).
 */

#ifndef SEMBLANCE_H
#include "semblance.h"
#define SEMBLANCE_H
#endif

float Semblance(ListaTracos *lista, float C, float t0, float wind, float seg, float *pilha)
{
    int traco;
    float t, h;
    int amostra, k;
    int w = (int) (wind/seg);
    int janela = 2*w+1;
    int N;
    float numerador[janela], denominador;
    float num;
    float valor;
    int j;
    int erro;

    //Numerador e denominador da funcao semblance zerados
    memset(&numerador,0,sizeof(numerador));
    denominador = 0;
    //Para cada traco do conjunto
    N = 0;
    erro = 0;
    for(traco=0; traco<lista->tamanho; traco++){
        //Calcular metade do offset do traco
        h = HalfOffset(lista->tracos[traco]);
        //Calcular o tempo de acordo com a funcao da hiperbole
        t = sqrt(t0*t0 + C*h*h);
        //Calcular a amostra equivalente ao tempo calculado
        amostra = (int) (t/seg);
        //Se a janela da amostra cobre os dados sismicos
        if(amostra - w >= 0 && amostra + w < lista->tracos[traco]->ns){
            //Para cada amostra dentro da janela
            for(j=0; j<janela; j++){
                k = amostra - w + j;
                //Interpolacao linear entre as duas amostras
                InterpolacaoLinear(&valor,lista->tracos[traco]->dados[k],lista->tracos[traco]->dados[k+1], t/seg-w+j, k, k+1);
                numerador[j] += valor;
                denominador += valor*valor;
                *pilha += valor;
            }
            N++;
        }
        else{
            erro++;
        }
        if(erro == 2) return -1;
    }
    num = 0;
    for(j=0; j<janela; j++){
        num += numerador[j]*numerador[j];
    }

    *pilha = (*pilha)/N/janela;

    return num / N / denominador;

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

