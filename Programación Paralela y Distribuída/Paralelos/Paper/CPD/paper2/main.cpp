/**
 * Authors:
 *          Heredia Parillo, JuanPablo Andrew
 *          Rendon Zuniga, Luis Francisco
 * 
 * File:    
 *          main.cpp (sssp)
 * Purpose:
 *          Find the shortest path from a source node to the rest.
 *          Implementation of "Scalable Single Source Shortest Path Algorithms 
 *          for Massively Parallel Systems".
 * Compile:
 *          g++ -Wall -fopenmp -I. -o sssp main.cpp -lm (makefile)
 * Run:
 *          ./sssp <cant_threads> <cant_nodes> <source_node> <delta>
 * Notes:
 *          -
 * 
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <omp.h>
#include <utility>
#include <math.h>
#include <algorithm>

#define MAX_WEIGHT 10
#define MAX_DEGREE 10
#define MAX_DISTANCE 1000
#define INFINITO 10000
#define ENDL std::endl
#define COUT std::cout

typedef unsigned int uint;

int cant_threads;
int delta;

class funtor1{
public:
    bool operator ()(std::pair<int,int> a, std::pair<int,int> b){
        return a.second<b.second;
    } 
};

std::vector< std::vector< std::pair<int,int> > > graph;         // Matriz dispersa, cada nodo contiene sus aristas (destiny,weight)
std::vector< int > distances;
std::vector< int > node_predecessor;
std::vector< std::vector< int > > buckets;                      // Contenedor de nodos (indices al grafo)
std::vector< std::pair<int,int> > ranks;
std::vector<int> node_ranks;

void thread_work (int first_index, int last_index, int &k);
void deltaStepping();
bool relax (int u, int v);
void printGraph();

int main (int argc, char** argv){
    if(argc != 5){
        printf ("Incorrect parameters \n");
        return 0;
    }
    
    cant_threads        = atoi (argv[1]);
    uint cant_nodes     = atoi (argv[2]);
    uint cant_edges     = 0;
    uint node_source    = atoi (argv[3]);
    delta               = atoi (argv[4]);

    srand(0);

 /////////////////////////////////////////////////////////////////////////////
 // Inicializar
 /////////////////////////////////////////////////////////////////////////////

    graph.assign(cant_nodes,std::vector<std::pair<int,int> >(0));
    for(uint i=0; i<cant_nodes; ++i){
        ranks.push_back(std::make_pair(i,0));
    }
    distances.assign(cant_nodes,INFINITO);
    node_predecessor.assign(cant_nodes,-1);

    for (uint i = 0; i < cant_nodes; ++i){
        int weight;
        uint node_aux;
        int d = 1 + (rand() % MAX_DEGREE);
        ranks[i].second+=d;
        for (int j = 0; j < d; ++j){
            weight = 1 + (rand () % MAX_WEIGHT);
            node_aux = rand () % cant_nodes;
        
            while(node_aux == i) node_aux = rand () % cant_nodes;
        
            graph[i].push_back (std::make_pair (node_aux, weight) );
            graph[node_aux].push_back (std::make_pair (i, weight) );
            cant_edges++;
        }
    }
    funtor1 funt;
    std::sort(ranks.begin(),ranks.end(),funt);
    node_ranks.assign(cant_nodes,0);
    
    for(size_t i=0; i<ranks.size(); ++i){
        node_ranks[ranks[i].first]=i;
    }

    uint cant_buckets = (MAX_DISTANCE / delta) +1;

    std::vector< int >  temp2;
    for (uint i = 0; i < cant_buckets; ++i)
        buckets.push_back(temp2);

    buckets[0].push_back(node_source);
    distances[node_source] = 0;
    
    for (uint i = 0; i < cant_nodes; ++i)
        buckets[cant_buckets].push_back (i);
    
 /////////////////////////////////////////////////////////////////////////////
 // Separación del trabajo
 /////////////////////////////////////////////////////////////////////////////

    int thread_cant_nodes = ceil ((double)cant_nodes / (double)cant_threads); 
    int k=0;
    deltaStepping();

 /////////////////////////////////////////////////////////////////////////////
 // Finalize
 /////////////////////////////////////////////////////////////////////////////

    for(uint i = 0; i < cant_nodes; ++i)
        std::cout<<i<<" : ("<<node_predecessor[i]<<", "<<distances[i]<<")"<<ENDL;

    return 0;
}

void deltaStepping(){
    int k=0;
    #pragma omp parallel num_threads(cant_threads) shared(node_ranks, k, buckets, distances, node_predecessor, graph, COUT, delta, cant_threads) default(none)
    for (; k < (MAX_DISTANCE/delta); ){         /* Epoca*/
        /*process*/
        while ( !buckets[k].empty() ){                 /*Fase*/
            std::vector<int> aux_bucket;
            for (size_t u = 0; u < buckets[k].size (); ++u){ //// [ solo nodos entre first y last]
                int actual_u = buckets[k][u] ;
                if(node_ranks[actual_u]%cant_threads == omp_get_thread_num()){
                    for (size_t v = 0; v < graph[actual_u].size (); ++v){
                        /*relax*/
                        int actual_v = graph[actual_u][v].first;
                        if(graph[actual_u][actual_v].second<delta ||
                         (graph[actual_u][actual_v].second + distances[actual_v])/delta == k){
                            if (relax (actual_u, actual_v)){
                                aux_bucket.push_back (actual_v);
                            }
                        }
                    }
                }                    
            }
            //}
            //Intersección
            {
                std::vector<int> temp_bucket(buckets[k]);
                #pragma omp single
                buckets[k].clear();
                for (size_t  i = 0; i < temp_bucket.size (); ++i){
                    for (size_t j = 0; j < aux_bucket.size (); ++j){
                        if (temp_bucket[i] == aux_bucket[j])
                            #pragma omp critical
                            buckets[k].push_back(temp_bucket[i]);
                    }
                }
            }
        }

        for (size_t u = 0; u < buckets[k].size (); ++u){ //// [ solo nodos entre first y last]
                int actual_u = buckets[k][u] ;
                if(node_ranks[actual_u]%cant_threads == omp_get_thread_num()){
                    for (size_t v = 0; v < graph[actual_u].size (); ++v){
                        /*relax*/
                        int actual_v = graph[actual_u][v].first;
                        if(graph[actual_u][actual_v].second>=delta ||
                         (graph[actual_u][actual_v].second + distances[actual_v])/delta > k){
                            relax (actual_u, actual_v);
                        }
                    }
                }                    
            }

        #pragma omp single
        {
            k++;
            while(buckets[k].empty() && k<buckets.size()){
                k++;
            }
        }
    }
}

