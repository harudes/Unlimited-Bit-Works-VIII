#include <iostream>
#include <pthread.h>
using namespace std;

pthread_barrier_t barrier;
const int thread_count=4;

void* Thread_work(void* rank){
	
	long my_rank = (long) rank; 
	
	pthread_barrier_wait(&barrier);
	if(my_rank == 0)
		cout<<"Todos los threads pasaron la barrera"<<endl;
}

int main(int argc, char *argv[]) {
	pthread_t threads[thread_count];
	pthread_barrier_init(&barrier, NULL, thread_count);
	for(long i=0; i<thread_count; ++i){
		pthread_create(&threads[i], NULL, Thread_work, (void*) i);
	}
	
	for(long i=0; i<thread_count; ++i){
		pthread_join(threads[i],NULL);
	}
	return 0;
}

