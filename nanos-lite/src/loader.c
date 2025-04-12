#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)

extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern size_t get_ramdisk_size();

uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
  //获取大小
  size_t ramdisk_size = get_ramdisk_size();
  //读取到目标地址
  ramdisk_read(DEFAULT_ENTRY,0,ramdisk_size);
  //返回入口地址
  return (uintptr_t)DEFAULT_ENTRY;
}
