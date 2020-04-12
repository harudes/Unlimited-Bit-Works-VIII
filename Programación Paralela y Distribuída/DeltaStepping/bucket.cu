
#include "cuda.h"
#include "relax.cu"
#include <iostream>
#include <time.h>

using namespace std;

#define Delta 10

char* dir="graph.txt";

int cal_shortest_path();
void bellman_ford(graph &Graph, int &total_Teps);
int deltaHybrid();

int main(void){
    clock_t start = clock();
    int TEPS = deltaHybrid();
    cout<<"TEPS: "<<TEPS<<endl;
    
    /*graph Graph(dir);
    bellman_ford(Graph);*/
    int time = ((double)clock()-start/CLOCKS_PER_SEC);
    cout<<"Tiempo transcurrido: "<<time<<endl;
    cout<<"GTEPS: "<<(double)TEPS/(double)time<<endl;
}

void printResult(graph &graph_instance){
    for(int i=0; i<graph_instance.vertex_size; ++i){
        cout<<i<<" "<<graph_instance.global_vertex[i].pre_vertex<<" "<<graph_instance.global_vertex[i].dist<<endl;
    }
}

int cal_shortest_path(){
    int num_block = 8;
    int num_threads = 32;
    dim3 dg(num_block, 1, 1);
    dim3 db(num_threads, 1, 1);
    graph graph_instance(dir,Delta);
    int* TEPS_GPU,*TEPS=new int[num_block*num_threads];
    for(int i=0; i<num_threads*num_block; ++i)
        TEPS[i]=0;

    cudaSetDevice(0);

    cudaMalloc((void **)&TEPS_GPU,num_block*num_threads*sizeof(int));
    cudaMemcpy(TEPS_GPU,TEPS,num_block*num_threads*sizeof(int),cudaMemcpyHostToDevice);


    cudaMalloc((void **)&graph_instance.gpu_vertex,(graph_instance.vertex_size+2)*sizeof(graph::vertex));
    cudaMalloc((void**)&graph_instance.gpu_edge,graph_instance.edges_size*sizeof(graph::edge));

    cudaMalloc((void**)&graph_instance.gpu_used_result_buf,MAX_RESULT_SIZE * sizeof(graph::gpuResult));  

    cudaMemcpy(graph_instance.gpu_vertex,graph_instance.global_vertex,(graph_instance.vertex_size+2)*sizeof(graph::vertex),cudaMemcpyHostToDevice);
    cudaMemcpy(graph_instance.gpu_edge,graph_instance.global_edge,graph_instance.edges_size*sizeof(graph::edge), cudaMemcpyHostToDevice);

    cudaMemcpy(graph_instance.gpu_used_result_buf,graph_instance.gpu_result_buf,sizeof(graph::gpuResult) * MAX_RESULT_SIZE, cudaMemcpyHostToDevice);

    cudaMalloc((void**)&graph_instance.vertex_buf_ptr, MAX_BUCKET_SIZE);


    int min,result_count=0;
    int* temp_vertex_array;
    temp_vertex_array = (int*)malloc(sizeof(int) * MAX_BUCKET_SIZE);
    int timeInBucket=0;
    int cantidad=0;
    while(!graph_instance.is_all_bucket_empty()){
        cantidad++;
        min = graph_instance.min_no_empty_bucket(); 
        int count = graph_instance.bucket_set_to_array(min, temp_vertex_array);

        for(int i=count; i< MAX_BUCKET_SIZE; i++)
                temp_vertex_array[i] = 0;
	
        for(int i = 0; i < MAX_BUCKET_SIZE; i++){
            if(temp_vertex_array[i] != 0){
                graph_instance.bucket_array[min].erase(temp_vertex_array[i]);
            }
        }
		
        cudaMemcpy(graph_instance.vertex_buf_ptr,temp_vertex_array, MAX_BUCKET_SIZE,cudaMemcpyHostToDevice);
       relax_all<<<num_block,num_threads>>>(graph_instance.vertex_buf_ptr,
               graph_instance.gpu_vertex,graph_instance.gpu_edge,graph_instance.gpu_result_buf,
               graph_instance.gpu_used_result_buf,TEPS_GPU);
       cudaMemcpy(graph_instance.gpu_result_buf,graph_instance.gpu_used_result_buf,
                   sizeof(graph::gpuResult)*num_threads*num_block, cudaMemcpyDeviceToHost); 

       result_count = 0;
       clock_t start = clock();
       while(1){
            if(result_count >= MAX_BUCKET_SIZE){
                break;
            }
            if(graph_instance.gpu_result_buf[result_count].index == 0){
                result_count++;
                continue;
             }

             
            int old_index = graph_instance.gpu_result_buf[result_count].old_distance / graph_instance.delta;
            int new_index = graph_instance.gpu_result_buf[result_count].new_distance / graph_instance.delta;
            if(graph_instance.gpu_result_buf[result_count].old_distance != MAX_DISTANCE){
                graph_instance.bucket_array[old_index].erase(graph_instance.gpu_result_buf[result_count].index);
            }
                   
            graph_instance.bucket_array[new_index].insert(graph_instance.gpu_result_buf[result_count].index);
            result_count++;
        }
        timeInBucket += ((double)clock()-start/CLOCKS_PER_SEC);
    }
    
    cout<<"Tiempo en bucket: "<<timeInBucket<<endl;
    cout<<"Numero de buckets: "<<cantidad<<endl;
    cudaMemcpy(graph_instance.global_vertex,graph_instance.gpu_vertex,(graph_instance.vertex_size+2)*sizeof(graph::vertex),cudaMemcpyDeviceToHost);
    
    printf("over\n");
    
    cudaMemcpy(TEPS,TEPS_GPU,num_block*num_threads*sizeof(int),cudaMemcpyDeviceToHost);
    cudaFree(graph_instance.gpu_vertex);
    cudaFree(graph_instance.gpu_edge);
    cudaFree(graph_instance.gpu_used_result_buf);  
    cudaFree(TEPS_GPU);
    free(temp_vertex_array);
    int total_Teps=0;
    for(int i=0; i<num_block*num_threads; ++i){
        total_Teps+=TEPS[i];
    }
    return total_Teps;
}

