#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/param.h"

#define MAXLEN 512 // 每行输入的最大长度

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: xargs <command>\n");
        exit(1);
    }

    char buf[MAXLEN];
    char *cmd_argv[MAXARG]; // 参数数组
    int i;

    // 初始化参数数组，前面是命令，后面是标准输入的一行
    for (i = 0; i < argc - 1; i++) {
        cmd_argv[i] = argv[i + 1];
    }
    cmd_argv[argc - 1] = 0;

    while (1) {
        // 读取一行输入
        int n = 0;
        char c;
        while (read(0, &c, 1) == 1 && c != '\n') {
            if (n < MAXLEN - 1) {
                buf[n++] = c;
            }
        }
        buf[n] = '\0';

        // 如果没有读到内容，退出
        if (n == 0 && c != '\n') {
            break;
        }

        // 把标准输入的内容添加到命令参数中
        cmd_argv[argc - 1] = buf;
        cmd_argv[argc] = 0;

        // 创建子进程执行命令
        int pid = fork();
        if (pid < 0) {
            printf("fork failed\n");
            exit(1);
        }
        if (pid == 0) {
            // 子进程
            exec(cmd_argv[0], cmd_argv);
            // exec 如果失败则返回
            printf("exec %s failed\n", cmd_argv[0]);
            exit(1);
        } else {
            // 父进程等待子进程完成
            wait(0);
        }
    }
    exit(0);
}