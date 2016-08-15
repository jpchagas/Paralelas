#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

#define  NUM_ROWS 100000
#define  VETOR_LEN 1000
#define SUICIDE 9
#define TAG_INDICE 0
#define TAG_VETOR 1
#define	TAG_TAREFA_CONCLUIDA 2
#define TAG_SUICIDIO 3

/*

REPORT DE BUGS:
-Programa estava mandando task do mestre para o escravo: CORRIGIDO;
-Programa não esta mandando daddos da matriz criada, acho que são valores de endereço memória: PENDENTE;


*/

void bs(int n, int * vetor)
{
    int c=0, d, troca, trocou =1;

    while (c < (n-1) & trocou )
    {
        trocou = 0;
        for (d = 0 ; d < n - c - 1; d++)
            if (vetor[d] > vetor[d+1])
            {
                troca      = vetor[d];
                vetor[d]   = vetor[d+1];
                vetor[d+1] = troca;
                trocou = 1;
            }
        c++;
    }
}

int cmpfunc (const void * a, const void * b){
	return ( *(int*)a - *(int*)b );
}

int main(int argc,char **argv){
   int  proc_n,           /* Número de processos proc_n */
        nescravos,          /* número de escravos */
        my_rank,          /* número do processo corrente, my_rank */
        dest,             /* destination task id to send message */
        indexm,
	indexe,           /* index into the array */
	indexr,
	tag,
        i,j,              /* loop variable */
        source;           /* origin task id of message */
	
    int **data;
    int result[VETOR_LEN];
    double t1,t2;

    MPI_Status status;


    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&proc_n);
    MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
    nescravos = proc_n-1;


/**************************** MESTRE ************************************/
	if (my_rank == 0){
		printf("MASTER: TENHO %d ESCRAVOS\n",nescravos);
		
		//Inicia a contagem de tempo.
		t1 = MPI_Wtime();
		
		//Gera a matriz
		int aux = NUM_ROWS * VETOR_LEN;
		data = (int **)malloc(NUM_ROWS * sizeof(int *));
		for(i = 0; i < NUM_ROWS; i++){
			data[i] =(int *) malloc(VETOR_LEN * sizeof(int));
			for(j = 0; j < VETOR_LEN; j++){
				data[i][j] = aux;
				aux--;
			}
		}

		/* Rajada inicial de tarefas */
		indexm = 0;
		for(dest=1; dest<= nescravos; dest++){
			MPI_Send(&indexm, 1, MPI_INT, dest, TAG_INDICE, MPI_COMM_WORLD);
			MPI_Send(data[indexm], VETOR_LEN, MPI_INT, dest, TAG_VETOR, MPI_COMM_WORLD);
			indexm = indexm + 1;
		}

		/* Aguarda retorno dos escravos e envia tarefa novamente para o que retornou.*/
		indexr = 0;
		while(nescravos>0){
			

			MPI_Recv(&indexr, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			
			if(status.MPI_TAG == TAG_INDICE){
				MPI_Recv(result, VETOR_LEN, MPI_INT, status.MPI_SOURCE, TAG_VETOR, MPI_COMM_WORLD, &status);
				printf("MESTRE: RECEBI A TAREFA %d DO ESCRAVO %d\n",indexr, status.MPI_SOURCE);
				memcpy(data[indexm],result,VETOR_LEN*sizeof(int));
			}
			
			
			
			/* Se o numero de tarefas ja se esgotou, termina o escravo.*/
			if(indexm>=NUM_ROWS-1){
				MPI_Send(&indexm, 1, MPI_INT,status.MPI_SOURCE, TAG_SUICIDIO, MPI_COMM_WORLD);
				nescravos--;
				printf("Matei um escravo \n");
			/* Se nao envia a proxima tarefa.*/
			}else {
				MPI_Send(&indexm,1,MPI_INT,status.MPI_SOURCE,TAG_INDICE,MPI_COMM_WORLD);//envia o indice do vetor para o escravo
				MPI_Send(data[indexm], VETOR_LEN, MPI_INT,status.MPI_SOURCE, TAG_VETOR, MPI_COMM_WORLD);//envia o vetor para o escravo
				indexm++;
				printf("Reenviei uma tarefa\n");
			}
		}
          
		printf("MASTER: TERMINEI! \n");
	  	t2 = MPI_Wtime();
	  	printf("\nTempo de execucao: %f\n\n", t2-t1);
		printf("\n");
      } 
      
   

/**************************** ESCRAVO ************************************/
	do{
		tag = status.MPI_TAG;
		source = 0;
		indexe = 0;
		MPI_Recv(&indexe, 1, MPI_INT, source, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		//Recebi o indice
		if(status.MPI_TAG == TAG_INDICE){
			MPI_Recv(result, VETOR_LEN, MPI_INT, source, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			printf("EU SOU O ESCRAVO %d E RECEBI A TAREFA %d\n",my_rank, indexe);
			
		}

		if(status.MPI_TAG == TAG_VETOR){ //Recebi o vetor para ordenar
			printf("RECEBI A TAG_VETOR E AGORA VOU ORDENAR\n");
			
			qsort(result, VETOR_LEN, sizeof(int), cmpfunc);
			
			//Envia parte ordenada para o master
			dest = 0;
			MPI_Send(&indexe, 1, MPI_INT, 0, TAG_INDICE, MPI_COMM_WORLD);
			MPI_Send(result, VETOR_LEN, MPI_INT, 0, TAG_VETOR, MPI_COMM_WORLD);
		}	       
	}while(tag!=TAG_SUICIDIO);
   MPI_Finalize();
}
