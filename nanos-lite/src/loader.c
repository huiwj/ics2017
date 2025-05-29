#include "common.h"

#define DEFAULT_ENTRY ((void *)0x8048000)

extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern size_t get_ramdisk_size();
extern int fs_close(int fd);
extern ssize_t fs_read(int fd,void *buf,size_t len);
extern size_t fs_filesz(int fd);
extern int fs_open(const char *pathname,int flags, int mode);
extern void* new_page(void);

uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
  //获取大小
  int fd = fs_open(filename,0,0);
  //Log("Info:file '%s' opened successfully, fd = %d",filename,fd);

  int size = fs_filesz(fd);
  //Log("Info: file size of '%s' is '%zu' bytes",filename,size);

  void* va  = DEFAULT_ENTRY;
  void* pa;
  while(size>0)
  {
    pa = new_page();
    _map(as,va,pa);
    fs_read(fd,pa,4096);
    va += 4096;
    size -= 4096;
  }
  fs_close(fd);
  return (uintptr_t)DEFAULT_ENTRY;
}
