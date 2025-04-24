#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)

extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern size_t get_ramdisk_size();
extern int fs_close(int fd);
extern ssize_t fs_read(int fd,void *buf,size_t len);
extern size_t fs_filesz(int fd);
extern int fs_open(const char *pathname,int flags, int mode);

uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
  //获取大小
  int fd = fs_open(filename,0,0);
  Log("Info:file '%s' opened successfully, fd = %d",filename,fd);

  size_t size = fs_filesz(fd);
  Log("Info: file size of '%s' is %zu bytes",filename,size);

  fs_read(fd,(void*)DEFAULT_ENTRY,size);
  Log("Info:successfully read %zu bytes from '%s' to 0x%x",size,filename,DEFAULT_ENTRY);
  fs_close(fd);
  
  //size_t ramdisk_size = get_ramdisk_size();
  //读取到目标地址
 // ramdisk_read(DEFAULT_ENTRY,0,ramdisk_size);
  //返回入口地址
  return (uintptr_t)DEFAULT_ENTRY;
}
