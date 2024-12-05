#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// 关闭不必要的文件描述符，确保没有资源泄露
void close_unused_pipes(int pipes[2]) {
  close(pipes[0]);
  close(pipes[1]);
}

// 每个进程的筛选逻辑
void filter(int read_pipe) {
  int prime;

  // 读取第一个数作为素数
  if (read(read_pipe, &prime, sizeof(int)) == 0) {
    close(read_pipe); // 关闭管道读取端
    exit(0);          // 无更多数据，退出
  }

  printf("prime %d\n", prime);

  int next_pipes[2];
  if (pipe(next_pipes) < 0) {
    printf("Error: pipe creation failed\n");
    exit(1);
  }

  int pid = fork();
  if (pid < 0) {
    printf("Error: fork failed\n");
    exit(1);
  } else if (pid == 0) {
    // 子进程：递归筛选
    close(next_pipes[1]);  // 子进程只需要读端
    filter(next_pipes[0]);
  } else {
    // 父进程逻辑：继续筛选
    close(next_pipes[0]);  // 父进程只需要写端
    int num;
    while (read(read_pipe, &num, sizeof(int)) > 0) {
      if (num % prime != 0) {
        write(next_pipes[1], &num, sizeof(int));
      }
    }

    close(read_pipe);      // 关闭自己的读取端
    close(next_pipes[1]);  // 关闭写端
    wait(0);               // 等待子进程结束
    exit(0);
  }
}

int main() {
  int initial_pipes[2];
  if (pipe(initial_pipes) < 0) {
    printf("Error: pipe creation failed\n");
    exit(1);
  }

  int pid = fork();
  if (pid < 0) {
    printf("Error: fork failed\n");
    exit(1);
  } else if (pid == 0) {
    // 子进程：筛选从2到35
    close(initial_pipes[1]);  // 子进程只需要读端
    filter(initial_pipes[0]);
  } else {
    // 父进程：生成数字2到35
    close(initial_pipes[0]);  // 父进程只需要写端
    for (int i = 2; i <= 35; i++) {
      write(initial_pipes[1], &i, sizeof(int));
    }
    close(initial_pipes[1]);  // 关闭写端，告知子进程数据结束
    wait(0);                  // 等待子进程完成
  }

  exit(0);
}