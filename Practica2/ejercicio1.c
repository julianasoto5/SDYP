#include<stdio.h>
#include<stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#define ORDENXFILAS 0
#define ORDENXCOLUMNAS 1
/*------VARIABLES COMPARTIDAS-------*/
int N=100; //Dimension por defecto de las matrices
int T=2;
double *A,*B,*C;

//Retorna el valor de la matriz en la posicion fila y columna segun el orden que este ordenada
double getValor(double *matriz,int fila,int columna,int orden){
 if(orden==ORDENXFILAS){
  return(matriz[fila*N+columna]);
 }else{
  return(matriz[fila+columna*N]);
 }
}

//Establece el valor de la matriz en la posicion fila y columna segun el orden que este ordenada
void setValor(double *matriz,int fila,int columna,int orden,double valor){
 if(orden==ORDENXFILAS){
  matriz[fila*N+columna]=valor;
 }else{
  matriz[fila+columna*N]=valor;
 }
}

//Para calcular tiempo
double dwalltime(){
        double sec;
        struct timeval tv;

        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}

void* mult(void* arg){
    int id=*(int*)arg;
    printf("Hilo id:%d\n",id);
    int i,j,k;

    for (i=id*N/T; i<(id+1)*N/T;i++){ 
        for (j=0;j<N;j++){
            setValor(C,i,j,ORDENXFILAS,0);
            for (k=0; k < N; k++){
                 setValor(C,i,j,ORDENXFILAS,getValor(C,i,j,ORDENXFILAS)+getValor(A,i,k,ORDENXFILAS)*getValor(B,k,j,ORDENXCOLUMNAS));
            }    
        }
    }

    pthread_exit(NULL);
}
int main(int argc,char*argv[]){
 int i,j,k;
 int check=1;
 double timetick;
 
 //Controla los argumentos al programa
 if ((argc != 3) || ((N = atoi(argv[1])) <= 0) || ((T = atoi(argv[2])) <= 0))
  {
    printf("\nUsar: %s n\n  n: Dimension de la matriz (nxn X nxn)\n", argv[0]);
    exit(1);
  }

 //Aloca memoria para las matrices
  A=(double*)malloc(sizeof(double)*N*N);
  B=(double*)malloc(sizeof(double)*N*N);
  C=(double*)malloc(sizeof(double)*N*N);

 //Inicializa las matrices A y B en 1, el resultado sera una matriz con todos sus valores en N
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
	setValor(A,i,j,ORDENXFILAS,1);
	setValor(B,i,j,ORDENXCOLUMNAS,1);
   }
  }   

  timetick = dwalltime();
  int id;
  pthread_t hilos[T];
  int threads_ids[T];
  for (id=0;id<T;id++){
    threads_ids[id]=id;
     pthread_create(&hilos[id],NULL,&mult,(void*)&threads_ids[id]);
  }

  for (id=0;id<T;id++){
    pthread_join(hilos[id],NULL);
  }

 printf("Tiempo en segundos %f\n", dwalltime() - timetick);

 //Verifica el resultado
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
	check=check&&(getValor(C,i,j,ORDENXFILAS)==N);
   }
  }   

  if(check){
   printf("Multiplicacion de matrices resultado correcto\n");
  }else{
   printf("Multiplicacion de matrices resultado erroneo\n");
  }

 free(A);
 free(B);
 free(C);
 return(0);
}


/*
1. Paralelizar la multiplicación de matrices cuadradas de NxN. Obtener el tiempo
de ejecución para N=512, 1024 y 2048. Ejecutar con 2 y 4 threads.

    N=512
        T=2 --> 0.677012
        T=4 --> 0.459754
    N=1024
        T=2 --> 6.325109
        T=4 --> 3.998031
    N=2048
        T=2 --> 55.207674
        T=4 --> 53.834103
    N=4096
        T=2 --> 439.533411
        T=4 --> 404.371139
*/