#include <iostream>
#include <pthread.h>
#include <thread>
using namespace std;

int counter=0;
const int thread_count=4;
pthread_mutex_t barrier_mutex;

void* Thread_work(void* rank){
	
	long my_rank = (long) rank; 
	
	pthread_mutex_lock(&barrier_mutex);
	counter++;
	pthread_mutex_unlock(&barrier_mutex);
	while(counter < thread_count);
	if(my_rank == 0)
		cout<<"Todos los threads pasaron la barrera"<<endl;
}

int main(int argc, char *argv[]) {
	pthread_t threads[thread_count];
	pthread_mutex_init(&barrier_mutex, NULL);
	for(long i=0; i<thread_count; ++i){
		pthread_create(&threads[i], NULL, Thread_work, (void*) i);
	}
	
	for(long i=0; i<thread_count; ++i){
		pthread_join(threads[i],NULL);
	}
	return 0;
}

