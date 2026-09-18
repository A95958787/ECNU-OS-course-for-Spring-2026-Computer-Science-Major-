#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

static char buf[1024];
static char *args[MAXARG];
static int init_argc = 0;
static int argc_now = 0;

int read_line(int fd) {
    char c[2];
    argc_now = init_argc;
    int offset = 0;

    while (1) {
        // 跳过空白字符，找到第一个非空字符
        while (1) {
            if (read(fd, c, 1) != 1) {
                return 0; // 文件读取结束
            }
            if (c[0] == '\n') {
                return 1; // 读取到换行符
            }
            if (c[0] != ' ' && c[0] != '\t') {
                args[argc_now++] = buf + offset;
                buf[offset++] = c[0];
                break;
            }
        }

        // 继续读取一个完整的参数
        while (1) {
            if (read(fd, buf + offset, 1) != 1) {
                buf[offset] = '\0'; // 文件读取结束
                return 0;
            }
            if (buf[offset] == '\n') {
                buf[offset] = '\0'; // 行结束
                return 1;
            }
            if (buf[offset] == ' ' || buf[offset] == '\t') {
                buf[offset++] = '\0'; // 参数结束
                break;
            }
            ++offset;
        }
    }
}

int main(int argc, char *argv[]) {
    int pid, status;

    if (argc < 2) {
        fprintf(2, "Usage: xargs command ...\n");
        exit(1);
    }

    char *cmd = argv[1];
    init_argc = argc - 1;
    args[0] = cmd;

    for (int i = 1; i < argc; i++) {
        args[i] = argv[i + 1];
    }

    int more_input = 1;
    while (more_input) {
        more_input = read_line(0);  // 从标准输入读取
        args[argc_now] = 0; // 设置参数结尾

        if (more_input == 0 && argc_now == init_argc) {
            exit(0);
        }

        pid = fork();
        if (pid == 0) {
            exec(cmd, args); // 子进程执行命令
            printf("exec failed!\n");
            exit(1);
        } else {
            wait(&status); // 父进程等待子进程结束
        }
    }

    exit(0);
}