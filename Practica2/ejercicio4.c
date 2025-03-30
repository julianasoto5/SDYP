#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define NUM_MAX 100
/*---------VARIABLES COMPARTIDAS---------*/
int T;
int N;
int* vector;
int total = 0;
double prom;
/*---------------------------------------*/


/*-----------------MUTEX-----------------*/
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
/*---------------------------------------*/

//Para calcular tiempo
double dwalltime(){
    double sec;
    struct timeval tv;

    gettimeofday(&tv,NULL);
    sec = tv.tv_sec + tv.tv_usec/1000000.0;
    return sec;
}


void* calcularPromedio(void* arg){
    int tid=*(int*)arg;
    printf("Hilo id:%d\n",tid);
    //Código que ejecutará cada hilo

    int inicio = tid*N/T;
    int fin = (tid+1)*N/T;
    int cant=0;
    for (int i=inicio; i<fin;i++){
        cant+=vector[i];  
    }

    pthread_mutex_lock(&mutex);
    total+=cant;
    pthread_mutex_unlock(&mutex);



    pthread_exit(NULL);
}

int main(int argc, char*argv[]){
    printf("EJECUCIÓN PARALELA\n\n");
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



    double timetick = dwalltime();
    for (int id=0;id<T;id++){
        threads_ids[id]=id;
        pthread_create(&misThreads[id],NULL,&calcularPromedio,(void*)&threads_ids[id]);
    }
    
    for (int id=0;id<T;id++){
        pthread_join(misThreads[id],NULL);
    }
    
    prom=total/N;
    printf("Tiempo en segundos %f\n", dwalltime() - timetick);
    printf("Promedio del vector: %.3f\n",prom);

    printf("EJECUCIÓN SECUENCIAL\n\n");
    int cant=0;
    timetick=dwalltime();
    for(int i=0; i<N;i++)
        cant+=vector[i];
    double promSec = cant/N;
    printf("Tiempo en segundos %f\n", dwalltime() - timetick);
    printf("Promedio del vector: %.3f\n",promSec);

    //LIBERACIONES - DESTRUCCIONES
    pthread_mutex_destroy(&mutex);
    free(vector);


    return 0;
}


/*
4. Paralelizar un algoritmo que calcule el valor promedio de N elementos
almacenados en un vector de tamaño N. Ejecutar con 2 y 4 Threads.
*/