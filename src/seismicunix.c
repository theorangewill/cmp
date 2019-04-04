/** @file seismicunix.c
 *  @brief Implementação da biblioteca seismicunix.h
 *
 *  @author William F. C. Tavares (theorangewill).
 */

#ifndef SEISMICUNIX_H
#include "seismicunix.h"
#define SEISMICUNIX_H
#endif

int LeitorArquivoSU(char *argumento, ListaTracos ***listaTracos, int *tamanhoLista, float aph)
{
    int i;
    int flag;
    float hx, hy;
    Traco *traco;
    FILE *arquivo = fopen(argumento, "r");


	if(arquivo == NULL){
		return 0;
	}

    (*tamanhoLista) = 0;

    //Leitura de um traco por vez, ate o final do arquivo
    while(1){
        //Aloca memoria para um traco
        traco = (Traco*) malloc(sizeof(Traco));

        //Leitura do cabecalho do traco
        if(fread(traco, SEISMIC_UNIX_HEADER, 1, arquivo) < 1) break;
        
        //Aloca memoria para os dados sismicos
        //traco->ns numero de amostras
        traco->dados = malloc(sizeof(float) * traco->ns);

        //PrintTracoCabecalhoSU(traco);

        //Leitura das amostras
        if(fread(traco->dados, sizeof(float), traco->ns, arquivo) < 1) break;

        //Verificar o aperture
        OffsetSU(traco,&hx,&hy);
        hx/=2;
        hy/=2;
        if(hx*hx + hy*hy > aph*aph){
            free(traco->dados);
            free(traco);
            continue;
        }

        //PrintTracoSU(traco);

        flag = 0;
        //Insere o traco lido na lista que possui o mesmo cdp
        //criando uma lista de tracos com mesmo cdp
        for(i=0; i<*tamanhoLista; i++){
            if((*listaTracos)[i]->cdp == traco->cdp){
                if((*listaTracos)[i]->capacidade <= (*listaTracos)[i]->tamanho){
                    (*listaTracos)[i]->tracos = realloc((*listaTracos)[i]->tracos,((*listaTracos)[i]->tamanho+10)*sizeof(Traco*));
                    (*listaTracos)[i]->capacidade = (*listaTracos)[i]->tamanho+10;
                }
                (*listaTracos)[i]->tracos[(*listaTracos)[i]->tamanho] = traco;
                (*listaTracos)[i]->tamanho++;
                flag = 1;
                break;
            }
        }
        //Se nao existe uma lista com cdp do traco, eh criada uma nova lista
        if(!flag){
            if(*tamanhoLista == 0){
                *listaTracos = (ListaTracos**) malloc(sizeof(ListaTracos*));
            }
            else{
                *listaTracos = (ListaTracos**) realloc(*listaTracos,((*tamanhoLista)+1)*sizeof(ListaTracos*));
            }
            (*listaTracos)[*tamanhoLista] = (ListaTracos*) malloc(sizeof(ListaTracos));
            (*listaTracos)[*tamanhoLista]->cdp = traco->cdp;
            (*listaTracos)[*tamanhoLista]->capacidade = 10;
            (*listaTracos)[*tamanhoLista]->tamanho = 1;
            (*listaTracos)[*tamanhoLista]->numeroVizinhos = 0;
            (*listaTracos)[*tamanhoLista]->vizinhos = NULL;
            (*listaTracos)[*tamanhoLista]->tracos = (Traco**) malloc(sizeof(Traco*)*10);
            (*listaTracos)[*tamanhoLista]->tracos[0] = traco;
            (*tamanhoLista)++;
        }
    }

    fclose(arquivo);

    /*
    for(i=0; i<*tamanhoLista; i++){
        printf("cdp: %d (%d/%ld)\n", (*listaTracos)[i]->cdp,  (*listaTracos)[i]->tamanho,sizeof((*listaTracos)[i]->tracos));
        for(int j=0; j<(*listaTracos)[i]->tamanho; j++){
            float x, y, d;
            OffsetSU((*listaTracos)[i]->tracos[j],&x,&y);
            d = sqrt(x*x+y*y);
            printf("\t%f %f = %f  (%p)\n", x, y, d, (*listaTracos)[i]->tracos[j]);
        }
        printf("\n--\n");
        //getchar();
    }
    */

    //Ordenar por offset cada conjunto
    for(i=0; i<*tamanhoLista; i++)
        qsort((*listaTracos)[i]->tracos,(*listaTracos)[i]->tamanho,sizeof(Traco**),comparaOffset);

    //Ordenar por CDP
    qsort((*listaTracos),*tamanhoLista,sizeof(ListaTracos*),comparaCDP);

    //PrintListaTracosSU(*listaTracos,*tamanhoLista);
    return 1;
}

