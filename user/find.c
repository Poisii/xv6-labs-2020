#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fs.h"

// 检查文件是否是 "." 或 ".."
int is_special_dir(char *name) {
  return (strcmp(name, ".") == 0 || strcmp(name, "..") == 0);
}

// 拼接路径字符串
void join_path(char *path, char *file, char *buf) {
  strcpy(buf, path);
  int len = strlen(buf);
  if (buf[len - 1] != '/') {
    buf[len] = '/';
    buf[len + 1] = '\0';
  }
  strcpy(buf + strlen(buf), file); // 替代 strcat 的功能
}

// 查找目录中是否存在目标文件
void find(char *path, char *target) {
  int fd;
  struct stat st;
  struct dirent de;
  char subpath[512]; // 删除未使用的 buf

  // 打开目录
  if ((fd = open(path, 0)) < 0) {
    printf("find: cannot open %s\n", path);
    return;
  }

  // 获取文件状态
  if (fstat(fd, &st) < 0) {
    printf("find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  // 确保是目录
  if (st.type != T_DIR) {
    printf("find: %s is not a directory\n", path);
    close(fd);
    return;
  }

  // 遍历目录内容
  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0) continue; // 无效目录项
    if (is_special_dir(de.name)) continue; // 跳过 "." 和 ".."

    // 拼接路径
    join_path(path, de.name, subpath);

    // 如果是目标文件，则打印路径
    if (strcmp(de.name, target) == 0) {
      printf("%s\n", subpath);
    }

    // 如果是目录，递归调用 find
    if (stat(subpath, &st) == 0 && st.type == T_DIR) {
      find(subpath, target);
    }
  }

  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage: find <path> <name>\n");
    exit(1);
  }

  find(argv[1], argv[2]);
  exit(0);
}