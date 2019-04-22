/** @file seismicunix.h
 *  @brief Biblioteca dos dados Seismic Unix
 *
 *  @author William F. C. Tavares (theorangewill).
 */

#ifndef STDIO_H
#include <stdio.h>
#define STDIO_H
#endif

#ifndef STDLIB_H
#include <stdlib.h>
#define STDLIB_H
#endif


#ifndef MATH_H
#include <math.h>
#define MATH_H
#endif

#define SEISMIC_UNIX_HEADER 240

/*! \brief Registro do traço sísmico.
 *  FONTE: http://www.geo.uib.no/eworkshop/index.php?n=Main.SeismicUnix
 *         https://github.com/JohnWStockwellJr/SeisUnix/wiki/Seismic-Unix-data-format
 *         http://www.geophysik.uni-jena.de/Lehre/Master+of+Science/Reflexionsseismik/SEG_Y+Trace+Header+Format.html
*/
typedef struct {
  int tracl; /**< . */
  int tracr; /**< . */
  int fldr; /**< . */
  int tracf; /**< . */
  int ep; /**< . */
  int cdp; /**< Common-depth point. */
  int cdpt; /**< . */
  short int trid; /**< Identificador do traço: (1) dado (2) morto (3) dummy (4) time break (5) uphole (6) sweep (7) timing (8) water break (9+) opcional. */
  short int nvs; /**< . */
  short int nhs; /**< . */
  short int duse; /**< . */
  int offset; /**< Distancia entre a fonte e o receptor. */
  int gelev; /**< . */
  int selev; /**< . */
  int sdepth; /**< . */
  int gdel; /**< . */
  int sdel; /**< . */
  int sqdep; /**< . */
  int gwdep; /**< . */
  short int scalel; /**< . */
  short int scalco; /**< Escalar utilizado para converter sx, sy, gx, gy para numeros reais (Se positivo multiplica-se, se negativo divide-se). */
  int sx; /**< Coordenada X da fonte. */
  int sy; /**< Coordenada Y da fonte. */
  int gx; /**< Coordenada X dos receptores. */
  int gy; /**< Coordenada Y dos receptores. */
  int counit; /**< . */
  short int wevel; /**< . */
  short int swevel; /**< . */
  short int sut; /**< . */
  short int gut; /**< . */
  short int sstat; /**< . */
  short int gstat; /**< . */
  short int tstat; /**< . */
  short int laga; /**< . */
  short int delrt; /**< . */
  short int muts; /**< . */
  short int mute; /**< . */
  short int ns; /**< Número de amostras. */
  short int dt; /**< Intervado das amostras em microsegundos. */
  short int gain; /**< . */
  short int igc; /**< . */
  short int igi; /**< . */
  short int corr; /**< . */
  short int sfs; /**< . */
  short int sfe; /**< . */
  short int slen; /**< . */
  short int styp; /**< . */
  short int stas; /**< . */
  short int stae; /**< . */
  short int tatyp; /**< . */
  short int afilf; /**< . */
  short int afils; /**< . */
  short int nofilf; /**< . */
  short int nofils; /**< . */
  short int lcf; /**< . */
  short int hcf; /**< . */
  short int lcs; /**< . */
  short int hcs; /**< . */
  short int year; /**< . */
  short int day; /**< . */
  short int hour; /**< . */
  short int minute; /**< . */
  short int sec; /**< . */
  short int timbas; /**< . */
  short int trwf; /**< . */
  short int grnors; /**< . */
  short int grnofr; /**< . */
  short int grnlof; /**< . */
  short int gaps; /**< . */
  short int otrav; /**< . */
  float d1; /**< . */
  float f1; /**< . */
  float d2; /**< . */
  float f2; /**< . */
  float ungpow; /**< . */
  float unscale; /**< . */
  int ntr; /**< . */
  short int mark; /**< . */
  short int shortpad; /**< . */
  int unass[7]; /**< . */
  float *dados; /**< . */
}Traco;


/*! \brief Registro de conjunto de traços sísmicos de mesmo CDP.
*/
typedef struct ListaTracos ListaTracos;

struct ListaTracos{
  int cdp; /**< CDP do conjunto. */
  int capacidade; /**< Tamanho alocado para o vetor tracos. */
  int numeroVizinhos; /**< Numero de vizinhos. */
  struct ListaTracos **vizinhos; /**< CDPs vizinhos. */
  int tamanho; /**< Quantidade de tracos. */
  Traco **tracos; /**< Tracos. */
};

/*
 * Le o arquivo do dado sismico SU.
 */
int LeitorArquivoSU(char* arquivo, ListaTracos ***listaTracos, int *tamanhoLista, float aph);

/*
 * Retorna o scalco multiplicado (se positivo) ou dividindo (se negativo).
 */
float ScalcoSU(Traco *traco);

/*
 * Calcula a metade do offset.
 */
void OffsetSU(Traco *traco, float *hx, float *hy);


/*
 * Calcula o ponto médio.
 */
void MidpointSU(Traco *traco, float *mx, float *my);

/*
 * Computa os vizinhos.
 */
void ComputarVizinhos(ListaTracos **lista, int tamanho, int traco, float md);

/*
 * Função para comparar dois offsets
 */
int comparaOffset(const void* a, const void* b);

/*
 * Função para comparar dois cdps
 */
int comparaCDP(const void* a, const void* b);

/*
 * Imprime os vizinhos de um CDP
 */
void PrintVizinhosSU(ListaTracos *tracos);

/*
 * Imprime o cabecalho do traco.
 */
void PrintTracoCabecalhoSU(Traco *traco);

/*
 * Imprime o traco.
 */
void PrintTracoSU(Traco *traco);

/*
  * Imprime lista de tracos.
  */
void PrintListaTracosSU(ListaTracos **lista, int tamanho);

/*
  * Libera memória de uma lista de tracos.
  */
void LiberarMemoriaSU(ListaTracos ***lista, int *tamanho);
