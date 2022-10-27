#include "kernel/types.h"
#include "user/user.h"


#define RD 0
#define WR 1

int createSieveProgress(int parentPipe[2]) {
    close(parentPipe[WR]); //关闭父输入端
    int baseprime = 0;
    if (read(parentPipe[RD], &baseprime, sizeof(int)) != sizeof(int)) {
        return 0;
    }
    
    printf("prime %d\n", baseprime);

    int data = 0;

    int p[2];
    
    pipe(p);

    while (read(parentPipe[RD], &data, sizeof(int) == sizeof(int))) {
        if (data % baseprime) {
            write(p[WR], &data, sizeof(int));
        }
    }
    
    if (fork()) {
        close(p[WR]);
        close(p[RD]);
        wait(0);
    } else {
        createSieveProgress(p);
    }
    return 0;
}

/**
 * @brief 线性素数筛 通道版本
 * 
 * 这里要求使用通道来对素数筛选，每个通道仅筛选其中一项素数
 * 
 * 1. 先来个生成2-35数的进程
 * 
 * 2. 创建管道过滤
 * 
 * 1. 每个通道筛选其遇到的第一个素数，当通道无法从上一个通道获得质数时，便关闭
 * 
 * 
 * 问题1：进程的孙子进程关闭时，进程的wait会受到影响吗？
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char const *argv[]) {

    int p[2];

    pipe(p);

    // 父进程，用于生成源数据 2-35
    for (int i = 2; i < 36; i++) { write(p[WR], &i, sizeof(int)); }

    if (fork()) {
        close(p[RD]);
        close(p[WR]);
        wait(0);
    } else {
        createSieveProgress(p);
    }
    exit(0);
}