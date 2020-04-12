/*
 * Authors: Heredia Parillo, JuanPablo Andrew   &
 *          Rendon Zuniga, Luis Francisco
 * 
 * File:     main.c (sssp)
 * Purpose:  Implement "Scalable Single Source Shortest Path Algorithms 
 *           for Massively Parallel Systems"
 * Input:
 *      Number of nodes, delta parameter & source node for paths
 * Output:
 *      for now, the execution time
 *
 * Compile:  mpicc -Wall -o sssp main.c dyrray.c -I (makefile)
 * Run:
 *    mpiexec -n <p> sssp <nodes> <root> <delta> 
 *       - p: the number of processes
 *       - nodes: number of nodes
 *       - root: source for paths
 *       - delta: delta
 *
 */

#include <stdio.h>
#include "dyrray.h"
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>

#define MAX_WEIGHT 10
#define MAX_DEGREE 10
#define INFI 1000

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

void Get_args(int argc,char* argv[],int* global_cant_nodes,int* global_root_node,int* local_cant_nodes,int *delta,int my_rank,int p,MPI_Comm comm){

    
    if(argc != 4){
//            fflush(stderr);
        *global_cant_nodes = -1;
    }else{
        *global_cant_nodes = atoi(argv[1]);
        *global_root_node = atoi(argv[2]);
        *delta = atoi(argv[3]);
    }
    MPI_Bcast(global_cant_nodes,1,MPI_INT,0,comm);
    MPI_Bcast(global_root_node,1,MPI_INT,0,comm);
    MPI_Bcast(delta,1,MPI_INT,0,comm);
    if(*global_cant_nodes <= 0){
        MPI_Finalize();
        exit(-1);
    }
    *local_cant_nodes = *global_cant_nodes / p;
}  /* Get_args */

void updateBuks(int *new_buks, int *old_buks, int **Bucks, int **last_of_buck, int nodes){
    for(int i=0; i<nodes; ++i){
        if(new_buks[i]!=-1 && old_buks[i] != -1){
            push_element(Bucks[new_buks[i]], i,last_of_buck[new_buks[i]]);
            delete_element(Bucks[old_buks[i]], i,last_of_buck[old_buks[i]]);
        }
    }
}

int main(int argc, char** argv){
    int my_rank, comm_sz;
    srand(time(NULL));

    double start, finish, loc_elapsed, elapsed;
    MPI_Comm comm;

    MPI_Init(&argc, &argv);
    comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &comm_sz);
    MPI_Comm_rank(comm, &my_rank);

    int global_cant_nodes;      // Cantidad total de los nodos del grafo
    int local_cant_nodes;       // Cantidad de nodos para cada proceso
    int global_root;            // Nodo Raiz
    int delta;                  // Parametro delta
    int *local_nodes_u;         // Arreglo que contendrÃ¡ el identificador (indice) del nodo source (u)
    int *local_nodes_v;         // Arreglo que contendrÃ¡ el identificador (indice) del nodo destino (v)
    int *local_weights;         // Arreglo que contendrÃ¡ el peso o distancia entre el nodo u y v
    
    
    if(my_rank==0)
        Get_args(argc, argv, &global_cant_nodes, &global_root, &local_cant_nodes, &delta, my_rank, comm_sz, comm);
    
    /**Arreglos que contendran todas las aristas*/
    int *temp_global_nodes_u, *temp_global_nodes_v, *temp_global_weights;
    int *last_global_nodes_u, *last_global_nodes_v, *last_global_weights;

    int truly_cant_elements;        // Cantidad total de aristas
    int local_cant_edges;           // Cantidad de aristas asignadas por proceso

    if(my_rank == 0){

        temp_global_nodes_u = new_array(&last_global_nodes_u);
        temp_global_nodes_v = new_array(&last_global_nodes_v);
        temp_global_weights = new_array(&last_global_weights);
        int k,v,w;
        /**CreaciÃ³n del Grafo**/
        for(int i=0;i<global_cant_nodes;++i){
            k = 1 + (rand() % MAX_DEGREE);
            for (int j = 0; j < k; ++j){
                v = random() % global_cant_nodes;
                while(v == i)
                    v = rand() % global_cant_nodes;
                w = 1+(rand()%MAX_WEIGHT);
                push_element(temp_global_nodes_u, i, &last_global_nodes_u);
                push_element(temp_global_nodes_v, v, &last_global_nodes_v);
                push_element(temp_global_weights, w, &last_global_weights);
                
                
            }
        }
        /*****************************/

        truly_cant_elements = last_global_weights  - temp_global_weights;
        printf("%d \n",truly_cant_elements);
        local_cant_edges = truly_cant_elements / comm_sz;
        MPI_Bcast(&local_cant_edges,1,MPI_INT,0,comm);
        MPI_Scatter(temp_global_nodes_u,local_cant_edges,MPI_INT,
                    local_nodes_u,local_cant_edges,MPI_INT,0,comm);
        MPI_Scatter(temp_global_nodes_v,local_cant_edges,MPI_INT,
                    local_nodes_v,local_cant_edges,MPI_INT,0,comm);
        MPI_Scatter(temp_global_weights,local_cant_edges,MPI_INT,
                    local_weights,local_cant_edges,MPI_INT,0,comm);

        free(temp_global_nodes_u);  free(temp_global_nodes_v);  free(temp_global_weights);

    }else{
        MPI_Scatter(temp_global_nodes_u,local_cant_edges,MPI_INT,
                    local_nodes_u,local_cant_edges,MPI_INT,0,comm);
        MPI_Scatter(temp_global_nodes_v,local_cant_edges,MPI_INT,
                    local_nodes_v,local_cant_edges,MPI_INT,0,comm);
        MPI_Scatter(temp_global_weights,local_cant_edges,MPI_INT,
                    local_weights,local_cant_edges,MPI_INT,0,comm);
    }
    
    MPI_Finalize();
    return 0;
}


