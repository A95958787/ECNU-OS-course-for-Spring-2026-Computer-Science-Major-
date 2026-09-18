#include "kernel/types.h"
#include "user/user.h"

#define RD 0 //定义pipe的read端
#define WR 1 //定义pipe的write端

int main(int argc)
{
    int child_fd[2]; // 父到子
    int parent_fd[2]; // 子到父
    char buf[1];
    pipe(child_fd);
    pipe(parent_fd);
    int pid = fork();
    if (pid == 0) {
        close(child_fd[RD]);
        close(parent_fd[WR]); // 关闭写端 防止堵塞
        read(parent_fd[RD], buf, sizeof(buf)); // 读取父进程传来的字符（这个用一个char类型的buf来传递）
        close(parent_fd[RD]);
        fprintf(2,"%d: received ping\n", getpid());
        write(child_fd[WR], buf, sizeof(char)); // 向父进程写（也用buf）
        close(child_fd[WR]);
    } else {
        close(parent_fd[RD]);
        close(child_fd[WR]);
        write(parent_fd[WR], buf, sizeof(char)); // 向子进程写
        close(parent_fd[WR]);
        read(child_fd[RD], buf, sizeof(buf)); // 从子进程读取
        close(child_fd[RD]);
        fprintf(2,"%d: received pong\n", getpid());
    }
    exit(0);
}