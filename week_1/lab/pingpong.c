#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
    int p[2];
    char buf[16];

    pipe(p);
    if(fork() == 0) {
        read(p[0], buf, 1);
        if(buf[0] == 'a') {
            printf("%d: received ping\n", getpid());
            write(p[1], "b", 1);
        }
    }
    else {
        write(p[1], "a", 1);
        wait(0);
        read(p[0], buf, 1);
        if(buf[0] == 'b')
            printf("%d: received pong\n", getpid());
    }
    close(p[0]);
    close(p[1]); 
    exit(0);

}