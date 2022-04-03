#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
    if(argc < 2) {
        printf("Usage: sleep <num>\n");
        exit(1);
    }

    int t = atoi(argv[1]);
    if(sleep(t) == 0)
        exit(0);
    else
        exit(1);
}