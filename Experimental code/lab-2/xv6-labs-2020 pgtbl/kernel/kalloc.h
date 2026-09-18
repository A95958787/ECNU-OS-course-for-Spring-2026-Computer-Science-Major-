// kernel/kalloc.h
#ifndef _KALLOC_H_
#define _KALLOC_H_

#include "types.h"

void* kalloc(void);
void kfree(void *);
uint64 freemem_amount(void);
void kinit(void);

#endif