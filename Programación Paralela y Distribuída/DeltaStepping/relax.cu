#ifndef _RELAX_H_
#define _RELAX_H_
#include "vertex.cu"
__global__ 
void get_result(graph::vertex* gpu_global_vertex,int i){
	printf("result: %d\n",gpu_global_vertex[i].dist);	
}

__global__ 
void relax_all(int* gpu_vertex_buf, graph::vertex* gpu_global_vertex, graph::edge* gpu_global_edge, graph::gpuResult* cpu_result, graph::gpuResult* gpu_used_result_buf, int* TEPS_GPU){
    const unsigned int bid = blockIdx.x; 
    const unsigned int num_block = gridDim.x; 
    const unsigned int tid_in_block = threadIdx.x;
    const unsigned int num_thread = blockDim.x;
    const unsigned int tid_in_grid = blockDim.x * blockIdx.x +threadIdx.x;
    //printf("thread id: %d, thread teps: %d\n",tid_in_grid,TEPS_GPU[tid_in_grid]);

    int i=0,j=0;
    for (i=bid;i<MAX_BUCKET_SIZE;i+=num_block){
        graph::vertex *temp_v = &gpu_global_vertex[gpu_vertex_buf[i]];
        int num_edges = gpu_global_vertex[gpu_vertex_buf[i]+1].edge_index - temp_v->edge_index;
        int tent_current = temp_v->dist;
        if(gpu_vertex_buf[i] == 0)
            return;   
        for(j=tid_in_block;j<MAX_RESULT_SIZE;j+=num_thread){
            int dist_current = 0;
            int dest = 0;
            int tent_dest = 0;
            int flag = 0;
            if(j < num_edges){
                TEPS_GPU[tid_in_grid]++;
                dist_current = gpu_global_edge[temp_v->edge_index+j].distance;
                dest = gpu_global_edge[temp_v->edge_index+j].des_v;
                tent_dest = gpu_global_vertex[dest].dist;
            }
            if(tent_current + dist_current < tent_dest){
                gpu_global_vertex[dest].dist = tent_current + dist_current;
                gpu_global_vertex[dest].pre_vertex = i;
                flag =1;
            }

            gpu_used_result_buf[j+32*bid].index = dest*flag;
                    gpu_used_result_buf[j+32*bid].old_distance = tent_dest*flag;
                    gpu_used_result_buf[j+32*bid].new_distance = (tent_current+dist_current)*flag;
            if(dest*flag==1275){
                printf("@@@%d %d\n",j+32*bid,gpu_used_result_buf[j+32*bid].index);
                gpu_used_result_buf[j+32*bid].index = dest*flag;
            }
        }
    }
}

__global__
void bellmanFordKuda(graph::vertex* gpu_vertex, graph::edge* gpu_edge, int edges_size, int vertex_size, int* distances, bool *change, int* TEPS){
    *change=false;
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    if(i<vertex_size){
        
        int source=0;
        for(int j=0; j<edges_size; ++j){
            if(source<vertex_size-1 && j==gpu_vertex[source+1].edge_index)
                ++source;
                graph::edge edge = gpu_edge[j];
            if(edge.des_v==i){
                TEPS[i]++;
                int aux=distances[source]+edge.distance;
                if(aux<gpu_vertex[i].dist){
                    gpu_vertex[i].dist=aux;
                    gpu_vertex[i].pre_vertex=source;
                    *change=true;
                }
            }
        }
        for(int i=0; i<vertex_size; ++i)
            distances[i]=gpu_vertex[i].dist;
    }
}

#endif
