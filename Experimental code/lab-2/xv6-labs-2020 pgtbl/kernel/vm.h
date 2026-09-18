// kernel/vm.h
#ifndef _VM_H_
#define _VM_H_

#include "types.h"
#include "defs.h"

//4096 bytes
#define PGSIZE 4096
#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))

typedef uint64* pagetable_t;

void kvminit(void);
void kvminithart(void);
void kvmmap(uint64, uint64, uint64, int);
pagetable_t kvmmake(void);
void uvminit(pagetable_t, uchar *, uint);
uint64 uvmalloc(pagetable_t, uint64, uint64);
uint64 uvmdealloc(pagetable_t, uint64, uint64);
int uvmcopy(pagetable_t, pagetable_t, uint64);
void uvmfree(pagetable_t, uint64);
void freewalk(pagetable_t);
void vmprint(pagetable_t pagetable);

#endif
