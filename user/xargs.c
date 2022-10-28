#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"


void xargs(char *argv[]) {
    int pid, status;
    if ((pid=fork()) == 0) {
        exec(argv[0], argv);
        exit(1);
    }
    wait(&status);
    return;
}

/**
 * @brief Write a simple version of the UNIX xargs program: 
 * read lines from the standard input and run a command for each line,
 * supplying the line as arguments to the command.
 * 
 * 1. 调用fork与exec来执行每个command，并使用wait来等待子进程完成输出
 * 2. 子进程可能输出多行，父进程在读入时需要注意，多行对应需要父进程执行多条命令
 * 3. kernel/param.h定义了MAXARG，或许对你定义argv array有帮助
 * 4. 记得修改Makefile中的UPROGS，记得make clean
 * 
 * 这里需要状态机，我们明天大部分精力会用来开发这个程序，来看如何编写该命令行
 * 
 * 1. 当 command line中输出xargs时，这时我们会用标准输入中的每一行去执行xargs后的命令行
 * 忽略标准输入这点，xargs后面跟随的就是一条命令，一条待补充参数的命令，待补充参数就来自xargs前执行得到的结果
 * 
 * 就相当于是fork子进程去执行n条命令 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char *argv[]) {
    int i, j;
    char c, buf[512], *xargv[MAXARG] = {0};
    if (argc < 2 || argc - 1 > MAXARG) {
        fprintf(2, "usage: xargs <cmd> {args}, # of args should be less than 32\n");
        exit(1);
    }

    // 对于sizeof有个疑问，他是怎么求出数组长度的，难不成在定义buf[512]的时候，这块区域就已经被获取到了？
    memset(buf, 0, sizeof(buf));

    for (i = 1; i < argc; i++) {
        xargv[i - 1] = argv[i];
    }

    for (; i < MAXARG; i++) {
        xargv[i] = 0;
    }

    j = 0;

    while (read(0, &c, 1) > 0) {
        if (c != '\n')
            buf[j++] = c;
        else {
            if (j != 0) {
                buf[j] = '\0';
                xargv[argc - 1] = buf;
                xargs(xargv);
                j = 0;
            }
        }
    }
    if (j != 0) {
        buf[j] = '\0';
        xargv[argc - 1] = buf;
        xargs(xargv);
    }
    exit(0);
}
