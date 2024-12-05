#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  int i;
  char *nargv[MAXARG];
  int mask;

  if (argc < 2) {
    fprintf(2, "trace: missing arguments\n");
    exit(1);
  }

  mask = atoi(argv[1]);  // 获取掩码参数
  if (trace(mask) < 0) {  // 调用 trace 系统调用
    fprintf(2, "%s: trace failed\n", argv[0]);
    exit(1);
  }

  for (i = 2; i < argc && i < MAXARG; i++) {
    nargv[i-2] = argv[i];  // 将参数从 argv 复制到 nargv
  }

  exec(nargv[0], nargv);  // 执行目标程序
  exit(0);
}
