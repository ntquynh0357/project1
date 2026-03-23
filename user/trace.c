#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int i;
  char *nargv[MAXARG];

  // Kiểm tra xem người dùng có nhập đủ cú pháp không (vd: trace 32 grep hello)
  if(argc < 3 || (argv[1][0] < '0' || argv[1][0] > '9')){
    fprintf(2, "Usage: %s mask command\n", argv[0]);
    exit(1);
  }

  // Gọi system call trace bạn đã viết dưới kernel
  if (trace(atoi(argv[1])) < 0) {
    fprintf(2, "%s: trace failed\n", argv[0]);
    exit(1);
  }

  // Chuẩn bị các tham số còn lại để chạy lệnh thực sự (như grep)
  for(i = 2; i < argc && i < MAXARG; i++){
    nargv[i-2] = argv[i];
  }
  nargv[i-2] = 0;

  // Thực thi lệnh đó
  exec(nargv[0], nargv);
  printf("exec %s failed\n", nargv[0]);
  exit(1);
}