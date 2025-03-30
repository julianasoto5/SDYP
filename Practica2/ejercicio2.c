#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#define ELEM_POR_THREAD(N,T) N/T

/*---------VARIABLES COMPARTIDAS---------*/
int N;
int T;
int X;
int ocurrencias=0;
int* vector;
/*---------------------------------------*/

//MUTEX
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* cantOcurrencias(void* arg){
    int tid=*(int*)arg;
    printf("Hilo id:%d\n",tid);

    int cant=0;
    int fin = (tid+1)*ELEM_POR_THREAD(N,T);
    int inicio = tid*ELEM_POR_THREAD(N,T);
    for (int i=inicio; i<fin; i++){
        cant+=(vector[i]==X);
    }
    pthread_mutex_lock(&mutex);
    ocurrencias+=cant;
    pthread_mutex_unlock(&mutex);
    
    pthread_exit(NULL);
    
}


int cantOcurrenciasSec(){
    int cant=0;
    for (int i=0; i<N;i++){
        cant+=(vector[i]==X);
    }

    return cant;
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

    X = atoi(argv[3]);

    //INICIALIZACION
    srand(time(NULL));
    for (int i=0; i<N; i++)
        vector[i]=rand()%100;

        printf("EJECUCIÓN SECUENCIAL\n");
        double timetick = dwalltime();
        int cant = cantOcurrenciasSec();
        printf("Tiempo en segundos %f\n", dwalltime() - timetick);
        printf("Cantidad de ocurrencias del nro %d: %d\n",X,cant);
    
      
        printf("EJECUCIÓN PARALELA");
    timetick = dwalltime();
    for (int id=0;id<T;id++){
        threads_ids[id]=id;
        pthread_create(&misThreads[id],NULL,&cantOcurrencias,(void*)&threads_ids[id]);
    }
     

    for (int id=0;id<T;id++){
        pthread_join(misThreads[id],NULL);
    }

    printf("Tiempo en segundos %f\n", dwalltime() - timetick);
    printf("Cantidad de ocurrencias del nro %d: %d\n",X,ocurrencias);


    free(vector);
    
    pthread_mutex_destroy(&mutex);
    return 0;
}


/*
Paralelizar un algoritmo que cuente la cantidad de veces que un elemento X
aparece dentro de un vector de N elementos enteros. Al finalizar, la cantidad
de ocurrencias del elemento X debe quedar en una variable llamada
ocurrencias. Ejecutar con 2 y 4 threads.

    T=2
        0.002134
    T=4
        0.000677

*/