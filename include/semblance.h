/** @file semblance.h
 *  @brief Biblioteca da operação Semblance
 *
 *  @author William F. C. Tavares (theorangewill).
 */

#ifndef STDIO_H
#include <stdio.h>
#define STDIO_H
#endif

#ifndef MATH_H
#include <math.h>
#define MATH_H
#endif

#ifndef STRING_H
#include <string.h>
#define STRING_H
#endif

#ifndef SEISMICUNIX_H
#include "seismicunix.h"
#define SEISMICUNIX_H
#endif

/*
 * Implementação da função semblance
 */
float Semblance(ListaTracos *lista, float A, float B, float C, float t0, float wind, float seg, float *pilha, float azimuth);


float SemblanceCMP(ListaTracos *lista, float A, float B, float C, float t0, float wind, float seg, float *pilha, float azimuth);

/*
 * Calcula a metade do offset.
 */
float HalfOffset(Traco *traco, float azimuth);


/*
 * Realiza interpolacao linear.
 */
void InterpolacaoLinear(float *x, float x0, float x1, float y, float y0, float y1);