#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define NUM_MAX 100
#define CANT_ELEM N/T
typedef struct{
    int orden;
    int check;
} ordenStatus;

/*---------VARIABLES COMPARTIDAS---------*/
int T;
int N;
int* vector;
int resultado;
ordenStatus* chequeo0;
ordenStatus* chequeo1;
/*---------------------------------------*/


/*-----------------MUTEX-----------------*/

/*---------------------------------------*/


/*----------------BARRERA----------------*/
pthread_barrier_t* barrera;
/*---------------------------------------*/

int chequeoCreciente(int* vec, int i, int fin){
    int check=1;
    while(check&&(i<fin-1)){
        check=(vec[i]<=vec[++i]);
    }
    return check;
}
int chequeoDecreciente(int* vec, int i, int fin){
    int check=1;
    while(check&&(i<fin-1)){
        check=(vec[i]>=vec[++i]);
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
        chequeo0[tid].orden=0;
        chequeo0[tid].check = chequeoDecreciente(vector,inicio,fin);
    }else {
        chequeo0[tid].orden=1;
        chequeo0[tid].check = chequeoCreciente(vector,inicio,fin);
    }
    
    ordenStatus *chequeo = chequeo0, *chequeo_sig = chequeo1, *aux;

    //determina si el vector es monolítico o no de forma recursiva
    if(inicio!=fin){
        int numBarrera=0;
        int trozos = T;
        
        while(trozos!=1){
            pthread_barrier_wait(&barrera[numBarrera++]);
            if(tid>=trozos/2) //libero la mitad de los hilos activos
                break;
            
            //tengo cant=trozos hilos, cada uno se va a fijar que el de al lado esté ordenado, y si lo está, que lo esté de la misma forma
            if ((chequeo[tid*2].check)&&(chequeo[tid*2+1].check)){
                //los dos están ordenados
                if(chequeo[tid*2].orden == chequeo[tid*2+1].orden) {
                    chequeo_sig[tid].orden=chequeo[tid*2].orden;
                    chequeo_sig[tid].check = 1;

                }else chequeo_sig[tid].check=0;
            }else chequeo_sig[tid].check=0;
            
            //no quise hacer un if-else para determinar a cuál vector ir :)
            aux = chequeo_sig;
            chequeo_sig = chequeo;
            chequeo = aux;

            trozos/=2; //disminuye hilos
        }
    }

    if(!tid){
        //es el tid=0
        resultado=chequeo[0].check;
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

    chequeo0=(ordenStatus*)malloc(sizeof(ordenStatus)*T); //en la primera ejecucion guardo estado del orden de T subvectores
    chequeo1=(ordenStatus*)malloc(sizeof(ordenStatus)*T/2);


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

    barrera=(pthread_barrier_t*)malloc((sizeof(pthread_barrier_t)*(1+(T/4))));

    
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
    printf("Resultado monolitico: %d\n\n",resultado);
    for (int i= 0; i<(T/4)+1; i++){
        pthread_barrier_destroy(&barrera[i]);
    }

    //SECUENCIAL

    free(vector);
    free(vectorSec);
    free(chequeo0);
    free(chequeo1);
    return 0;
}



/*

6. Paralelizar un algoritmo que determine si un vector de N elementos es
monotónico. Un vector es monotónico si todos sus elementos están ordenados
en forma creciente o decreciente.

    -->CADA EJECUCION CHEQUEA SI ESTA ORDENADO
    -->SI CADA PORCION DEL VECTOR ESTA ORDENADO, HAY QUE
       CHEQUEAR QUE TODOS ESTEN ORDENADOS DE IGUAL FORMA

    muy flashero?
*/