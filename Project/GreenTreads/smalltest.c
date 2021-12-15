#include <stdio.h>
#include "green.h"
int flag = 0;
green_cond_t cond0, cond1, cond2;
green_mutex_t mutex;
long count;

void * test(void * arg){
    int i = *(int*)arg;
    int loop = 10000000;
    while(loop >0){
        if(loop%100000 == 0){
            printf("Thread %d: %d\n", i, loop);
        } 
        loop--;
        // green_yield();
    }
}

void * test2(void *arg){
    int i = *(int*)arg;
    int loop = 4;
    while(loop >0){
        if(flag == i){
            printf("Thread %d: %d\n", i, loop);
            loop--;
            flag = (i+1)%3;
            if(i ==0){
                green_cond_signal(&cond1);
            }
            if(i == 1){
                printf("Here\n");
                green_cond_signal(&cond2);
            }
            if(i == 2){
                green_cond_signal(&cond0);
            }
        }
        else{
            printf("Thread %d wait\n",i);
            if(i ==0){
                green_cond_wait(&cond0);
            }
            if(i == 1){
                green_cond_wait(&cond1);
            }
            if(i == 2){
                green_cond_wait(&cond2);
            }
            
        }
    }
}

void * test3(void * arg){
    int i = *(int*)arg;
    int loop = 1000000;
    while(loop >0){
        loop--;
        count ++;
        // green_yield();
    }
}

void * test4(void * arg){
    int i = *(int*)arg;
    int loop = 100000;
    while(loop >0){
        if(loop%10000 == 0){
            printf("Thread %d: %d\n", i, loop);
        } 
        // we lock
        green_mutex_lock(&mutex);
        loop--;

        count ++;

        // we unlock 
        green_mutex_unlock(&mutex);
    }
    printf("End thread:%d \n", i);
    if(mutex.head == NULL){
        printf("Mutex empty\n");
    }
}

int main(){
    green_t g0, g1, g2,g3;
    count = 0;
    int a0 = 0;
    int a1 = 1;
    int a2 = 2;
    int a3 = 3;
    green_cond_init(&cond0);
    green_cond_init(&cond1);
    green_cond_init(&cond2);
    green_mutex_init(&mutex);
    green_create(&g0, test4, &a0);
    green_create(&g1, test4, &a1);
    green_create(&g2, test4, &a2);
    //green_create(&g3, test4, &a3);


    printf("Threads created\n");
    green_join(&g0, NULL);
    green_join(&g1, NULL);
    green_join(&g2, NULL);
    //green_join(&g3, NULL);

    printf("Done\n");
    printf("count:%ld\n",count);
    return 0;
}