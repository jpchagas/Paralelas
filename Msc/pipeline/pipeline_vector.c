#include <stdio.h>
#include "mpi.h"
 
main(int argc, char** argv)
{
int my_rank;  /* Identificador do processo */
int proc_n;   /* Número de processos */
int tag = 50; /* Tag para as mensagens */
int vector[10] = {1,2,3,4,5,6,7,8,9,10};
int message; /* Buffer para as mensagens */
int i;
MPI_Status status; /* Status de retorno */
 
MPI_Init (&argc , & argv);
 
MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
MPI_Comm_size(MPI_COMM_WORLD, &proc_n); 

	for(i=0;i<10;i++){
		/*Sou o primeiro e apenas crio a mensagem*/
		if (my_rank == 0) {
   			message = vector[i];
   		}
		/*Sou o próximo recebo do anterior e processo a mensagem*/
		else {
      			MPI_Recv (&message, 1, MPI_INT , my_rank-1, tag, MPI_COMM_WORLD, &status);
   		}
		message = message + 1;
		/*Sou o último pego o valor final e exibo na tela*/
		if(my_rank==proc_n-1){
   			printf("Mensagem: %d", message);
		/*Não sou o último envio para minha direita*/
		}else{
        		MPI_Send (&message, 1, MPI_INT,my_rank+1, tag, MPI_COMM_WORLD);
		}
	}
MPI_Finalize();

}