int deltaHybrid(){
    int num_block = 8;
    int num_threads = 32;
    dim3 dg(num_block, 1, 1);
    dim3 db(num_threads, 1, 1);
    graph graph_instance(dir,Delta);
    int* TEPS_GPU,*TEPS=new int[num_block*num_threads];
    for(int i=0; i<num_threads*num_block; ++i)
        TEPS[i]=0;

    cudaSetDevice(0);

    cudaMalloc((void **)&TEPS_GPU,num_block*num_threads*sizeof(int));
    cudaMemcpy(TEPS_GPU,TEPS,num_block*num_threads*sizeof(int),cudaMemcpyHostToDevice);

     //copy to GPU
    cudaMalloc((void **)&graph_instance.gpu_vertex,(graph_instance.vertex_size+2)*sizeof(graph::vertex));
    cudaMalloc((void**)&graph_instance.gpu_edge,graph_instance.edges_size*sizeof(graph::edge));

    //malloc danteng!!
    cudaMalloc((void**)&graph_instance.gpu_used_result_buf,MAX_RESULT_SIZE * sizeof(graph::gpuResult));  
    //copy  
    cudaMemcpy(graph_instance.gpu_vertex,graph_instance.global_vertex,(graph_instance.vertex_size+2)*sizeof(graph::vertex),cudaMemcpyHostToDevice);
    cudaMemcpy(graph_instance.gpu_edge,graph_instance.global_edge,graph_instance.edges_size*sizeof(graph::edge), cudaMemcpyHostToDevice);

    cudaMemcpy(graph_instance.gpu_used_result_buf,graph_instance.gpu_result_buf,sizeof(graph::gpuResult) * MAX_RESULT_SIZE, cudaMemcpyHostToDevice);
    //malloc vertex buffer
    cudaMalloc((void**)&graph_instance.vertex_buf_ptr, MAX_BUCKET_SIZE);
    //malloc result buffer

    int min=0,result_count=0;
    int* temp_vertex_array;
    temp_vertex_array = (int*)malloc(sizeof(int) * MAX_BUCKET_SIZE);
    int cantidad=0;
    while(!graph_instance.is_all_bucket_empty() && (float) graph_instance.getFinishedNodes(min)/(float) graph_instance.vertex_size <0.4){
        cantidad++;
        min = graph_instance.min_no_empty_bucket(); 
        int count = graph_instance.bucket_set_to_array(min, temp_vertex_array);

        for(int i=count; i< MAX_BUCKET_SIZE; i++)
                temp_vertex_array[i] = 0;
	
        for(int i = 0; i < MAX_BUCKET_SIZE; i++){
            if(temp_vertex_array[i] != 0){
                graph_instance.bucket_array[min].erase(temp_vertex_array[i]);
            }
        }
		
        cudaMemcpy(graph_instance.vertex_buf_ptr,temp_vertex_array, MAX_BUCKET_SIZE,cudaMemcpyHostToDevice);

       relax_all<<<num_block,num_threads>>>(graph_instance.vertex_buf_ptr,
               graph_instance.gpu_vertex,graph_instance.gpu_edge,graph_instance.gpu_result_buf,
               graph_instance.gpu_used_result_buf,TEPS_GPU);
       cudaMemcpy(graph_instance.gpu_result_buf,graph_instance.gpu_used_result_buf,
                   sizeof(graph::gpuResult)*num_threads*num_block, cudaMemcpyDeviceToHost); 

       result_count = 0;
       while(1){
            if(result_count >= MAX_BUCKET_SIZE){
                break;
            }
            if(graph_instance.gpu_result_buf[result_count].index == 0){
                result_count++;
                continue;
             }

             
            int old_index = graph_instance.gpu_result_buf[result_count].old_distance / graph_instance.delta;
            int new_index = graph_instance.gpu_result_buf[result_count].new_distance / graph_instance.delta;
            if(graph_instance.gpu_result_buf[result_count].old_distance != MAX_DISTANCE){
                graph_instance.bucket_array[old_index].erase(graph_instance.gpu_result_buf[result_count].index);
            }
                   
            graph_instance.bucket_array[new_index].insert(graph_instance.gpu_result_buf[result_count].index);
            result_count++;
        }
       
    }
    cout<<"Cantidad de buckets: "<<cantidad<<endl;
    get_result<<<1,1>>>(graph_instance.gpu_vertex,2);
    cudaMemcpy(graph_instance.global_vertex,graph_instance.gpu_vertex,(graph_instance.vertex_size+2)*sizeof(graph::vertex),cudaMemcpyDeviceToHost);
    
    printf("over\n");
    
    cudaMemcpy(TEPS,TEPS_GPU,num_block*num_threads*sizeof(int),cudaMemcpyDeviceToHost);
    cudaFree(graph_instance.gpu_vertex);
    cudaFree(graph_instance.gpu_edge);
    cudaFree(graph_instance.gpu_used_result_buf);  
    cudaFree(TEPS_GPU);
    free(temp_vertex_array);
    int total_Teps=0;
    for(int i=0; i<num_block*num_threads; ++i){
        total_Teps+=TEPS[i];
    }
    bellman_ford(graph_instance, total_Teps);
    return total_Teps;
}

