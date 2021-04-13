/*Nikolis Ioannis 3100127*/


#include "os_ex2.h"

int n;
int t;
int m;
int *array;
int *b;
pthread_t *threads;
struct timeval tv;
pthread_mutex_t myMutex;

void *sort(void*th){
    int c,d,rc,swap,k,l;
    int *thId = (int*)th;
    l = ((*thId/(float)t)*n); /*mexri pou*/
    k = ((*thId-1)/(float)t)*n; /*apo pou*/
    
    printf("Thread %d started for sorting cells from %d to %d.\n",*thId,k,l-1);

    /*bubble sort*/
    for (c=0; c<l-k-1;c++){
        for(d = 0; d <l-k-c-1;d++){
            /*an eimaste sto 1o mode lockaroume edw to thread*/
            if(m==1){
                rc = pthread_mutex_lock(&myMutex);
                if (rc != 0) {	
                    printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
                    pthread_exit(&rc);
                }
            }
            if(array[k+d] > array[k+d+1]){
                /*an eimaste sto 2o mode lockaroume otan theloume na grapsoume*/
                if(m==2){
                    rc = pthread_mutex_lock(&myMutex);
                    if (rc != 0) {	
                        printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
                        pthread_exit(&rc);
                    }
                }
                swap = array[k+d];
                array[k+d] = array[k+d+1];
                array[k+d+1] = swap;
                
                /*sto 2o mode xelockaroume afou grapsoume*/
                if(m==2){
                    rc = pthread_mutex_unlock(&myMutex);
                    if (rc != 0) {	
                        printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
                        pthread_exit(&rc);
                    }
                }
            }
            /*sto 1o mode xelockaroume afou diavasoume kai grapsoume*/
            if(m==1){
                rc = pthread_mutex_unlock(&myMutex);
                if (rc != 0) {	
                    printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
                    pthread_exit(&rc);
                }
            }

        }
    }

    /*perimenw na teleiwsei to amesws epomeno thread prwtou tupwsw thn sortarismenh lista*/
    if(*thId!=t){
        void *status;
        rc = pthread_join(threads[*thId], &status);
        if (rc != 0) {
            printf("ERROR: return code from pthread_join() is %d\n", rc);
            exit(-1);		
        }
        printf("Thread %d terminated successfully.\n",*(int *) status);   
    }
  
    printf("Thread %d sorted as follows:\n",*thId);
    for (c=0; c<l-k;c++){
        printf("%d\n",array[k+c]);
    }
    pthread_exit(th);
    
}

void merging(int low, int mid, int high) {
   int l1, l2, i;

   for(l1 = low, l2 = mid + 1, i = low; l1 <= mid && l2 <= high; i++) {
      if(array[l1] <= array[l2])
         b[i] = array[l1++];
      else
         b[i] = array[l2++];
   }
   
   while(l1 <= mid)    
      b[i++] = array[l1++];

   while(l2 <= high)   
      b[i++] = array[l2++];

   for(i = low; i <= high; i++)
      array[i] = b[i];
}

int main(int argc, char*argv[]) {
    time_t start,end;
    gettimeofday(&tv,NULL);
    start = tv.tv_usec;
    int i,rc;
    if(argc!=5){
      printf("ERROR: The program should take at least four arguments!\n");
      exit(-1);
    }
    
    n = atoi(argv[1]);
    int s = atoi(argv[3]);
    t = atoi(argv[2]);
    m = atoi(argv[4]);

    threads = malloc(t*sizeof(pthread_t));
    array = malloc(n*sizeof(int));
    b = malloc((n-1)*sizeof(int));
    
    
    /*anoigw h dhmiourgw an dn uparxei to .dat*/
    FILE *fp ;
    fp = fopen("results.dat","w");
   
    /*arxikopoiw ton pinaka me tous tuxaious arithmous kai ton tupwnw sto .dat*/
    srand(s);
    fprintf(fp,"Initial array:{");
    for(i=0;i<n;i++){
        /*diairw me to 10000000 gia na parw pio euanagnwstous arithmous*/
        array[i] = rand()/10000000;
        rc=array[i];
        if(i!=n-1){fprintf(fp,"%d,",rc);}
        else {fprintf(fp,"%d}\n---------------------------------------------\n",rc);}
    }
    /*arxikopoiisi tou mutex*/
    if(m==1||m==2){
        rc = pthread_mutex_init(&myMutex, NULL);
        if (rc != 0) {
            printf("ERROR: return code from pthread_mutex_init() is %d\n", rc);
            exit(-1);
        }
    }
    int *threadId;
    threadId = malloc(t*sizeof(int));
    /*dhmiourgw ena pinaka pou periexei ta id twn threads kai ta arxikopoiw*/
    for(i=0;i<t;i++){
        threadId[i] = i+1;
        rc = pthread_create(&threads[i],NULL,sort,&threadId[i]);
        /*elegxos oti to thread dimiourgithike swsta.*/
    	if (rc != 0) {
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(-1);
    	}
    }
   	
    void *status;
    rc = pthread_join(threads[0], &status);
    if (rc != 0) {
	printf("ERROR: return code from pthread_join() is %d\n", rc);
	exit(-1);		
    }
    printf("Thread %d terminated successfully.\n", *(int *) status);
    
    /*"katastrofi" mutex*/
    if(m==1||m==2){
        rc = pthread_mutex_destroy(&myMutex);
        if (rc != 0) {
            printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
            exit(-1);		
        }
    }
    
    /*kalw thn merging gia na sugxwneusw tous hdh sortarismenoun pinakes*/
    int high,low;
    if(t==8){
        high=(2/(float)t)*n;low=(0/(float)t)*n;
        merging(low,(low+high)/2-1,high-1);
        high=(4/(float)t)*n;low=(2/(float)t)*n;
        merging(low,(low+high)/2-1,high-1);
        high=(6/(float)t)*n;low=(4/(float)t)*n;
        merging(low,(low+high)/2-1,high-1);
        high=(8/(float)t)*n;low=(6/(float)t)*n;
        merging(low,(low+high)/2-1,high-1);
        t=t/2;
    }
    if(t==4){
        high=(2/(float)t)*n;low=(0/(float)t)*n;
        merging(low,(low+high)/2-1,high-1);
        high=(4/(float)t)*n;low=(2/(float)t)*n;
        merging(low,(low+high)/2-1,high-1);
        t=t/2;
    }
    if(t==2){
        high=(2/(float)t)*n;low=(0/(float)t)*n;
        merging(low,(low+high)/2-1,high-1);
        t=t/2;
    }
    if(t==1){
        fprintf(fp,"Sorted array={");
        for(i=0;i<n;i++){  
            rc=array[i];
            if(i!=n-1){fprintf(fp,"%d,",rc);}
            else {fprintf(fp,"%d}",rc);}
        }
    }
    
  /*ypologismos xronou ektelesis*/
    gettimeofday(&tv,NULL);
    end = tv.tv_usec;
    end = end - start;
    if(end<0)end=end*(-1);
    printf("Mergesort completed after : %d nanoseconds.",(int)((double)(end)*1000));
    return 1;
}