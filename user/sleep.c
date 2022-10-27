#include "kernel/types.h"
#include "user/user.h"

int main(int argc, const char *argv[]) {
    if (argc != 2) {
        fprintf(2, "usage: sleep <time>\n");
        exit(1);
    }

    fprintf(1, "%d - %s %s\n", argc, argv[0], argv[1]);

    sleep(atoi(argv[1]));
    exit(0);
}