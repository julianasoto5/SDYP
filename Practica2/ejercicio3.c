#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define ELEM_POR_THREAD(N,T) N/T
#define NUM_MAX 100
typedef struct {
    int max;
    int min;
} num;

/*---------VARIABLES COMPARTIDAS---------*/
num resultado;
int* vector;
int N;
int T;
/*---------------------------------------*/

/*---------MUTEX---------*/
pthread_mutex_t mutexMax = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexMin = PTHREAD_MUTEX_INITIALIZER;
/*-----------------------*/


void* function(void* arg){
    int tid=*(int*)arg;
    printf("Hilo id:%d\n",tid);
    //Código que ejecutará cada hilo
    int inicio = tid*ELEM_POR_THREAD(N,T);
    int fin = (tid+1)*ELEM_POR_THREAD(N,T);
    num resul;
    resul.max=vector[inicio];
    resul.min=vector[inicio];
    for (int i=inicio+1; i<fin; i++){
        if(vector[i]>resul.max)    
            resul.max = vector[i];
        if(vector[i]<resul.min)
            resul.min = vector[i];
    }

    pthread_mutex_lock(&mutexMax);
    if(resultado.max<resul.max)
        resultado.max=resul.max;
    pthread_mutex_unlock(&mutexMax);
    pthread_mutex_lock(&mutexMin);
    if(resultado.min>resul.min)
        resultado.min=resul.min;
    pthread_mutex_unlock(&mutexMin);

    pthread_exit(NULL);
}

//Para calcular tiempo
double dwalltime(){
    double sec;
    struct timeval tv;

    gettimeofday(&tv,NULL);
    sec = tv.tv_sec + tv.tv_usec/1000000.0;
    return sec;
}

int main(int argc, char*argv[]){
    T = atoi(argv[1]);
    pthread_t misThreads[T];
    int threads_ids[T];


    
    N = atoi(argv[2]);
    vector=(int*)malloc(sizeof(int)*N);
    printf("Se ejecutarán %d hilos para procesar un vector de %d elementos\n",T,N);
    //INICIALIZACION
    srand(time(NULL));
    for (int i=0; i<N; i++)
        vector[i]=rand()%NUM_MAX;


    resultado.max=-1;
    resultado.min=NUM_MAX;
    double timetick = dwalltime();
    for (int id=0;id<T;id++){
        threads_ids[id]=id;
        pthread_create(&misThreads[id],NULL,&function,(void*)&threads_ids[id]);
    }

    for (int id=0;id<T;id++){
        pthread_join(misThreads[id],NULL);
    }

    printf("Tiempo en segundos %f\n", dwalltime() - timetick);
    printf("Máximo: %d\nMínimo: %d\n",resultado.max,resultado.min);

    free(vector);
    pthread_mutex_destroy(&mutexMax);
    pthread_mutex_destroy(&mutexMin);
    return 0;
}


/*
3. Paralelizar la búsqueda del mínimo y el máximo valor en un vector de N
elementos. Ejecutar con 2 y 4 Threads.
*/