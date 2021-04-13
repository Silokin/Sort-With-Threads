/*Nikolis Ioannis 3100127*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

void *sort(void*th);

void merging(int low, int mid, int high);