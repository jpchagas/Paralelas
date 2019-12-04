/**
* @file
* @author
* @date
* @brief
*
* Detail
 */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define DELTA 500000
#define VECTOR_SIZE 1000000

/**
* @brief
*
* Detail
*/

int main (int argc, char* argv[]){
  
  /*
  printf("Parameter: %d \n",argc);
  for(size_t i = 0; i < 4; i++)
  {
    printf("Parameter: %s \n",argv[i]);
  }
  */

  int vector[VECTOR_SIZE], cvector[VECTOR_SIZE];
  int rank, 
      nproc, 
      tam_vetor,
      metade;
  double t1,t2;

MPI_Init (&argc, &argv);      /* starts MPI */
t1 = MPI_Wtime();  // inicia a contagem do tempo
MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
MPI_Comm_size (MPI_COMM_WORLD, &nproc);        /* get number of processes */
MPI_Status status;

if(nproc==1){
  inicializa(vector,VECTOR_SIZE);
  bubble_sort(vector,VECTOR_SIZE);
  printVector(vector,VECTOR_SIZE);
}else{
  /*****************************************************************************************************
BEGIN PSEUDO-CODE
******************************************************************************************************/  


//RECEBO O VETOR
if (rank != 0 )
  {
    //printf("My Rank != 0\n");
    MPI_Recv(cvector, VECTOR_SIZE, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status,MPI_INT,&tam_vetor);
    //printVector(cvector,tam_vetor);
    //MPI_Recv ( vetor, pai);                       // não sou a raiz, tenho pai
    //MPI_Get_count(&Status, MPI_IN\T, &tam_vetor);  // descubro tamanho da mensagem recebida
    
  }
else{
    //printf("My Rank == 0\n");
    tam_vetor = VECTOR_SIZE;               // defino tamanho inicial do vetor
    inicializa (vector, tam_vetor);      // sou a raiz e portanto gero o vetor - ordem reversa
    
  }

//TESTE PARA VERIFICAR SE DIVIDE OU CONQUISTA: SE TAMANHO DO VETOR FOI MAIOR QUE O DELTA DIVIDE SE NÃO CONQUISTA
if (tam_vetor <= DELTA)
{
  //printf("Conquisto! - Executa BubbleSort\n");
  bubble_sort(cvector,tam_vetor);  // conquisto
  //printVector(cvector,tam_vetor);
}
  
else{
  //printf("Divido! - Divido o vetor em partes iguais\n");
  

//CALCULA QUAL TAMANHO DA METADE VETOR
metade = tam_vetor/2;

// QUEBRA EM DUAS METADES O VETOR E MANDA PARA O FILHOS
//MPI_Send (&vector[0], filho esquerda, tam_vetor/2 );  // mando metade inicial do vetor
//MPI_Send (&vetor[tam_vetor/2], filho direita , tam_vetor/2 );  // mando metade final 
MPI_Send(&vector[0], metade, MPI_INT, 2 * rank + 1, 0, MPI_COMM_WORLD);
MPI_Send(&vector[metade], metade, MPI_INT, 2 * rank + 2, 0, MPI_COMM_WORLD);

// RECEBE O VETOR RESULTADO DO FILHO

//    MPI_Recv ( &vetor[0], filho esquerda);             
//    MPI_Recv ( &vetor[tam_vetor/2], filho direita); 
MPI_Recv(vector, metade, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
MPI_Recv(&vector[metade], metade, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
//printVector(vector,tam_vetor);


// INTERCALA O VETOR INTEIRO PARA COLOCAR EM ORDEM
    intercala(vector,tam_vetor);
}

// mando para o pai

if (rank !=0)
{
  //printf("Conquistado! - Enviando para o pai a resposta\n");
//   MPI_Send ( vetor, pai, tam_vetor );  // tenho pai, retorno vetor ordenado pra ele
  MPI_Send(cvector, tam_vetor, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
}
else
{
  //printf("Sou pai, vetor resultado:\n");
  //printVector(vector, tam_vetor);                    // sou o raiz, mostro vetor
}
  
/*************************************************************************************************
END PSEUDO-CODE
*************************************************************************************************/  


}
  t2 = MPI_Wtime(); // termina a contagem do tempo
  printf("\nTempo de execucao: %f\n\n", t2-t1); 
  //printf( "Delta value %d\n", DELTA);
  MPI_Finalize();
  return 0;
}

/**
* @brief Function to fill the initial vector
* 
* This function is developed to fill the initial vector with integer numbers.
* The value that fill the cell is the vector size less the position of cell
* E.g. VECTOR_SIZE = 10 and CELL_POSITION = 4, so 10 - 4 = 6 
*
* @code
*   inicializa(vector={},10);
* @endcode
* @param
* @return
* @see
* @note
* @warning
*/
void inicializa(int vector[], int size)
{
  for(size_t i = 0; i <= size; i++)
  {
    vector[i] = size - i;
  } 
}

/**
* @brief Function to print on screen each vector value
* 
* This function is developoed to iterate over a vector and print valeu of each position.
*
* @code
*   printVector(vector={12345},5);
* @endcode
* @param
* @return
* @see
* @note
* @warning
*/
void printVector(int vector[],int size)
{
  printf("Vector numbers:\n");
  for(size_t i = 0; i < size; i++)
  {
    printf( "| %d |", vector[i]);
  }
  printf( "\n");
}

/**
* @brief Function to print on screen each vector value
* 
* This function is developoed to iterate over a vector and print valeu of each position.
*
* @code
*   bubble_sort(vector={54321},5);
* @endcode
* @param
* @return
* @see
* @note
* @warning
*/

void bubble_sort (int vetor[], int n) {
    int aux;

    for (size_t i = 0; i < n - 1; i++) {
        //printf("\n[%d] ", k);

        for (size_t j = 0; j < n - i - 1; j++) {
          //  printf("%d, ", j);

            if (vetor[j] > vetor[j + 1]) {
                aux          = vetor[j];
                vetor[j]     = vetor[j + 1];
                vetor[j + 1] = aux;
            }
        }
    }
}

/**
* @brief Function to print on screen each vector value
* 
* This function is developoed to iterate over a vector and interleaving
*
* @code
*   intercala(vector={67891012345},10);
* @endcode
* @param
* @return
* @see
* @note
* @warning
*/

void intercala(int vector[],int size){
  int auxvector[size];
  int i, j;
  i = 0;
	j = size / 2;

	for (size_t n = 0; n < size; n++) {
		if (((vector[i] <= vector[j]) && (i < (size / 2)))
		    || (j == size))
			auxvector[n] = vector[i++];
		else
			auxvector[n] = vector[j++];
	}
  vector = auxvector;
}