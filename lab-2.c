#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

#define _USE_MATH_DEFINES

long int N; //dimensão do vetor de entrada
int nthreads;//numero de threads
double *vetor; //vetor de entrada com dimensão dim


void * tarefa(void * arg){
    long int id = (long int) arg;
    double *somaLocal;
    long int tamBloco = N / nthreads;
    long int ini = id * tamBloco;
    long int fim;
    somaLocal = (double *) malloc(sizeof(double));
    if(somaLocal == NULL){
        fprintf(stderr, "ERRO--malloc\n");
        exit(1);
    }
    *somaLocal = 0;
    if(id == nthreads-1) fim = N;
    else fim = ini + tamBloco;
    for(long int i = ini; i < fim; i++)
        *somaLocal += vetor[i];
    pthread_exit((void *) somaLocal);
}

int main(int argc, char *argv[]){

    double somaSeq = 0;
    double somaConc = 0;
    double ini, fim;
    pthread_t *tid;
    double *retorno;//valor de retorno das threads

    //recebe e valida os paramentros de entrada
    if(argc < 3){
        fprintf(stderr, "Digite: %s <dimensão do vetor> <numero de threads>\n", argv[0]);
        return 1;
    }
    N = atoll(argv[1]);
    nthreads = atoi(argv[2]);

    //aloca o valor de entrada
    vetor = (double *)  malloc(sizeof(double)*N);
    if(vetor == NULL){
        fprintf(stderr, "ERRO--malloc\n");
        return 2;
    }

    //preenche o vetor de entrada
    vetor[0] = 1;
    for(long int i = 1; i < N; i++){
        vetor[i] = pow((-1), i)*(1.0/(2*i+1));
    }
    //soma sequencial dos elementos
    GET_TIME(ini);
    for(long int i = 0; i < N; i++)
        somaSeq += vetor[i];
    GET_TIME(fim);
    printf("Tempo sequencial: %lf\n", fim-ini);

    //soma concorrente dos elementos
    GET_TIME(ini);
    tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
    if(tid == NULL){
        fprintf(stderr, "ERRO--malloc\n");
        return 2;
    }

    //criar threads
    for(long int i = 0; i < nthreads; i++){
        if(pthread_create(tid + i, NULL, tarefa, (void*) i)){
            fprintf(stderr, "ERRO--pthread_create\n");
            return 3;
        }
    }

    //aguardar o termino das threads
    for(int i = 0; i < nthreads; i++){
        if(pthread_join(*(tid + i), (void**) &retorno)){
            fprintf(stderr, "ERRO--pthread_create\n");
            return 3;
        }
        //soma global
        somaConc += *retorno;
        free(retorno);
    }
    GET_TIME(fim);
    printf("Tempo concorrente: %lf\n", fim-ini);

    //exibir os resultados
    somaSeq = 4*somaSeq;
    somaConc = 4*somaConc;
    printf("soma seq: %.15f\n", somaSeq);
    printf("soma conc: %.15f\n", somaConc);
    printf("diff seq: %.15f\n", M_PI - somaSeq);
    printf("diff conc: %.15f\n", M_PI - somaConc);

    //libera as areas de memoria alocadas
    free(vetor);
    free(tid);

    return 0;
}