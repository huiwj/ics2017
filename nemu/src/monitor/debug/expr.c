#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <string.h>
#include <stdio.h>

enum {
  TK_NOTYPE = 256, //空白
  TK_EQ =1,
  TK_NEQ,
  TK_AND,
  TK_OR,
  TK_NOT,
  TK_NUM,
  TK_REG,
  TK_HEX,//十六进制
  //TK_DEREF, //指针解引用

  TK_LEFT,
  TK_RIGHT,

  TK_ADD,
  TK_SUB,
  TK_MUL,
  TK_DIV

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", TK_ADD},         // plus
  {"==", TK_EQ},        // equal
  {"!=", TK_NEQ},
  {"\\&\\&",TK_AND},
  {"\\|\\|",TK_OR},
  {"!",TK_NOT},
  {"[0-9]*",TK_NUM},
  {"\\$[a-zA-z]*[0-9]*",TK_REG},
  {"0x[0-9a-fA-F]+",TK_HEX},
   //{"\\*",TK_DEREF},
  {"\\(",TK_LEFT},
  {"\\)",TK_RIGHT},
  {"\\-",TK_SUB},
  {"\\*",TK_MUL},
  {"\\/",TK_DIV}


           
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0; //检查字符串的位置
  int i;
  regmatch_t pmatch; //存储正则表达式的匹配结果

