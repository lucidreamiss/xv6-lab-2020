#include "kernel/types.h"
#include "user/user.h"

#define PIPE_OUT 0
#define PIPE_IN 1

int main() {
    /**
     * pipe()
     * 可以搜索sys_pipe来看其具体实现
     * 返回两个fd到传入的数组中
     * 其中，fd0表示管道输出端，读取端
     *      fd1表示管道输入端，写入端
     *      fd1 -> IIIIIIIII -> fd0
     * 
     * 如何使用管道呢，
     * 1. 我们将数据msg写入到fd1中，write(fd1, msg)：msg -> fd1
     * 2. 此时数据便通过管道传到了fd0，所以我们需要对fd0的数据做接收
     * 3. 子进程read(fd0, receive) ：fd0 -> receive
     * 4. 疑问，是什么时候read都可以吗，管道里的数据不及时接收有问题吗？
     * 5. 答：执行read时可认为等待输入，会等待管道另一端输入数据。
     */
    int pipe_p2c[2]; //父进程到子进程

    pipe(pipe_p2c); // 父 -> 子

    int pipe_c2p[2];

    pipe(pipe_c2p); // 子 -> 父

    int pid = fork();

    char buf = 'P';

    // 出错, 关闭管道退出
    if (pid < 0) {
        close(pipe_p2c[PIPE_IN]);
        close(pipe_p2c[PIPE_OUT]);

        close(pipe_c2p[PIPE_IN]);
        close(pipe_c2p[PIPE_OUT]);

        fprintf(2, "fork pid < 0, pid : %d\n", pid);

        exit(1);
    } else if (pid == 0) {
        // 子进程
        close(pipe_c2p[PIPE_OUT]);
        close(pipe_p2c[PIPE_IN]);

        int exit_status = 0;

        if (read(pipe_p2c[PIPE_OUT], &buf, sizeof(char)) == sizeof(char)) {
            fprintf(1, "%d: received ping\n", getpid());
        } else {
            exit_status = 1;
            fprintf(2, "child read error\n");
        }

        if (write(pipe_c2p[PIPE_IN], &buf, sizeof(char)) != sizeof(char)) {
            exit_status = 2;
            fprintf(2, "child write error\n");
        }
        
        close(pipe_c2p[PIPE_IN]);
        close(pipe_p2c[PIPE_OUT]);

        exit(exit_status);
    } else {
        // 父进程
        close(pipe_c2p[PIPE_IN]);
        close(pipe_p2c[PIPE_OUT]);

        int exit_status = 0;

        int writelen = write(pipe_p2c[PIPE_IN], &buf, sizeof(char));

        if (writelen != sizeof(char)) {
            exit_status = 2;
            fprintf(2, "parent write error, writelen: %d\n", writelen);
        }

        if (read(pipe_c2p[PIPE_OUT], &buf, sizeof(char)) == sizeof(char)) {
            fprintf(1, "%d: received pong\n", getpid());
        } else {
            exit_status = 1;
            fprintf(2, "parent read error\n");
        }
        
        close(pipe_c2p[PIPE_OUT]);
        close(pipe_p2c[PIPE_IN]);

        exit(exit_status);
    }
}