void bellman_ford(graph &Graph, int &total_Teps){

    int *GPU_Distances;
    int totalThreads = 1024 * ceil(Graph.vertex_size/1024.0);
    cudaMalloc((void **)&Graph.gpu_vertex,Graph.vertex_size*sizeof(graph::vertex));
    cudaMalloc((void **)&Graph.gpu_edge,Graph.edges_size*sizeof(graph::edge));
    cudaMalloc((void **)&GPU_Distances,Graph.vertex_size*sizeof(int));
    cudaMemcpy(Graph.gpu_vertex,Graph.global_vertex,Graph.vertex_size*sizeof(graph::vertex),cudaMemcpyHostToDevice);
    cudaMemcpy(Graph.gpu_edge,Graph.global_edge,Graph.edges_size*sizeof(graph::edge),cudaMemcpyHostToDevice);
    int *distances = new int[Graph.vertex_size];
    int *TEPS = new int[totalThreads], *GPU_TEPS;
    for(int i=0; i<totalThreads; ++i)
        TEPS[i]=0;
    cudaMalloc((void **)&GPU_TEPS, sizeof(int) * totalThreads);
    cudaMemcpy(GPU_TEPS,TEPS,sizeof(int) * totalThreads, cudaMemcpyHostToDevice);
    for(int i=0; i<Graph.vertex_size; ++i){
        distances[i]=Graph.global_vertex[i].dist;
    }
    cudaMemcpy(GPU_Distances,distances,Graph.vertex_size*sizeof(int),cudaMemcpyHostToDevice);
    bool *change = new bool, *GPU_change;
    *change=true;
    cudaMalloc((void **) &GPU_change, sizeof(bool));
    while(*change){
        *change=false;
        bellmanFordKuda<<<dim3(ceil(Graph.vertex_size/1024.0),1,1),dim3(1024,1,1)>>>(Graph.gpu_vertex,Graph.gpu_edge,Graph.edges_size,Graph.vertex_size,GPU_Distances,GPU_change,GPU_TEPS);
        cudaMemcpy(change,GPU_change,sizeof(bool),cudaMemcpyDeviceToHost);
    }
    cudaMemcpy(Graph.global_vertex,Graph.gpu_vertex,Graph.vertex_size*sizeof(graph::vertex),cudaMemcpyDeviceToHost);
    //printResult(Graph);
    cudaMemcpy(TEPS,GPU_TEPS,sizeof(int) * totalThreads, cudaMemcpyDeviceToHost);
    for(int i=0; i<totalThreads; ++i)
        total_Teps+=TEPS[i];
    cudaFree(Graph.gpu_edge);
    cudaFree(Graph.gpu_vertex);
    cudaFree(GPU_Distances);
    cudaFree(GPU_change);
    cudaFree(GPU_TEPS);
    free(distances);
}