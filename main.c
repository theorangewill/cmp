/** @file main.c
 *  @brief Algoritmo CMP
 *
 *  @author William F. C. Tavares (theorangewill).
 */

#include "seismicunix.h"
 

void liberarMemoria(ListaTracos **lista, int *tamanho);


int main (int argc, char **argv)
{

    if(argc < 2){
        printf("ERRO: ./cmp <dado sismico>\n");
        exit(1);
    }

    ListaTracos **listaTracos;
    int tamanhoLista = 0;

    if(!LeitorArquivoSU(argv[1], listaTracos, &tamanhoLista)){
        printf("ERRO NA LEITURA\n");
        exit(1);
    }

    //liberarMemoria(listaTracos, &tamanhoLista);

    return 1;
}

void liberarMemoria(ListaTracos **lista, int *tamanho)
{
    int i, j;
    for(i=0; i<*tamanho; i++){
        for(j=0; j<lista[i]->tamanho; j++){
            free(lista[i]->tracos[j]);
        }
        
        free(lista[i]->tracos);
    }
    free(lista);
    tamanho = 0;
}