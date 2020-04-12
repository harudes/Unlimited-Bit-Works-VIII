#include "dyrray.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

int* new_array(int **ptr_last){
    int *arr =(int*) malloc(1*sizeof(int));
    *ptr_last = arr;
    return arr;
}

void realloc_array(int **arr, unsigned int newSize, int **ptr_last){
    //*arr = (int*) realloc(*arr,newSize * sizeof(int));
    
    int* new_array = (int*) malloc(newSize * sizeof(int));
    for(int i=0; i<newSize-1; ++i){
        new_array[i]=(*arr)[i];
    }
    free(*arr);
    *arr = new_array;
    *ptr_last = *arr + newSize - 1;
}

bool array_is_empty(int *arr, int* ptr_last){
    return (ptr_last < arr);
}

void push_element(int* arr, int element, int **ptr_last){
    unsigned int size = *ptr_last - arr + 1;
    realloc_array(&arr,size+1,ptr_last);  
    **ptr_last = element;
}

int get_element(int* arr, int index, int *ptr_last){
    unsigned int size = ptr_last - arr;
    if(index >= size) return -1;
    return *(arr + index);
}

void set_element(int* arr, int index, int element, int *ptr_last){
    unsigned int size = ptr_last - arr;
    if(index >= size) return;
    *(arr + index) = element;
}

void print_array(int* arr, int *ptr_last){
    unsigned int size = ptr_last - arr;
    printf("[");
    if(!array_is_empty(arr,ptr_last)){
        printf("%d", *(arr));
        for(int i=1; i<size; ++i) {
            printf(", %d",*(arr + i));
        }
    }
    printf("]\n");
}

void delete_element(int* arr, int index, int *ptr_last){
    unsigned int size = ptr_last - arr;
    if (index >= size) return;
    //int *aux = arr + index;
    for(int i = index;i<size-1;++i){
        *(arr + i) = *(arr + i + 1);
    }
    ptr_last--;
}

void Intersec(int *targe,int * last_targe,int * A,int *last_A,int *B,int *last_B){
    int *temp = new_array(last_targe);
    unsigned int size_A = last_A - A;
    unsigned int size_B = last_B - B;
    for(int i=0;i<size_A;++i){
        for(int j=0;j<size_B;++j){
            if(*(A+i) == *(B+j))
                push_element(temp,*(A+i),last_targe);
        }
    }
    targe = temp;
}