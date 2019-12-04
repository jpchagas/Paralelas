#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "mpi.h"

/*
O objetivo do trabalho é implementar, usando a biblioteca MPI, uma versão paralela seguindo o modelo divisão e conquista, de um programa
que ordena um grande vetor usando o algortimo Bubble Sort (o programa sequencial está no final desta especificação).
*/
//#define DEBUG 1            // comentar esta linha quando for medir tempo
#define ARRAY_SIZE 16      // trabalho final com o valores 10.000, 100.000, 1.000.000
#define VETOR 5
#define DELTA 10

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

int *interleaving(int* vetor, int tam)
{
	int *vetor_auxiliar;
	int i1, i2, i_aux;
	vetor_auxiliar = (int *)malloc(sizeof(int) * tam);

	i1 = 0;
	i2 = tam / 2;

	for (i_aux = 0; i_aux < tam; i_aux++) {
		if (((vetor[i1] <= vetor[i2]) && (i1 < (tam / 2)))
		    || (i2 == tam))
			vetor_auxiliar[i_aux] = vetor[i1++];
		else
			vetor_auxiliar[i_aux] = vetor[i2++];
	}

	return vetor_auxiliar;
}

int main(int argc,char **argv)
{


    int vetor[ARRAY_SIZE], vaux[ARRAY_SIZE], vaux2[ARRAY_SIZE];
    int i,
				tamanho,
				final,
				metade,
				intsize,
				proc_n,
				my_rank,
				pai;

	int *nv1, *nv2, *vetor_size,*vetor_auxiliar;
		
	for (i=0 ; i<ARRAY_SIZE; i++)            /* init array with worst case for sorting */
		vetor[i] = ARRAY_SIZE-i;

	#ifdef DEBUG
	printf("\nVetor: ");
	for (i=0 ; i<ARRAY_SIZE; i++)              /* print unsorted array */
    		printf("[%03d] ", vetor[i]);
	#endif
		
	//int delta = ceil(ARRAY_SIZE/((proc_n+1)/2));
	MPI_Status status;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&proc_n);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

		//printf("Número de Processos: %d \n",proc_n);
		/*SE NÚMERO DE PROCESSOS É 1 ENTÃO EXECUTA VERSÃO SEQUENCIAL*/
	if(proc_n<=1){
		bs(ARRAY_SIZE, vetor);                    /* sort array */

	     // int *vetor_auxiliar;         /* ponteiro para o vetor resultantes que sera alocado dentro da rotina */
    		 // vetor_aux = interleaving(vetor, tam);

    		#ifdef DEBUG
		    printf("\nVetor: ");
		    for (i=0 ; i<ARRAY_SIZE; i++)/* print sorted array */
			fprintf(stderr,"[%03d] ", vetor[i]);
    		#endif

    		return 0;

	}else{
		if((ARRAY_SIZE/proc_n-1)<DELTA){
			/*Processo atual é a raíz*/
			if(my_rank==0){
				printf("Sou a Raíz e meu Rank é: %d \n",my_rank);
				if((sizeof vetor/sizeof(int)) > DELTA){

					/*DIVIDE*/
					printf("Raíz Dividiu! \n");
					tamanho = sizeof(vetor);
					intsize = sizeof(int);
			
					//(sizeof(vetor)/sizeof(int))/2)-1
					metade = ((tamanho/intsize)/2);
				
					MPI_Send(&vetor[0], metade, MPI_INT, 2 * my_rank + 1, VETOR, MPI_COMM_WORLD);
					MPI_Send(&vetor[metade], metade, MPI_INT, 2 * my_rank + 2, VETOR, MPI_COMM_WORLD);

            	
					MPI_Recv(vetor, metade, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
					MPI_Recv(&vetor[metade], metade, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
					
					printf("\nVetor vindo dos filhos %d e %d: ", 2 * my_rank + 1, 2 * my_rank + 2);
	           			vetor_auxiliar = interleaving(vetor, *vetor_size);
					int k;
					for(k = 0; k < ARRAY_SIZE; k++) fprintf(stderr, "%d ", vetor[k]);

				}else{
					/*CONQUISTA*/
					printf("Raíz Conquistou!\n");
					//MPI_Recv(&vector, DELTA, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
					bs(DELTA, vetor);/* sort array */
				}
			}else{
				printf("Sou nodo e Meu Rank é: %d \n",my_rank);

				MPI_Recv(vaux, ARRAY_SIZE, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				MPI_Get_count(&status,MPI_INT,vetor_size);

			//	printf("rank: %d , v= %d, d=%d, if=%d",my_rank,*vetor_size,DELTA,*vetor_size>DELTA);

				pai = status.MPI_SOURCE;

				if(*vetor_size > DELTA){

					printf("Sou nodo e Meu Rank é: %d , estou dividindo\n",my_rank);
					/*DIVIDE*/					
					//(sizeof(vetor)/sizeof(int))/2)-1

					metade = (*vetor_size/2);

					MPI_Send(vaux, metade, MPI_INT, 2*my_rank+1, VETOR, MPI_COMM_WORLD);
					MPI_Send(&vaux[metade], metade, MPI_INT, 2*my_rank+2, VETOR, MPI_COMM_WORLD);

					MPI_Recv(vaux, metade, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
					MPI_Recv(&vaux[metade], metade, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				//	printf("\nVetor vindo dos filhos %d e %d: ",2*my_rank+1,2*my_rank+2);

		    		 	vetor_auxiliar = interleaving(vaux, *vetor_size);
					//I_Send(vaux, *vetor_size, MPI_INT, pai, VETOR, MPI_COMM_WORLD);
				}else{
					printf("Sou nodo e Meu Rank é: %d , estou conquistando\n",my_rank);

					/*CONQUISTA*/
					printf("Sou nodo e Meu Rank é: %d , meu pai é %d\n", my_rank, pai);

					bs(*vetor_size, vaux);/* sort array */
					MPI_Send(vaux, *vetor_size, MPI_INT, pai, VETOR, MPI_COMM_WORLD);
				
				}
			}
		}
	}
	MPI_Finalize();

}