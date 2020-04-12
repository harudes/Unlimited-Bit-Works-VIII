#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string>

using namespace std;

int main(int argc, char *argv[]) {
	if(argc=3){
		int v,e;
		v=stoi(argv[1]);
		e=stoi(argv[2]);
		cout<<'p'<<'\t'<<v<<'\t'<<e;
        for(int i=0; i<v; ++i){
            for(int j=0, top=rand()%e+1; j<top; ++j){
                int aux=rand()%v;
                while(i==aux)aux=rand()%v;
                cout<<endl<<'a'<<'\t'<<i<<'\t'<<aux<<'\t'<<rand()%10+1;
            }
        }
	}
	return 0;
}