/// Revisa si debe tener una parte critica
bool relax (int u, int v){
    uint old_bucket;
    if(distances[v]!=INFINITO)
        old_bucket = floor ((double)distances[v] / (double)delta);
    else
        old_bucket = buckets.size()-1;
    int temp = distances[v];
    int new_distance = distances[u] + graph[u][v].second;
    if(new_distance < distances[v]){
        #pragma omp critical
        {
            if(new_distance < distances[v]){
                distances[v] = new_distance;
                node_predecessor[v]=u;  
            }
        }
    }
    uint new_bucket = floor ((double)distances[v] / (double)delta);

    if (new_bucket < old_bucket){
        #pragma omp critical
            buckets[new_bucket].push_back(v);
            for (size_t i = 0; i < buckets[old_bucket].size (); ++i){
                if (buckets[old_bucket][i] == v){
                    #pragma omp critical
                    buckets[old_bucket].erase (buckets[old_bucket].begin () + i);
                    break;
                }
            }
    }
    return (temp != distances[v]);
}

void printGraph(){
    std::cout<<graph.size()<<ENDL;
    for(uint i=0; i<graph.size(); ++i){
        std::cout<<i<<" "<<graph[i].size()<<": ";
        for(uint j=0; j<graph[i].size(); ++j){
            std::cout<<i<<" "<<graph[i][j].first<<" "<<graph[i][j].second<<"  ";
        }
        std::cout<<ENDL;
    }
    
}