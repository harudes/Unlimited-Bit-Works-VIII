#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <sys/time.h>
using namespace std;

int m=8, n=8000000;

vector<vector<double> > A;
vector<double> x;
vector<double> y;

int thread_count=4;

void *Pth_mat_vect(void* rank) {
	long my_rank = (long) rank;
	int local_m = m/thread_count; 
	int my_first_row = my_rank*local_m;
	int my_last_row = (my_rank+1)*local_m - 1;
	
	for (int i = my_first_row; i <= my_last_row; i++) {
		y[i] = 0.0;
		for (int j = 0; j < n; j++)
			y[i] += A[i][j]*x[j];
	}
	
	return NULL;
}

void fillMatrix(){
	for(int i=0; i<m; ++i){
		for(int j=0; j<n; ++j){
			A[i][j]=(double) (rand()%70000)/7;
		}
	}
}

void fillVector(){
	for(int i=0; i<n; ++i){
		x[i]=(double) (rand()%70000)/7;
	}
}

void printVector(vector<double> &vec){
	for(size_t i=0, top=vec.size(); i<top; ++i){
		cout<<vec[i]<<" ";
	}
	cout<<endl;
}

int main(int argc, char *argv[]) {
	//srand(time(NULL));
	double begin, end;
	timeval t1, t2;
	pthread_t threads[thread_count];
	A.assign(m,vector<double>(n,0.0));
	x.assign(n,0.0);
	y.assign(m,0.0);
	fillMatrix();
	fillVector();
	gettimeofday(&t1, NULL);
	begin = t1.tv_sec + t1.tv_usec/1000000.0;
	for(long i=0; i<thread_count; ++i)
		pthread_create(&threads[i], NULL, Pth_mat_vect, (void*) i);
	for(int i=0; i<thread_count; ++i)
		pthread_join(threads[i], NULL);
	gettimeofday(&t2, NULL);
	end = t2.tv_sec + t2.tv_usec/1000000.0;
	cout<<"El tiempo que ha tomado es: "<<end-begin<< endl;
	//printVector(y);
	return 0;
}

