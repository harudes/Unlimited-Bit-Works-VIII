#include "vertex.h"

graph::graph(char* filepath, int D){
    init_memory(filepath);
    delta = D;
    gpu_result_buf = (graph::gpuResult*)malloc(MAX_RESULT_SIZE * sizeof(graph::gpuResult));
    printf("end of graph init\n");
    src = 1;
    global_vertex[src].dist = 0;
    dest = 4;
    init_all_bucket();
}

graph::~graph(){
    free(global_vertex);
    free(global_edge);
    free(gpu_result_buf);
}

int graph::init_graph(){
    int i;
   
    global_vertex =(struct vertex*) malloc((vertex_size+2)*sizeof(struct vertex));
    global_edge = (struct edge*)malloc(edges_size*sizeof(struct edge));
  
   
    for(i=0;i<vertex_size+2;i++){
      global_vertex[i].edge_index =0;
      global_vertex[i].dist = MAX_DISTANCE;
      global_vertex[i].pre_vertex = -1;
    }
  
    graph_init=1;
    return 0;
  }

int graph::init_memory(char* filepath){
    char string[256];
  
    FILE* fp = fopen(filepath,"r");
    if(fp==NULL)
      return -1;
  
    
    while(fgets(string,256,fp)!=NULL){
      static char sign;
    
      static int src,dest,dist,cur_v=0,cur_edge=0;
    
      sscanf(string,"%c",&sign);
  
    
      if(sign=='a'){
          if(!graph_init)
              return -2;
          if(cur_edge>edges_size)
              return -4;
        sscanf(string,"%c\t%d\t%d\t%d",&sign,&src,&dest,&dist);
    
    
        global_edge[cur_edge].des_v=dest;
        global_edge[cur_edge].distance=dist;
        cur_edge++;
  
  
        if(cur_v!=src){
    
          if(cur_v==src-1){
    
              global_vertex[src].edge_index=cur_edge-1;	
              cur_v=src;
          }
          else
              return -3;
        }
      }
      //the line describe the size of graph
      else if(sign=='p'){ 
        sscanf(string,"%c\t%d\t%d",&sign,&src,&dest);
        vertex_size = src;
        edges_size = dest;
        printf("GOT the size of graph, vertex:%d edge:%d\n",vertex_size,edges_size);
        init_graph();
      }
    }
  
    fclose(fp);
    printf("end of init\n");
    //copy to GPU
   /* CUDA_SAFE_CALL(cudaMalloc((void **)&gpu_vertex,(vertex_size+2)*sizeof(struct vertex)));
    CUDA_SAFE_CALL(cudaMalloc((void**)&gpu_edge,edges_size*sizeof(struct edge)));
    CUDA_SAFE_CALL(cudaMemcpy(gpu_vertex,global_vertex,(vertex_size+2)*sizeof(struct vertex),cudaMemcpyHostToDevice));
    CUDA_SAFE_CALL(cudaMemcpy(gpu_edge,global_edge,edges_size*sizeof(struct edge)));
    */
    return 0;
  }

int graph::is_all_bucket_empty(){
    return min_no_empty_bucket()==-1;
}

int graph::min_no_empty_bucket(){
    for(int i=0;i<MAX_BUKET_NUM;i++){
        if(!bucket_array[i].empty()){
              return i;
        }
    }
    return -1;
}

int graph::bucket_set_to_array(int index, int* array){
    int count = 0;
    std::set<int>::iterator it = bucket_array[index].begin();
    for(;it!=bucket_array[index].end();it++){
            array[count]=*it;
            count++;

	    if(count>=8)
		return 8;
	    if(index==62){
	    printf("!!! %d\n",*it);
}
	    if(*it == 1354){
	    printf("%d oooops!1354\n",index);
}
	    if(count>MAX_BUCKET_SIZE){
		printf("oops!\n");
		exit(1);
	    }
        }
    return count;
}

int graph::init_all_bucket(){
    printf("insert src : %d\n", src);
    bucket_array[0].insert(src);
    return 1;
}

int graph::getFinishedNodes(int bucket){
    int result=0;
    for(int i=0; i<bucket; ++i){
        result+=bucket_array[i].size();
    }
    return result;
}