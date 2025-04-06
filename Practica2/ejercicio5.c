#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <bits/pthreadtypes.h>

#define CANT_ELEM N/T
#define NUM_MAX 100

/*---------VARIABLES COMPARTIDAS---------*/
int T;
int N;
int* vector;
int* resul;
int* parcial0;
int* parcial1;
/*---------------------------------------*/


/*-----------------BARRERA-----------------*/

pthread_barrier_t* barrera; //DECLARACIÓN BARRERA

/*---------------------------------------*/


//Para calcular tiempo
double dwalltime(){
    double sec;
    struct timeval tv;

    gettimeofday(&tv,NULL);
    sec = tv.tv_sec + tv.tv_usec/1000000.0;
    return sec;
}


void merge(int j, int k, int* vec, int* resultado, int size){ 
    
    int i = j;               // índice donde guardamos en resultado
    int j_end = k;           // fin del primer bloque
    int k_end = k + size / 2; // fin del segundo bloque

    while (j < j_end && k < k_end) {
        if (vec[j] <= vec[k])
            resultado[i++] = vec[j++];
        else
            resultado[i++] = vec[k++];
    }

    // Copiar lo que quede
    while (j < j_end) resultado[i++] = vec[j++];
    while (k < k_end) resultado[i++] = vec[k++];
}

void ordenacion(int size, int inicio, int fin){
    int temp;
    for(int i=0; i<size-1;i++){
        for(int j=inicio; j<fin-i-1; j++){
            if(vector[j]>vector[j+1]){
                temp=vector[j];
                vector[j]=vector[j+1];
                vector[j+1]=temp;
            }
        }
    }
}



void* function(void* arg){
    int tid=*(int*)arg;
    printf("Hilo id:%d\n",tid);
    
    int k,j;
    int temp;
    int inicio=tid*CANT_ELEM;
    int fin=inicio+CANT_ELEM;

    ordenacion(CANT_ELEM,inicio,fin); //Cada hilo ordena SU parte del vector

    int trozos=T; //T multiplo de 2
    int paridad = 1;
    int primero = 1;
    int s;
    int numBarrera = 0;
    while (trozos != 1){ //haya hilos ejecutando
        pthread_barrier_wait(&barrera[numBarrera++]); //esperamos hilos en la barrera

        if(tid>=(trozos/2)) //se descartan la mitad de los hilos para que ahora la otra mitad
            break;          //se encargue de hacer el merge entre dos subvectores ordenados consecutivos
        else{
            /*
            Cada hilo va a estar manejando una parte del vector. 
            Se van a mover entre los indices j y k.
            El tamaño del vector sobre el que van a hacer el merge está
            determinado por la variable s (size). Notar que es el mismo para
            todos los hilos en una misma ejecucion. 
            */

            j= (tid)*2*(N/trozos); //Calculamos el inicio del subvector ordenado
            k= j + (N/trozos);  //Calculamos el inicio del siguiente subvector ordenado
            s= 2*(N/trozos);  //Calculamos tamaño de los subvectores

            if(primero){
                merge(j,k,vector, parcial1,s);
                primero = 0;
            } else if(paridad){
                merge(j,k,parcial1, parcial0,s);
                paridad=0;
            } else {
                merge(j,k,parcial0, parcial1,s);
                paridad=1;
            }

        }
        trozos/=2; //disminuimos cantidad de hilos
    }
    if (tid == 0){
        resul = (paridad) ? parcial1 : parcial0;
    }

    pthread_exit(NULL);
}

int main(int argc, char*argv[]){
    T = atoi(argv[1]);
    N = atoi(argv[2]);

    if ((T & (T - 1)) != 0) {
        printf("El número de hilos debe ser potencia de 2.\n");
        return 1;
    }
    if (N % T != 0) {
        printf("N debe ser divisible por T.\n");
        return 1;
    }

    pthread_t misThreads[T];
    int threads_ids[T];

    
    vector=(int*)malloc(sizeof(int)*N);
    int* vectorSec=(int*)malloc(sizeof(int)*N);
    //INICIALIZACION
    //srand(time(NULL));
    for (int i=0; i<N; i++){
        vector[i]=rand()%NUM_MAX;
        vectorSec[i]=vector[i];
    }
    printf("Se ejecutarán %d hilos para procesar un vector de %d elementos\n",T,N);


    parcial0=(int*)malloc(sizeof(int)*N);
    parcial1=(int*)malloc(sizeof(int)*N);

    barrera=(pthread_barrier_t*)malloc((sizeof(pthread_barrier_t)*(1+(T/4))));

    pthread_barrier_init(&barrera[0], NULL, T); //barrera del sort
    int hilos_en_nivel = T;
    
    for (int i = 0; i < 1+(T/4); i++) {
        pthread_barrier_init(&barrera[i], NULL, hilos_en_nivel);
        hilos_en_nivel /= 2;
    }

    double timetick = dwalltime();
    for (int id=0;id<T;id++){
        threads_ids[id]=id;
        pthread_create(&misThreads[id],NULL,&function,(void*)&threads_ids[id]);
    }
    

    for (int id=0;id<T;id++){
        pthread_join(misThreads[id],NULL);
    }
   printf("Tiempo en segundos %f\n", dwalltime() - timetick);

    //CHEQUEO
    int check=1;
    int i=0;
    while(check&&(i<N-1)){
        check=(resul[i]<=resul[++i]);
    }
    printf("Resultado ordenación: %d\n",check);
    

    for (int i= 0; i<(T/4)+1; i++){
        pthread_barrier_destroy(&barrera[i]);
    }

    /*
    printf("EJECUCION SECUENCIAL\n");
    timetick=dwalltime();
    ordenacion(N,0,N);
    printf("Tiempo en segundos %f\n", dwalltime() - timetick);
*/
    
    free(vector);
    free(vectorSec);
    free(parcial0);
    free(parcial1);

    return 0;
}


/*
5. Paralelizar la ordenación por mezcla de un vector de N elementos. Ejecutar
con 2 y 4 Threads.
*/

