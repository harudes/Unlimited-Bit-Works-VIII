#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
using namespace std;

struct list_node_s{
	int data;
	list_node_s* next;
	pthread_mutex_t mutex;
};

list_node_s* head_p;
pthread_rwlock_t rwlock;
const int thread_count = 8;
int total_op = 100000;
int initial_list = 1000;

bool Member(int value){
	list_node_s* temp_p;
	
	temp_p = head_p;
	while(temp_p != NULL && temp_p->data < value){
		temp_p = temp_p->next;
	}
	
	if(temp_p == NULL || temp_p->data > value){
		return 0;
	}
	else{
		return 1;
	}
}

bool Insert(int value){
	list_node_s *curr_p = head_p, *pred_p = NULL, *temp_p;
	
	while(curr_p != NULL && curr_p->data < value){
		pred_p = curr_p;
		curr_p = curr_p->next;
	}
	
	if(curr_p == NULL || curr_p->data > value){
		temp_p = new list_node_s;
		temp_p->data = value;
		temp_p->next = curr_p;
		if(pred_p == NULL)
			head_p = temp_p;
		else
			pred_p->next = temp_p;
		return true;
	}
	else
	   return false;
}

bool Delete(int value){
	list_node_s *curr_p = head_p, *pred_p = NULL;
	
	while(curr_p != NULL && curr_p->data < value){
		pred_p = curr_p;
		curr_p = curr_p->next;
	}
	
	if(curr_p != NULL && curr_p->data == value){
		if(pred_p == NULL){
			head_p = curr_p->next;
		}
		else{
			pred_p->next = curr_p->next;
		}
		delete curr_p;
		return true;
	}
	else
	   return false;
}

void* Thread_work(void* rank){
	int my_op = total_op/thread_count;
	double operation;
	for(int i=0; i<my_op; ++i){
		operation = (double) rand()/RAND_MAX;
		if(operation < 0.8){
			pthread_rwlock_rdlock(&rwlock);
			Member(rand()%10000);
			pthread_rwlock_unlock(&rwlock);
		}
		else if(operation < 0.9){
			pthread_rwlock_wrlock(&rwlock);
			Insert(rand()%10000);
			pthread_rwlock_unlock(&rwlock);
		}
		else{
			pthread_rwlock_wrlock(&rwlock);
			Delete(rand()%10000);
			pthread_rwlock_unlock(&rwlock);
		}
	}
}

void printList(){
	
}

int main(int argc, char *argv[]) {
	srand(time(NULL));
	pthread_t threads[thread_count];
	pthread_rwlock_init(&rwlock,NULL);
	double begin, end;
	timeval t1, t2;
	for(int i=0; i<initial_list;){
		if(Insert(rand()%10000))
			++i;
	}
	gettimeofday(&t1, NULL);
	begin = t1.tv_sec + t1.tv_usec/1000000.0;
	for(long i=0; i<thread_count; ++i){
		pthread_create(&threads[i], NULL, Thread_work, (void*) i);
	}
	for(int i=0; i<thread_count; ++i){
		pthread_join(threads[i], NULL);
	}
	gettimeofday(&t2, NULL);
	end = t2.tv_sec + t2.tv_usec/1000000.0;
	cout<<"El tiempo final fue: "<<end - begin<<endl;
	return 0;
}

