#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

/*---------VARIABLES COMPARTIDAS---------*/
int T;
/*---------------------------------------*/


/*-----------------MUTEX-----------------*/

/*---------------------------------------*/


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
    //Código que ejecutará cada hilo
    pthread_exit(NULL);
}

int main(int argc, char*argv[]){
    T = atoi(argv[1]);
    pthread_t misThreads[T];
    int threads_ids[T];

    double timetick = dwalltime();
    for (int id=0;id<T;id++){
        threads_ids[id]=id;
        pthread_create(&misThreads[id],NULL,&function,(void*)&threads_ids[id]);
    }
    printf("Tiempo en segundos %f\n", dwalltime() - timetick);

    for (int id=0;id<T;id++){
        pthread_join(misThreads[id],NULL);
    }

    return 0;
}
