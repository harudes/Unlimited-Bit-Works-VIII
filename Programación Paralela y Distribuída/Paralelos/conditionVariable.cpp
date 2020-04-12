#include <iostream>
#include <pthread.h>
using namespace std;

int counter = 0;
pthread_mutex_t mutex;
pthread_cond_t cond_var;
const int thread_count=4;

void* Thread_work(void* rank){
	
	long my_rank = (long) rank; 
	
	pthread_mutex_lock(&mutex);
	++counter;
	if(counter == thread_count){
		counter = 0;
		pthread_cond_broadcast(&cond_var);
	}
	else{
		while(pthread_cond_wait(&cond_var, &mutex) != 0);
	}
	pthread_mutex_unlock(&mutex);
	if(my_rank == 0)
		cout<<"Todos los threads pasaron la barrera"<<endl;
}

int main(int argc, char *argv[]) {
	pthread_t threads[thread_count];
	pthread_mutex_init(&mutex, NULL);
	for(long i=0; i<thread_count; ++i){
		pthread_create(&threads[i], NULL, Thread_work, (void*) i);
	}
	
	for(long i=0; i<thread_count; ++i){
		pthread_join(threads[i],NULL);
	}
	return 0;
}

