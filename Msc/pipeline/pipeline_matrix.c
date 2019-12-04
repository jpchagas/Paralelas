#include <stdio.h>
#include "mpi.h"
 
main(int argc, char** argv)
{
int my_rank;  /* Identificador do processo */
int proc_n;   /* Número de processos */
int tag = 50; /* Tag para as mensagens */
int matrix [3][3] = {{1,2,3},{4,5,6},{7,8,9}};
int message[3]; /* Buffer para as mensagens */
int i = 0;
int j = 0;
MPI_Status status; /* Status de retorno */
 
MPI_Init (&argc , & argv);
 
MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
MPI_Comm_size(MPI_COMM_WORLD, &proc_n); 

while (i < 3){
		/*Sou o primeiro e apenas crio a mensagem*/
		if (my_rank == 0) {
			for(j = 0; j< 3 ; j++){
				message[j] = matrix[i][j];
			}
		   }
		/*Sou o próximo recebo do anterior e processo a mensagem*/
		else {
		      MPI_Recv (&message[0], 3, MPI_INT , my_rank-1, tag, MPI_COMM_WORLD, &status);
		   }
		   for(j = 0; j < 3; j++){
		   	message[j] = message[j] + 1;
		   }
		/*Sou o último pego o valor final e exibo na tela*/
		if(my_rank==proc_n-1){
		   	printf("Mensagem: %d", message);
		/*Não sou o último envio para minha direita*/
		}else{

			MPI_Send (&message[0], 3, MPI_INT,my_rank+1, tag, MPI_COMM_WORLD);
		}
}
MPI_Finalize();
}
