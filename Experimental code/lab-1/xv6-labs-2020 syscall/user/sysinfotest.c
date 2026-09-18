#include "kernel/types.h"
#include "kernel/riscv.h"
#include "kernel/sysinfo.h"
#include "user/user.h"

// 封裝 sysinfo 
void get_sysinfo(struct sysinfo* info) {
  if (sysinfo(info) < 0) {
    printf("Error: sysinfo failed\n");
    exit(1);
  }
}

// 計算實體記憶體
int count_free_mem() {
  uint64 origin = (uint64)sbrk(0);
  struct sysinfo info;
  int total = 0;

  while ((uint64)sbrk(PGSIZE) != 0xffffffffffffffff) {
    total += PGSIZE;
  }

  get_sysinfo(&info);
  if (info.freemem != 0) {
    printf("Error: freemem should be 0, but got %d\n", info.freemem);
    exit(1);
  }

  sbrk(-(sbrk(0) - origin));
  return total;
}

// 測試記憶體資訊是否正確
void test_memory() {
  struct sysinfo info;
  uint64 free_now = count_free_mem();

  get_sysinfo(&info);
  if (info.freemem != free_now) {
    printf("Error: expected %d, got %d\n", free_now, info.freemem);
    exit(1);
  }

  if ((uint64)sbrk(PGSIZE) == 0xffffffffffffffff) {
    printf("Error: sbrk failed\n");
    exit(1);
  }

  get_sysinfo(&info);
  if (info.freemem != free_now - PGSIZE) {
    printf("Error: freemem mismatch after alloc\n");
    exit(1);
  }

  sbrk(-PGSIZE);
  get_sysinfo(&info);
  if (info.freemem != free_now) {
    printf("Error: freemem mismatch after free\n");
    exit(1);
  }
}

// 測試 sysinfo 系統呼叫合法性
void test_syscall() {
  struct sysinfo info;

  if (sysinfo(&info) < 0) {
    printf("Error: sysinfo call failed\n");
    exit(1);
  }

  if (sysinfo((struct sysinfo*)0xdeadbeef00001234) != 0xffffffffffffffff) {
    printf("Error: sysinfo should fail with bad arg\n");
    exit(1);
  }
}

// 測試系統進程數資訊
void test_process_count() {
  struct sysinfo info;
  uint64 original_nproc;
  int pid, status;

  get_sysinfo(&info);
  original_nproc = info.nproc;

  pid = fork();
  if (pid < 0) {
    printf("Error: fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    get_sysinfo(&info);
    if (info.nproc != original_nproc + 1) {
      printf("Error: child sees wrong nproc\n");
      exit(1);
    }
    exit(0);
  }

  wait(&status);
  get_sysinfo(&info);
  if (info.nproc != original_nproc) {
    printf("Error: parent sees wrong nproc after child exit\n");
    exit(1);
  }
}

int main(int argc, char* argv[]) {
  printf("sysinfotest: start\n");
  test_syscall();
  test_memory();
  test_process_count();
  printf("sysinfotest: OK\n");
  exit(0);
}
