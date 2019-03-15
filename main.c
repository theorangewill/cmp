/** @file main.c
 *  @brief Algoritmo CMP
 *
 *  @author William F. C. Tavares (theorangewill).
 */

#include "seismicunix.h"
#include <math.h>
#include <string.h>

/*
 * Algoritmo CMP.
 */
void CMP(ListaTracos *lista, float velini, float velfin, float incr, float wind, Traco* tracoEmpilhado, Traco* tracoSemblance, Traco* tracoC);

/*
 * Implementação da função semblance
 */
float Semblance(ListaTracos *lista, float C, float t0, float wind, float seg, float *pilha, float vel);

/*
 * Realiza interpolacao linear.
 */
void InterpolacaoLinear(float *x, float x0, float x1, float y, float y0, float y1);

/*
 * Seta os campos do cabecalho para o empilhamento
 */
void SetCabecalhoCMP(Traco *traco);

/*
 * Calcula a metade do offset.
 */
float HalfOffset(Traco *traco);

/*
 * Libera memoria alocada para o programa.
 */
void LiberarMemoria(ListaTracos ***lista, int *tamanho);

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

        printf("\t%d (cdp= %d) de %d\n", tracos, listaTracos[tracos]->cdp, tamanhoLista);
        //PrintTracoSU(listaTracos[tracos]->tracos[0]);

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
        printf("\t%d (cdp= %d) de %d\n", tracos, listaTracos[tracos]->cdp, tamanhoLista);
    break;
    }

    fclose(arquivoEmpilhado);
    fclose(arquivoSemblance);
    fclose(arquivoC);

    LiberarMemoria(&listaTracos, &tamanhoLista);

    return 1;
}

float Semblance(ListaTracos *lista, float C, float t0, float wind, float seg, float *pilha, float vel)
{
    int traco;
    float t, h;
    int amostra, k;
    //int w = (int) ceil(wind/seg);
    int w = (int) wind;
    int janela = 2*w+1;
    int N;
    float numerador[janela], denominador;
    float num;
    float valor; 
    int j;
    int erro;
    //printf("w=%d janela=%d\n", w, janela);
    //Numerador e denominador da funcao semblance zerados
    memset(&numerador,0,sizeof(numerador));
    denominador = 0;
    //Para cada traco do conjunto
    N = 0;
    erro = 0;
    //if(vel>439.0 && vel<440) printf("%f <<<< ", vel);
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
                //printf("valor: %f [k]:%f [k+1]:%f t/seg-w+j=%f, k:%d, k+1:%d\n", valor,lista->tracos[traco]->dados[k],lista->tracos[traco]->dados[k+1], t/seg-w+j, k, k+1);
                numerador[j] += valor;
                denominador += valor*valor;
                *pilha += valor;
                //if(vel>439.0 && vel<440) printf("%f ", valor);
            }
            N++;
        }
        else{
            erro++;
        }
        //if(vel>439.0 && vel<440) printf(".\n");
        //printf("ERRO=%d amostra=%d w=%d ns=%d  (%d >= 0 && %d < %d)\n", erro, amostra, w, lista->tracos[traco]->ns, amostra-w, amostra+w, lista->tracos[traco]->ns);
        if(erro == 2) return -1;
    }
    //if(vel>439.0 && vel<440) getchar();
    num = 0;
    for(j=0; j<janela; j++){
        num += numerador[j]*numerador[j];
    }
    //printf("%.10f %.10f %d (%d)  \t\t", num, denominador, N, lista->tamanho);
    *pilha = (*pilha)/N/janela;

    return num / N / denominador;

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

    incr = (velfin-velini)/incr;

    //Para cada amostra do primeiro traco
    for(amostra=0; amostra<amostras; amostra++){
        //Calcula o segundo inicial
        t0 = amostra*seg;

        //Inicializar variaveis antes da busca
        pilha = lista->tracos[0]->dados[amostra];
        bestVel = velini;
        bestC = 4/(velini*velini);
        bestS = 0;
        //Para cada velocidade
        for(vel=velini; vel<=velfin; vel+=incr){
            //Calcular o termo do calculo da hiperbole
            C = 4/(vel*vel);
            //Calcular semblance
            pilhaTemp = 0;
            s = Semblance(lista,C,t0,wind,seg,&pilhaTemp,vel);

            if(s<0 && s!=-1) printf("S NEGATIVO\n");
            if(s>1) printf("S MAIOR Q UM\n");
            //if(s == -1) break; //printf("ERRO NO SEMBLANCE");
            else if(s > bestS){  
                printf("\n*****%d S=%.10f C=%.20f Vel=%.10f Pilha=%.10f\n", amostra, s, C, vel, pilhaTemp);
                bestS = s;
                bestC = C;
                bestVel = vel;
                pilha = pilhaTemp;
            }
            else{
                ;//printf("\n%d S=%.10f C=%.20f Vel=%.10f Pilha=%.10f\n", amostra, s, C, vel, pilhaTemp);
            }
            if(vel >=1830 && vel <=1831) 
                printf("\n>>>%d S=%.10f C=%.20f Vel=%.10f Pilha=%.10f\n", amostra, s, C, vel, pilhaTemp);
        }
        break;
        tracoEmpilhado->dados[amostra] = pilha;
        tracoSemblance->dados[amostra] = bestS;
        tracoC->dados[amostra] = bestC;
        printf("\n%d S=%.10f C=%.20f Vel=%.10f Pilha=%.10f\n", amostra, bestS, bestC, bestVel, pilha);
    }
}

void InterpolacaoLinear(float *x, float x0, float x1, float y, float y0, float y1)
{
    *x = x0 + (x1- x0) * (y - y0) / (y1 - y0);
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


void LiberarMemoria(ListaTracos ***lista, int *tamanho)
{
    LiberarMemoriaSU(lista,tamanho);
}
