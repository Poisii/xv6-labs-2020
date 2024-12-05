#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: sleep <ticks>\n");
    exit(1);
  }

  int ticks = atoi(argv[1]); // 将参数转为整数
  if (ticks <= 0) {
    printf("Error: ticks must be a positive number\n");
    exit(1);
  }

  sleep(ticks); // 调用系统调用
  exit(0);
}