  nr_token = 0; //识别token数量

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {  //遍历正则式
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        //尝试匹配第i个正则式
        char *substr_start = e + position; //匹配的子字符串的起始位置
        int substr_len = pmatch.rm_eo; //匹配到的子字符串长度

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len; //跳过已匹配的子字符串

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE:
            break;
          case TK_NUM:
            if(nr_token >= 32)
            {
              printf("ERROR:too many tokens!");
              return false;
            }

            if(substr_len>=32)
            {
              printf("ERROR:tokens are too long!");
              return false;
            }

            tokens[nr_token].type = TK_NUM;

            for(int j=0;j < sizeof(tokens[nr_token].str)-1 && j < substr_len;j++)
            {
              tokens[nr_token].str[j]=substr_start[j];
            }
            tokens[nr_token].str[substr_len] = 0;

            nr_token++;

            break;
          case TK_ADD:
            tokens[nr_token].type = TK_ADD;
            nr_token++;
            break;
          case TK_SUB:
            tokens[nr_token].type = TK_SUB;
            nr_token++;
            break;
          case TK_DIV:
            tokens[nr_token].type = TK_DIV;
            nr_token++;
            break;
          case TK_MUL:
            tokens[nr_token].type = TK_MUL;
            nr_token++;
            break;
          case TK_HEX:
            if(nr_token >= 32)
            {
              printf("ERROR:too many tokens!");
              return false;
            }

            if(substr_len>=32)
            {
              printf("ERROR:tokens are too long!");
              return false;
            }

            tokens[nr_token].type = TK_HEX;

            for(int j=0;j < sizeof(tokens[nr_token].str)-1 && j < substr_len;j++)
            {
              tokens[nr_token].str[j]=substr_start[j];
            }
            tokens[nr_token].str[substr_len] = 0;

            nr_token++;

            break;
          case TK_REG:
            if(nr_token >= 32)
            {
              printf("ERROR:too many tokens!");
              return false;
            }

            if(substr_len>=32)
            {
              printf("ERROR:tokens are too long!");
              return false;
            }


            tokens[nr_token].type = TK_REG;

            for(int j=0;j < sizeof(tokens[nr_token].str)-1 && j < substr_len;j++)
            {
              tokens[nr_token].str[j]=substr_start[j];
            }
            tokens[nr_token].str[substr_len] = 0;

            nr_token++;

            break;
          case TK_EQ:
            tokens[nr_token].type = TK_EQ;
            nr_token++;
            break;
          case TK_NEQ:
            tokens[nr_token].type = TK_NEQ;
            nr_token++;
            break;
          case TK_AND:
            tokens[nr_token].type = TK_AND;
            nr_token++;
            break;
          case TK_OR:
            tokens[nr_token].type = TK_OR;
            nr_token++;
            break;
          case TK_NOT:
            tokens[nr_token].type = TK_NOT;
            nr_token++;
            break;
          case TK_LEFT:
            tokens[nr_token].type = TK_LEFT;
            nr_token++;
            break;
          case TK_RIGHT:
            tokens[nr_token].type = TK_RIGHT;
            nr_token++;
            break;
          default: 
            printf("not such tokens!");
            break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}

int check_parenthese(int p ,int q)
{
  int left = 0; //括号匹配
  int is_closed = false; //最外层括号
  int flag =0;

  if(tokens[p].type==TK_LEFT&&tokens[q].type ==TK_RIGHT)
  {
    flag =1;

  }
  

  for (int i=p;i<=q;i++)
  {
    if(tokens[i].type == TK_LEFT)
    {
      left ++;
    }
    else if(tokens[i].type == TK_RIGHT)
    {
      left --;
    }

    if(left<0) //不匹配
    {
      return -1;
    }

    if(left == 0 && i!=q) //最外层不匹配
    {
      flag = 0;
      is_closed = false;
    }
  }

  if(left == 0 && flag == 1 && is_closed == 1) //最外层成功匹配
  {
    return 1;
  }
  else if(left == 0 && flag == 0)
  {
    return 0; //内层匹配
  }
  else return -1;//不匹配
}

int get_precedence(int type) //优先级匹配
{
  switch (type)
  {
  case TK_OR:
    return 1;
  case TK_AND:
    return 2;
  case TK_EQ:
  case TK_NEQ:
    return 3;
  case TK_ADD:
  case TK_SUB:
    return 4;
  case TK_DIV:
  case TK_MUL:
    return 5;
  case TK_NOT:
  case TK_LEFT:
  case TK_RIGHT:
    return 6;
  default:
    return -1;
  }
}

bool is_right(int type) //判断是否是右结合
{
  switch (type)
  {
  case TK_NOT:
    return true;
  
  default:
    return false;
  }
}

int find_dominant_operator(int p,int q)
{
  int min_op = 10000;
  int main_op = -1;
  int left = 0 ;

  for(int i = p;i <= q; i++)
  {
    if(tokens[i].type == TK_LEFT)
    {
      left ++;
    }else if(tokens[i].type ==  TK_RIGHT)
    {
      left--;
    }

    if(left != 0) //跳过括号
    {
      continue;
    }

    int op = get_precedence(tokens[i].type);

    if(op == -1) //非运算符
    {
      continue;
    }

    if(op>min_op) //运算级高
    {
      continue;
    }
    else 
      if(op < min_op) //更低的运算级
      {
        min_op = op;
        main_op = i;
      }
      else //相同运算级
        {
          if(is_right(tokens[i].type)) //右结合取最右边
          {
            main_op = i;
          }
          else
          {
            if(main_op < 0) //左结合第一次出现
            {
              main_op = i;
            }
          }
        }
  } 
  return main_op;
}

uint32_t eval(int p,int q)
{
  if(p>q)
  {
    printf("Bad expression\n");
    assert(0);
  }
  else if(p == q)
  {
    if(tokens[p].type == TK_NUM)
    {
      int value = 0;
      for(int i=0;tokens[p].str[i]!=0;i++)
      {
        value = value * 10 + (tokens[p].str[i]-'0');
      }
      return value;
    }

    else if (tokens[p].type == TK_HEX)  //解析十六进制数
    {
      int value = 0;
      sscanf(tokens[p].str,"%x",&value);
      return value;
    }
    else if (tokens[p].type == TK_REG)
    {
      if (strlen(tokens[p].str)==3)
      {
        for (int i=0;i<8;i++)
        {
          return reg_l(i); //32位寄存器
        }

        if(strcmp(tokens[p].str,"eip")==0)
        {
          return cpu.eip; //特殊处理
        }
      }
      else if(strlen(tokens[p].str)==2)
      {
        for (int i=0;i<8;i++)
        {
          if(strcmp(tokens[p].str,reg_name(i,2))==0)
          {
            return reg_w(i); //16
          }

           if(strcmp(tokens[p].str,reg_name(i,1))==0)
          {
            return reg_b(i);  //8
          }


        }
      }

      printf("Wrong register!\n");
      return 0;
    }
  }
  else if(check_parenthese(p,q)==1)
  {
    return eval(p+1,q-1);
  }
  else 
  {
    if(check_parenthese(p,q)<0)
    {
      printf("unmatch parentheses\n");
      assert(0);
      return 0;
    }

    int op = find_dominant_operator(p,q);

    switch ((tokens[op].type))
    {
    case TK_NOT:
      //bool res_val = eval(p+1,q);
      //if(res_val == 0)
        //return 1;
      //else return 0;

      return !eval(p+1,q);
    
    default:
      break;
    }

    uint32_t val1 = eval(p,op-1);
    uint32_t val2 = eval(op+1,q);

    switch (tokens[op].type)
    {
      case TK_ADD: return val1 + val2;
      case TK_SUB: return val1 - val2;
      case TK_MUL: return val1 * val2;
      case TK_DIV:
        if(val2 == 0)
        {
          printf("Division by zero\n");
          return 0;
        }
        return val1 / val2;
      case TK_EQ: return val1 == val2;
      case TK_NEQ: return val1 != val2;
      case TK_AND: return val1 && val2;
      case TK_OR: return val1 || val2;
      default:
        printf("unsupported operator\n");
       // assort(0);
        return 0;
    }



  }
  return 0;

}


