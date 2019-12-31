#include <stdlib.h>
#include <stdio.h>
#include <math.h> 
#include"mpi.h"
#include <omp.h>

int * allocateTablo(int size) {
    int * tmp;
    tmp = malloc(size*sizeof(int));
    return tmp;
}

void printArray(int * tmp,int size) {
    int i;
    for (i = 0; i < size; ++i) {
        printf("%i ", tmp[i]);
    }
    printf("\n");
}

int getvectorlength(char *argv[]){
    FILE * fp;
    int len=0;
    int x=0;
    fp = fopen(argv[2], "r");
    while(fscanf(fp,"%d",&x)>0){len+=1;}
    return len;
}

int getmatrixlength(char *argv[]){
    FILE * fp;
    int len=0;
    int x=0;
    fp = fopen(argv[2], "r");
    while(fscanf(fp,"%d",&x)>0){len+=1;}
    return len*len;
}

int * readmatrix(char *argv[]){
    FILE * fp;
    int matrix_len=0;
    int x=0;
    int j=0;
    int * matrix;
    matrix_len = getmatrixlength(argv);
    matrix = allocateTablo(matrix_len);
    fp = fopen(argv[1], "r");
    while(fscanf(fp,"%d",&x)>0)
    {
        matrix[j]=x;
        j+=1;
    }
    fclose(fp);
    return matrix;
}

int * readvector(char *argv[]){
    FILE * fp;
    int vector_len=0;
    int x=0;
    int m=0;
    int * vector;
    vector_len = getvectorlength(argv);
    vector = allocateTablo(vector_len);
    fp = fopen(argv[2], "r");
    while(fscanf(fp,"%d",&x)>0)
    {
        vector[m]=x;
        m+=1;
    }
    fclose(fp);
    return vector;
}

void calculate(int * mat, int *vector, int *result, int size, int ProcessId){
    // omp_set_num_threads(2);
    #pragma omp parallel for
    for(int i=ProcessId*(size/4); i<(ProcessId+1)*(size/4);i++){
        result[i]=0;
        for(int j=i*size; j<(i+1)*size; j++){
            result[i] += mat[j] * vector [j-i*size];
        }
    }
}

void calculate2(int * mat, int *vector, int *result, int size, int ProcessId){
    // omp_set_num_threads(2);
    #pragma omp parallel for
    for(int i=ProcessId*(size/4); i<size;i++){
        result[i]=0;
        for(int j=i*size; j<(i+1)*size; j++){
            result[i] += mat[j] * vector [j-i*size];
        }
    }
}

int main(int argc, char *argv[]){
  //---------------------------------------------------------initialize MPI
    int rt = MPI_Init(&argc, &argv);  
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  //---------------------------------------------------------declare matrix and vector
    int * vector;
    int * matrix;
    int * result;
    int vector_len=getvectorlength(argv);
    int matrix_len=getmatrixlength(argv);
  //---------------------------------------------------------send matrix and vector
    if (world_rank != 0) {
        vector = malloc(sizeof(int) * vector_len);
        matrix = malloc(sizeof(int) * matrix_len);
        result = malloc(sizeof(int) * vector_len);
        MPI_Recv(vector, vector_len, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(matrix, matrix_len, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else {
        result = malloc(sizeof(int) * vector_len);
        vector = readvector(argv);
        matrix = readmatrix(argv);
    }
    MPI_Send(vector, vector_len, MPI_INT, (world_rank + 1) % world_size, 0, MPI_COMM_WORLD);
    MPI_Send(matrix, matrix_len, MPI_INT, (world_rank + 1) % world_size, 0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        MPI_Recv(vector, vector_len, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(matrix, matrix_len, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
  //---------------------------------------------------------calculate
    if(world_rank != 0){
        MPI_Recv(result, vector_len, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if(world_rank!= world_size-1){
        	calculate(matrix, vector, result, vector_len, world_rank);
    	}else{
    		calculate2(matrix, vector, result, vector_len, world_rank);
    	}
    }else{
        calculate(matrix, vector, result, vector_len, world_rank);
    }
    MPI_Send(result, vector_len, MPI_INT, (world_rank + 1) % world_size, 0, MPI_COMM_WORLD);
    if(world_rank == 0){
        MPI_Recv(result, vector_len, MPI_INT, world_size - 1, 0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for(int i=0;i<vector_len;i++){
            printf("%d\n", result[i]);
        }
    }
  //---------------------------------------------------------finish
    MPI_Finalize();  

    return 0;  
} 
