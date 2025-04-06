#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define NUM_MAX 100
#define CANT_ELEM N/T
#define DESCENDING_ORDER(v,i,j) v[i]>=v[j]
#define ASCENDING_ORDER(v,i,j) v[i]<=v[j]
typedef struct{
    int orden; //ORDEN=0 --> DECRECIENTE
    int check;
} ordenStatus;

/*---------VARIABLES COMPARTIDAS---------*/
int T;
int N;
int* vector;
int resultado;
ordenStatus* chequeo;
/*---------------------------------------*/


/*-----------------MUTEX-----------------*/

/*---------------------------------------*/


/*----------------BARRERA----------------*/
pthread_barrier_t barrera;
/*---------------------------------------*/

int chequeoCreciente(int* vec, int i, int fin){
    int check=1;
    while(check&&(i<fin-1)){
        check=ASCENDING_ORDER(vec,i,++i);//(vec[i]<=vec[++i]);
    }
    return check;
}
int chequeoDecreciente(int* vec, int i, int fin){
    int check=1;
    while(check&&(i<fin-1)){
        check=DESCENDING_ORDER(vec,i,++i);//(vec[i]>=vec[++i]);
    }
    
    return check;
}


//Para calcular tiempo
double dwalltime(){
    double sec;
    struct timeval tv;

    gettimeofday(&tv,NULL);
    sec = tv.tv_sec + tv.tv_usec/1000000.0;
    return sec;
}


void* function(void* arg){
    int tid=*(int*)arg;
    printf("Hilo id:%d\n",tid);

    
    int inicio=tid*CANT_ELEM;
    int fin=inicio+CANT_ELEM;

    //RECORRIDO DEL VECTOR -> determinación de orden y tipo
    if (vector[inicio] >= vector[++inicio]){
        chequeo[tid].orden=0;
        chequeo[tid].check = chequeoDecreciente(vector,inicio,fin);
    }else {
        chequeo[tid].orden=1;
        chequeo[tid].check = chequeoCreciente(vector,inicio,fin);
    }
    pthread_barrier_wait(&barrera);
    if(!tid){
        for(int i=0;i<T-1;i++){
            if (((chequeo[i].check)&&(chequeo[i+1].check)) && (chequeo[i].orden == chequeo[i+1].orden)){
                //los dos están ordenados
                inicio=(i+1)*CANT_ELEM-1;
                fin=inicio+1;
                if (chequeo[i].orden ? (ASCENDING_ORDER(vector,inicio,fin)) : (DESCENDING_ORDER(vector,inicio,fin))) { //el limite tiene sentido
                    resultado=1;
                }
                else {
                    resultado=0; 
                    break;
                }
            }else {
                resultado=0; 
                break;
            }
        }
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
    chequeo=(ordenStatus*)malloc(sizeof(ordenStatus)*T);


    //INICIALIZACION
    //srand(time(NULL));
    
    for (int i=0; i<N/2; i++){
        //vector[i]=rand()%NUM_MAX;
        vector[i]=i;
        vectorSec[i]=vector[i];
    }
    
    for (int i=N/2, c=N-1; i<N; i++,c--){
        //vector[i]=rand()%NUM_MAX;
        vector[i]=i;
        vectorSec[i]=vector[i];
    }
    printf("\n\n");


    pthread_barrier_init(&barrera, NULL, T);

    double timetick = dwalltime();
    for (int id=0;id<T;id++){
        threads_ids[id]=id;
        pthread_create(&misThreads[id],NULL,&function,(void*)&threads_ids[id]);
    }
    

    for (int id=0;id<T;id++){
        pthread_join(misThreads[id],NULL);
    }
    printf("Tiempo en segundos %f\n", dwalltime() - timetick);
    printf("Resultado monolitico: %d\n\n",resultado);
    pthread_barrier_destroy(&barrera);
    //SECUENCIAL

    free(vector);
    free(vectorSec);
    free(chequeo);
    
    return 0;
}



/*
6. Paralelizar un algoritmo que determine si un vector de N elementos es
monotónico. Un vector es monotónico si todos sus elementos están ordenados
en forma creciente o decreciente.
*/