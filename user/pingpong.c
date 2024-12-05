#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
  int pipe1[2]; // 父进程 -> 子进程
  int pipe2[2]; // 子进程 -> 父进程

  // 创建两个管道
  if (pipe(pipe1) < 0 || pipe(pipe2) < 0) {
    printf("Error: pipe creation failed\n");
    exit(1);
  }

  int pid = fork(); // 创建子进程

  if (pid < 0) {
    printf("Error: fork failed\n");
    exit(1);
  } else if (pid == 0) {
    // 子进程逻辑
    char byte;
    close(pipe1[1]); // 关闭 pipe1 的写端（父进程写 -> 子进程读）
    close(pipe2[0]); // 关闭 pipe2 的读端（子进程写 -> 父进程读）

    // 从 pipe1 读取字节
    if (read(pipe1[0], &byte, 1) != 1) {
      printf("Error: child read failed\n");
      exit(1);
    }
    printf("%d: received ping\n", getpid());

    // 向 pipe2 写入字节
    if (write(pipe2[1], &byte, 1) != 1) {
      printf("Error: child write failed\n");
      exit(1);
    }

    close(pipe1[0]);
    close(pipe2[1]);
    exit(0);

  } else {
    // 父进程逻辑
    char byte = 'x';
    close(pipe1[0]); // 关闭 pipe1 的读端（父进程写 -> 子进程读）
    close(pipe2[1]); // 关闭 pipe2 的写端（子进程写 -> 父进程读）

    // 向 pipe1 写入字节
    if (write(pipe1[1], &byte, 1) != 1) {
      printf("Error: parent write failed\n");
      exit(1);
    }

    // 从 pipe2 读取字节
    if (read(pipe2[0], &byte, 1) != 1) {
      printf("Error: parent read failed\n");
      exit(1);
    }
    printf("%d: received pong\n", getpid());

    close(pipe1[1]);
    close(pipe2[0]);
    wait(0); // 等待子进程退出
  }

  exit(0);
}