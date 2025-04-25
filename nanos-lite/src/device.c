#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) {
  int key = _read_key();
  if(key != _KEY_NONE)
  {
    Log("Rae Key: 0x%X\n",key);
    int is_press = (key & 0x8000)?1:0;
    int keybord = key & 0x7FFF;
    if(keybord<sizeof(keyname)/sizeof(keyname[0]))
    {
      const char * act = is_press ? "kd" : "ku";
      const char * name = keyname[keybord];
      int n = snprintf(buf,len,"%s %s\n",act,name);
      return (n>0)?n:0;
    }
    else
    {
      snprintf(buf,len,"UNnknow Key :%s %s\n");
      return strlen(buf); 
    }
    
  }
  
  {
    sprintf(buf,"t %d\n",(uint32_t)_uptime());
  } 

  return strlen(buf);
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) { 
  memcpy(buf,dispinfo + offset,len);
}

void fb_write(const void *buf, off_t offset, size_t len) {
  int start = offset/4; //每像素4字节
  int x = start % _screen.width;
  int y = start / _screen.width;
  _draw_rect(buf,x,y,len/4,1);

}

void init_device() {
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention

  sprintf(dispinfo,"WIDTH:%d\nHEIGHT:%d\n",_screen.width,_screen.height);
}
