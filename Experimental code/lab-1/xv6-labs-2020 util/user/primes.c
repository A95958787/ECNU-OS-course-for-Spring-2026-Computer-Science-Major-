#include "kernel/types.h"
#include "user/user.h"

#define RD 0 // pipe读端
#define WR 1 // pipe写端

void transmit_data(int lpipe[2], int rpipe[2], int first)
{
  int data;
  // 从左管道读取数据
  while (read(lpipe[RD], &data, sizeof(int)) == sizeof(int)) {
    // 如果无法被当前质数整除，则将数据传递入右管道
    if (data % first != 0) {
      write(rpipe[WR], &data, sizeof(int));
    }
  }
  close(lpipe[RD]);
  close(rpipe[WR]);
}

int main(int argc, char *argv[])
{
  int p[2];
  pipe(p);  // 创建初始管道

  // 将 2 到 35 的整数写入管道
  for (int i = 2; i <= 35; i++) {
    write(p[WR], &i, sizeof(int));
  }

  close(p[WR]);  // 初始管道写端关闭

  while (1) {
    int first;
    // 读取管道中的第一个数
    if (read(p[RD], &first, sizeof(int)) != sizeof(int)) {
      // 如果没有数据可读，退出循环
      break;
    }

    // 打印该质数
    printf("prime %d\n", first);

    int new_pipe[2];
    pipe(new_pipe);  // 创建新管道

    if (fork() == 0) {
      // 子进程传递数据，筛除当前质数的倍数
      close(new_pipe[RD]);
      transmit_data(p, new_pipe, first);
      exit(0);  // 子进程完成后退出
    } else {
      // 父进程关闭旧管道并等待子进程完成
      close(new_pipe[WR]);
      close(p[RD]);
      wait(0);  // 等待子进程结束
      p[RD] = new_pipe[RD];  // 更新父进程的管道读取端
    }
  }

  close(p[RD]);  // 最后关闭管道
  exit(0);
}