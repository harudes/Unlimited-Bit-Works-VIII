#include <iostream>
#include <pthread.h>
#include <semaphore.h>
using namespace std;

int counter=0;
sem_t count_sem;
sem_t barrier_sem;
const int thread_count=4;

void* Thread_work(void* rank){
	
	long my_rank = (long) rank; 
	
	sem_wait(&count_sem);
	if(counter == thread_count - 1){
		counter = 0;
		sem_post(&count_sem);
		for(int i=0; i<thread_count - 1; ++i){
			sem_post(&barrier_sem);
		}
	}
	else{
		++counter;
		sem_post(&count_sem);
		sem_wait(&barrier_sem);
	}
	if(my_rank == 0)
	   cout<<"Todos los threads pasaron la barrera"<<endl;
}

int main(int argc, char *argv[]) {
	pthread_t threads[thread_count];
	sem_init(&count_sem, 0, 1);
	sem_init(&barrier_sem, 0, 0);
	for(long i=0; i<thread_count; ++i){
		pthread_create(&threads[i], NULL, Thread_work, (void*) i);
	}
	
	for(long i=0; i<thread_count; ++i){
		pthread_join(threads[i],NULL);
	}
	return 0;
}