int comparaCDP(const void* a, const void* b)
{
    ListaTracos **A = (ListaTracos **) a;
    ListaTracos **B = (ListaTracos **) b;
    //printf("COMPARANDO %d %d\n", (*A)->cdp, (*B)->cdp);
    return (*A)->cdp - (*B)->cdp; 
}

int comparaOffset(const void* a, const void* b)
{
    Traco **A = (Traco **) a;
    Traco **B = (Traco **) b;
    float ax, ay, ad;
    float bx, by, bd;
    OffsetSU(*A,&ax,&ay);
    ad = sqrt(ax*ax+ay*ay);
    OffsetSU(*B,&bx,&by);
    bd = sqrt(bx*bx+by*by);
    return ad-bd;
}

float ScalcoSU(Traco *traco)
{
    //Se positivo, envia o dado
    if (traco->scalco > 0)
		return traco->scalco;
    //Se negativo, envia o inverso
	else if (traco->scalco < 0)
		return 1/traco->scalco;
    else return 1;
}

void OffsetSU(Traco *traco, float *hx, float *hy)
{
	float scalco;
    //Calcula o scalco, valor a ser multiplicado pelas dimensoes para torná-los numeros reais
    scalco = ScalcoSU(traco);
    //Eixo x
	*hx = scalco*(traco->gx-traco->sx);
    //Eixo y
	*hy = scalco*(traco->gy-traco->sy);
}


void MidpointSU(Traco *traco, float *mx, float *my)
{
	float scalco;
  //Calcula o scalco, valor a ser multiplicado pelas dimensoes para torná-los numeros reais
  scalco = ScalcoSU(traco);
  //Eixo x
	*mx = scalco*(traco->gx+traco->sx)/2;
    //Eixo y
	*my = scalco*(traco->gy+traco->sy)/2;
}

void ComputarVizinhos(ListaTracos **lista, int tamanho, int traco, float md)
{
    int i;
    float midx, midy;
    float vizx, vizy;
    float distx, disty;

    MidpointSU(lista[traco]->tracos[0], &midx, &midy);

    for(i=0; i<tamanho; i++){
        if(i == traco) continue;
        MidpointSU(lista[i]->tracos[0], &vizx, &vizy);
        distx = midx - vizx;
        disty = midy - vizy;
        if(distx*distx + disty*disty <= md*md){
            if(lista[traco]->numeroVizinhos == 0){
                lista[traco]->vizinhos = (ListaTracos**) malloc(sizeof(ListaTracos*));
            }
            else{
                lista[traco]->vizinhos = (ListaTracos**) realloc((lista[traco]->vizinhos),(lista[traco]->numeroVizinhos+1)*sizeof(ListaTracos*));
            }
            lista[traco]->vizinhos[lista[traco]->numeroVizinhos] = lista[traco];
            (lista[traco]->numeroVizinhos)++;
        }
    }
}


void PrintListaTracosSU(ListaTracos **lista, int tamanho)
{
    int i, j;
    for(i=0; i<tamanho; i++){
        printf("CDP: %d\t\t - %d (%d)\n", lista[i]->cdp, lista[i]->tamanho, lista[i]->capacidade);
        for(j=0; j<lista[i]->tamanho; j++){
            ;//printf("%p ", *(lista[i]->tracos[j]));
            //printf("%p (%d) ", lista[i]->tracos[j], lista[i]->tracos[j]->offset);
        }
        printf("\n");
    }
    printf("QUANTIDADE DE LISTAS: %d\n", tamanho);
}

