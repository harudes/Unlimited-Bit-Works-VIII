#define __vertex_h
#ifdef __vertex_h
#include "stdio.h"
#include "stdlib.h"
#include <set>


#define MAX_BUKET_NUM 0x7fff
#define MAX_DISTANCE 0x7fffff
#define MAX_BUCKET_SIZE 2048
#define MAX_RESULT_SIZE 2048
#include <chrono>
using namespace std::chrono;

class graph{
public:

    struct edge{
        int des_v;
        int distance;
    };
     
    struct vertex{
        int edge_index;
        int dist;
        int pre_vertex;
    };


    struct gpuResult{
      int index;
      int old_distance;
      int new_distance;
    };


    typedef std::set<int> bucket;

    graph(char* filepath, int D);
    ~graph();
    int init_all_bucket();
    int is_all_bucket_empty();
    int min_no_empty_bucket();
    int init_memory(char* filepath);
    int init_graph();

    void print_bucket();

    int bucket_set_to_array(int index, int* array);
    int getFinishedNodes(int bucket);

    struct vertex *global_vertex;
    struct edge *global_edge;
   int vertex_size,edges_size;
   int graph_init;
   int delta;
   bucket bucket_array[MAX_BUKET_NUM];
   int src ,dest;

   
    struct vertex *gpu_vertex;
    struct edge *gpu_edge;
   int* vertex_buf_ptr;
   struct gpuResult *gpu_result_buf;
    struct gpuResult *gpu_used_result_buf;
  
};
#endif
