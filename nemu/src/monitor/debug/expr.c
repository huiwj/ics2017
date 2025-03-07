#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <string.h>

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

bool check_parenthese(int p ,int q)
{
  int left = 0;
  int is_closed = true;

  if(tokens[p].type!=TK_LEFT||tokens[q].type !=TK_RIGHT)
  {
    return false;

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

    if(left<0)
    {
      return false;
    }

    if(left == 0 && i!=q)
    {
      is_closed = false;
    }
  }

  if(left == 0 && is_closed == true)
  {
    return true;
  }
  else return false;
}