void PrintTracoCabecalhoSU(Traco *traco)
{
    printf("tracl=%d\t\t\t tracr=%d\t\t fldr=%d\t\t\t tracf=%d\t\t ep=%d\t\t\t cdp=%d\t\t\t cdpt=%d\n", traco->tracl, traco->tracr, traco->fldr, traco->tracf, traco->ep, traco->cdp, traco->cdpt);
    printf("trid=%d\t\t\t nvs=%d\t\t\t nhs=%d\t\t\t duse=%d\n", traco->trid, traco->nvs, traco->nhs, traco->duse);
    printf("offset=%d\t\t gelev=%d\t\t selev=%d\t\t sdepth=%d\t\t gdel=%d\t\t sdel=%d\n", traco->offset, traco->gelev, traco->selev, traco->sdepth, traco->gdel, traco->sdel);
    printf("sqdep=%d\t\t\t gwdep=%d\t\t scalel=%d\t\t scalco=%d\n", traco->sqdep, traco->gwdep, traco->scalel, traco->scalco);
    printf("sx=%d\t\t sy=%d\t\t gx=%d\t\t gy=%d\t\t counit=%d\n", traco->sx, traco->sy, traco->gx, traco->gy, traco->counit);
    printf("wevel=%d\t\t\t swevel=%d\t\t sut=%d\t\t\t gut=%d\n", traco->wevel, traco->swevel, traco->sut, traco->gut);
    printf("sstat=%d\t\t\t gstat=%d\t\t tstat=%d\t\t laga=%d\n", traco->sstat, traco->gstat, traco->tstat, traco->laga);
    printf("delrt=%d\t\t\t muts=%d\t\t\t mute=%d\n", traco->delrt, traco->muts, traco->mute);
    printf("ns=%d\t\t\t dt=%d\t\t\t gain=%d\n", traco->ns, traco->dt, traco->gain);
    printf("igc=%d\t\t igi=%d\t\t\t corr=%d\n", traco->igc, traco->igi, traco->corr);
    printf("sfs=%d\t\t\t sfe=%d\t\t\t slen=%d\t\t\t styp=%d\t\t\t stas=%d\t\t\t stae=%d\n", traco->sfs, traco->sfe, traco->slen, traco->styp, traco->stas, traco->stae);
    printf("tatyp=%d\t\t\t afilf=%d\t\t afils=%d\t\t nofilf=%d\t\t nofils=%d\n", traco->tatyp, traco->afilf, traco->afils, traco->nofilf, traco->nofils);
    printf("lcf=%d\t\t\t hcf=%d\t\t\t lcs=%d\t\t\t hcs=%d\n", traco->lcf, traco->hcf, traco->lcs, traco->hcs);
    printf("year=%d\t\t\t day=%d\t\t\t hour=%d\t\t\t minute=%d\t\t sec=%d\n", traco->year, traco->day, traco->hour, traco->minute, traco->sec);
    printf("timbas=%d\t\t trwf=%d\t\t\t grnors=%d\t\t grnofr=%d\t\t grnlof=%d\n", traco->timbas, traco->trwf, traco->grnors, traco->grnofr, traco->grnlof);
    printf("gasp=%d\t\t\t otrav=%d\n", traco->gaps, traco->otrav);
    printf("d1=%f\t\t f1=%f\t\t d2=%f\t\t f2=%f\n", traco->d1, traco->f1, traco->d2, traco->f2);
    printf("ungpow=%f\t\t unscale=%f\t ntr=%d\t\t\t mark=%d\t\t\t shortpad=%d\n", traco->ungpow, traco->unscale, traco->ntr, traco->mark, traco->shortpad);
    printf("unass=%d %d %d %d %d %d %d\n", traco->unass[0], traco->unass[1], traco->unass[2], traco->unass[3], traco->unass[4], traco->unass[5], traco->unass[6]);
}

void PrintTracoSU(Traco *traco)
{
    float maior = 0, menor = 0;
    int idmaior = -1, idmenor = -1;
    for(int i = 0; i<traco->ns; i++){
        printf("%d\t%.12f\n", i, traco->dados[i]);
        if(traco->dados[i] > maior){
            maior = traco->dados[i];
            idmaior = i;
        }
        if(traco->dados[i] < menor){
            menor = traco->dados[i];
            idmenor = i;
        }
    }
    printf("\n");
    printf("[%d]=%f | [%d]=%f", idmaior, maior, idmenor, menor);
    printf("\n");
}

void LiberarMemoriaSU(ListaTracos ***lista, int *tamanho)
{
    int i, j;
    for(i=0; i<*tamanho; i++){
        for(j=0; j<(*lista)[i]->tamanho; j++){
            free((*lista)[i]->tracos[j]->dados);
            free((*lista)[i]->tracos[j]);
        }
        free((*lista)[i]->tracos);
        free((*lista)[i]);
    }
    //free(**lista);
    free(*lista);
    *tamanho = 0;
}
