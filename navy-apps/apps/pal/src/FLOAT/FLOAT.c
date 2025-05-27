#include "FLOAT.h"
#include <stdint.h>
#include <assert.h>

FLOAT F_mul_F(FLOAT a, FLOAT b) {

  return ((int64_t)a * (int64_t)b)>>16;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
  //assert(0);

  int is_neg = 0;//记录符号
  if(a<0)
  {
    a=-a;
    is_neg ^= 1;
  }
  if(b<0)
    {
      b=-b;
      is_neg ^= 1;
    }

//初始整数
  int quotient = a/b;
  int remainder = a%b;
//扩展小数
for(int i=0;i<16;++i)
{
  quotient <<=1; //左移一位
  remainder <<=1; //余数*2
  if(remainder >= b) //大于除数
  {
    remainder -= b;
    quotient |= 1;
  }
}

  return is_neg ? -quotient : quotient;
}

FLOAT f2F(float a) {
  /* You should figure out how to convert `a' into FLOAT without
   * introducing x87 floating point instructions. Else you can
   * not run this code in NEMU before implementing x87 floating
   * point instructions, which is contrary to our expectation.
   *
   * Hint: The bit representation of `a' is already on the
   * stack. How do you retrieve it to another variable without
   * performing arithmetic operations on it directly?
   */

  //assert(0);

 
  union float_
  {
    struct 
    {
      uint32_t max:23; //尾数
      uint32_t exp:8; //指数
      uint32_t sig:1; //符号位
    };
    uint32_t value;
    
  };
  union float_ f;
  
  f.value = *((uint32_t*)(void*)&a);

  int e = f.exp - 127; //偏移指数转为实际指数值

  FLOAT result;
  if(e<=7)
  {
    result = (f.max | (1<<23)) >> 7-e;
  }
  else
  {
    result = (f.max | (1<<23)) << (e-7);
  }

  return f.sig == 0 ? result : (result | (1<<31));
  
  
  
}

FLOAT Fabs(FLOAT a) {
  
  return (a > 0)?a:-a;
}

/* Functions below are already implemented */

FLOAT Fsqrt(FLOAT x) {
  FLOAT dt, t = int2F(2);

  do {
    dt = F_div_int((F_div_F(x, t) - t), 2);
    t += dt;
  } while(Fabs(dt) > f2F(1e-4));

  return t;
}

FLOAT Fpow(FLOAT x, FLOAT y) {
  /* we only compute x^0.333 */
  FLOAT t2, dt, t = int2F(2);

  do {
    t2 = F_mul_F(t, t);
    dt = (F_div_F(x, t2) - t) / 3;
    t += dt;
  } while(Fabs(dt) > f2F(1e-4));

  return t;
}
