//
// Created by root on 2020/12/21.
//
#include <stdlib.h>
#include "time.h"
#include "stdio.h"
#include "pthread.h"
int isInitRandom = 0;
void init_move_system(int moveSeed){
    //初始化移动随机数种子

    if (isInitRandom == 0) {
        srand(time(0) + moveSeed);
        isInitRandom = 1;
    }
    return;
}

pthread_mutex_t pthreadMutex;
void funa(){
    printf("a尝试获取锁\n");
    pthread_mutex_trylock(&pthreadMutex);
    printf("a获取锁\n");
    pthread_mutex_unlock(&pthreadMutex);
    printf("a已经解锁\n");
}

void funb(){
    printf("b尝试获取锁\n");
    pthread_mutex_trylock(&pthreadMutex);
    printf("b获取锁\n");
    funa();
    pthread_mutex_unlock(&pthreadMutex);
    printf("b已经解锁\n");
}

int main(){
    pthread_mutex_init(&pthreadMutex,NULL);
    funb();
    return 0;
}

