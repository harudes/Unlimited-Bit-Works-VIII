#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


/**
 * Arreglo dinamico
 * 
 * *arr es el puntero principal, apunta al inicio del arreglo
 * *ptr_last apunta al final del arreglo
 * 
*/

int* new_array(int **ptr_last);  // retorn el puntero arr, y ptr_last = arr
void realloc_array(int **arr, unsigned int newSize, int **ptr_last);    // incrementa o reduce el tamaño del arreglo
bool array_is_empty(int *arr, int* ptr_last); // si el ptr_last es menor a arr significa que el arreglo esta vacio
void push_element(int* arr, int element, int **ptr_last); // inserta un elemento al final del arreglo, accecible por ptr_last
int get_element(int* arr, int index, int *ptr_last);
void set_element(int* arr, int index, int element, int *ptr_last);
void print_array(int* arr, int *ptr_last);
void delete_element(int* arr, int index, int *ptr_last);

void Intersec(int *targe,int * last_targe,int * A,int *last_A,int *B,int *last_B);  // Se copia en el arreglo targe los elementos de A y B que  tienen en comun
