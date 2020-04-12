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
pthread_mutex_t head_p_mutex;
const int thread_count = 1;
int total_op = 100000;
int initial_list = 1000;

bool Member(int value){
	list_node_s* temp_p;
	
	pthread_mutex_lock(&head_p_mutex);
	temp_p = head_p;
	while(temp_p != NULL && temp_p->data < value){
		if(temp_p->next != NULL)
			pthread_mutex_lock(&(temp_p->next->mutex));
		if(temp_p == head_p)
			pthread_mutex_unlock(&head_p_mutex);
		pthread_mutex_unlock(&(temp_p->mutex));
		temp_p = temp_p->next;
	}
	
	if(temp_p == NULL || temp_p->data > value){
		if(temp_p == head_p)
			pthread_mutex_unlock(&head_p_mutex);
		if(temp_p != NULL)
			pthread_mutex_unlock(&(temp_p->mutex));
		return 0;
	}
	else{
		if(temp_p == head_p)
			pthread_mutex_unlock(&head_p_mutex);
		pthread_mutex_unlock(&(temp_p->mutex));
		return 1;
	}
}

bool Insert(int value){
	list_node_s *curr_p = head_p, *pred_p = NULL, *temp_p;
	
	while(curr_p != NULL && curr_p->data < value){
		if(curr_p->next != NULL)
			pthread_mutex_lock(&(curr_p->next->mutex));
		if(pred_p != NULL)
			pthread_mutex_unlock(&(pred_p->next->mutex));
		else
			pthread_mutex_unlock(&(head_p->mutex));
		pred_p = curr_p;
		curr_p = curr_p->next;
	}
	
	if(curr_p == NULL || curr_p->data > value){
		temp_p = new list_node_s;
		pthread_mutex_init(&(temp_p->mutex),NULL);
		temp_p->data = value;
		temp_p->next = curr_p;
		if(curr_p != NULL)
			pthread_mutex_unlock(&(curr_p->mutex));
		if(pred_p == NULL){
			head_p = temp_p;
			pthread_mutex_unlock(&head_p_mutex);
		}
		else
			pred_p->next = temp_p;
			pthread_mutex_unlock(&(pred_p->mutex));
		return true;
	}
	else{
		if(curr_p != NULL)
			pthread_mutex_unlock(&(curr_p->mutex));
		if(pred_p != NULL)
			pthread_mutex_unlock(&(pred_p->mutex));
		else
			pthread_mutex_unlock(&head_p_mutex);
		return false;
	}
	   
}

bool Delete(int value){
	list_node_s *curr_p = head_p, *pred_p = NULL;
	
	while(curr_p != NULL && curr_p->data < value){
		if(curr_p->next != NULL)
			pthread_mutex_lock(&(curr_p->next->mutex));
		if(pred_p != NULL)
			pthread_mutex_unlock(&(pred_p->next->mutex));
		else
			pthread_mutex_unlock(&(head_p->mutex));
		pred_p = curr_p;
		curr_p = curr_p->next;
	}
	
	if(curr_p != NULL && curr_p->data == value){
		if(pred_p == NULL){
			head_p = curr_p->next;
			pthread_mutex_unlock(&head_p_mutex);
		}
		else{
			pred_p->next = curr_p->next;
			pthread_mutex_unlock(&(pred_p->mutex));
		}
		pthread_mutex_unlock(&(curr_p->mutex));
		pthread_mutex_destroy(&(curr_p->mutex));
		delete curr_p;
		return true;
	}
	else{
		if (pred_p != NULL)
			pthread_mutex_unlock(&(pred_p->mutex));
		if (curr_p != NULL)
			pthread_mutex_unlock(&(curr_p->mutex));
		if (curr_p == head_p)
			pthread_mutex_unlock(&head_p_mutex);
		return false;
	}
	   
}

void* Thread_work(void* rank){
	int my_op = total_op/thread_count;
	double operation;
	for(int i=0; i<my_op; ++i){
		operation = (double) rand()/RAND_MAX;
		if(operation < 0.8){
			Member(rand()%10000);
		}
		else if(operation < 0.9){
			Insert(rand()%10000);
		}
		else{
			Delete(rand()%10000);
		}
	}
}

void printList(){
	
}

int main(int argc, char *argv[]) {
	srand(time(NULL));
	pthread_t threads[thread_count];
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

