#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/fs.h"

void find(const char *path, const char *filename) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open: %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot fstat: %s\n", path);
        close(fd);
        return;
    }

    if (st.type != T_DIR) {
        fprintf(2, "param1 not a dir: %s\n", path);
    }

    strcpy(buf, path);

    p = buf + strlen(path);

    *p++ = '/'; //此时p指向/的下一位上

    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        // 表明没啥用的目录项
        if (de.inum == 0) {
            continue;
        }
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;

        if (stat(buf, &st) < 0) {
            fprintf(2, "error");
            continue;
        }

        // 是目录，同时排除在.与..中递归
        if (st.type == T_DIR && strcmp(p, ".") != 0 && strcmp(p, "..") != 0) {
            find(buf, filename);
        } else if (strcmp(filename, p) == 0) {
            printf("%s\n", buf);
        }    
    }
    close(fd);
}

/**
 * @brief Write a simple version of the UNIX find program: 
 * find all the files in a directory tree with a specific name. 
 * 
 * 1. 看下ls.c是怎么遍历文件的
 *      1. open 指定目录，从指定目录开始遍历
 *      2. 系统调用fstat获取当前fd的信息，打印出结果
 *      
 * 2. 参照ls.c的做法
 *      1. open 指定目录，从该目录开始遍历
 *      2. 如何遍历，调用read读取目录项，dirent，拼接dirent名称，再用open的形式获取fd，再获取fstat
 *      2. 系统调用fstat获取当前fd的信息，若为文件，则字符串比较，若为目录，则执行递归的寻找操作
 *     
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char const *argv[]) {

    if (argc != 3) {
        fprintf(2, "usage: find <directory> <filename>");
        exit(1);
    }

    find(argv[1], argv[2]);
    return 0;
}
