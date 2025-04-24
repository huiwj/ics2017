#include "fs.h"

extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern void ramdisk_write(const void *buf, off_t offset, size_t len);
extern size_t events_read(void *buf, size_t len);
extern void dispinfo_read(void *buf, off_t offset, size_t len);
extern void fb_write(const void *buf, off_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  off_t disk_offset;
  off_t  open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO, FD_NORMAL};

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin (note that this is not the actual stdin)", 0, 0},
  {"stdout (note that this is not the actual stdout)", 0, 0},
  {"stderr (note that this is not the actual stderr)", 0, 0},
  [FD_FB] = {"/dev/fb", 0, 0},
  [FD_EVENTS] = {"/dev/events", 0, 0},
  [FD_DISPINFO] = {"/proc/dispinfo", 128, 0},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
  file_table[FD_FB].size = _screen.width * _screen.height *4;
}

ssize_t fs_write(int fd,const void *buf,size_t len)
{
  ssize_t size = file_table[fd].size;

 
  switch(fd)
  {
    case FD_STDIN:
      return 0;
    case FD_STDOUT:
    case FD_STDERR:
      Log("sys_write:fd %d len %d",fd,len);
      for (int i =0;i<len;i++)
      {
        _putc(((char*)buf)[i]);
      }
      
      return len;
    case FD_FB:
      fb_write(buf,file_table[fd].open_offset,len);
      file_table[fd].open_offset += len;
      return len;
      
    default:
       if(file_table[fd].open_offset + len > size)
        {
          len = size - file_table[fd].open_offset;
        }
      ramdisk_write((void*)buf,file_table[fd].disk_offset + file_table[fd].open_offset,len);
      file_table[fd].open_offset += len;
      return len;

  }
}

int fs_open(const char *pathname,int flags, int mode)
{
  for(int i=0;i<NR_FILES;i++)
  {
    if(strcmp(file_table[i].name,pathname)==0)
    {
      file_table[i].open_offset=0;
      return i;
    }
  }
  panic("File '%s' not found",pathname);
  return -1;
}

ssize_t fs_read(int fd,void *buf,size_t len)
{
  Log("fs_read:fd=%d,request len = %zu,open_offset = %d",fd,len,file_table[fd].open_offset);
  if(file_table[fd].open_offset + len > file_table[fd].size)
  {
    len = file_table[fd].size - file_table[fd].open_offset;
  }
  if(fd==FD_STDIN || fd == FD_STDOUT || fd == FD_STDERR) return 0;
  if(fd == FD_DISPINFO) //固定内容文件使用偏移读取
  {
    dispinfo_read(buf,file_table[fd].open_offset,len);
    file_table[fd].open_offset += len;
    return len;
  }
  if(fd == FD_EVENTS)//不断产生数据的设备，返回长度由events_read决定
  {
    return events_read((void*)buf,len);
  }
  //普通文件
  {
    ramdisk_read((void*)buf,file_table[fd].disk_offset + file_table[fd].open_offset,len);
    file_table[fd].open_offset += len;
    return len;
  }

}

int fs_close(int fd)
{
  return 0;
}

size_t fs_filesz(int fd)
{
  return file_table[fd].size;
}

off_t fs_lseek(int fd,off_t offset, int whence)
{
  Finfo *f = &file_table[fd];
  off_t new_offset = 0;

  switch (whence)
  {
  case SEEK_SET:
    new_offset = offset;
    break;
  case SEEK_CUR:
    new_offset = f->open_offset + offset;
    break;
  case SEEK_END:
    new_offset = f->size + offset;
    break;
  default:
    return -1;
  }

  if(new_offset > f->size)
  {
    return -1;
  }

  f->open_offset = new_offset;
  return new_offset;
}