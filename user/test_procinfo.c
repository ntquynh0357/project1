#include "kernel/types.h"
#include "user/user.h"

struct procinfo {
  int pid;
  int ppid;
  int state;
  uint64 sz;
  char name[16];
};

int main()
{
  struct procinfo info;
  int pid = getpid();

  if(procinfo(pid, &info) < 0){
    printf("fail\n");
    exit(1);
  }

  printf("Name: %s\n", info.name);
  printf("PID: %d\n", info.pid);
  printf("PPID: %d\n", info.ppid);
  printf("STATE: %d\n", info.state);
  printf("MEM: %ld\n", info.sz);

  exit(0);
}