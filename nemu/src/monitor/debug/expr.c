#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ=255,TK_NOTEQ=254,TK_AND=253,TK_OR=252,TK_REG=251,TK_HEXA=250,TK_DEC=249,TK_VAL=248,TK_POINTER=247,TK_NEG=246,TK

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
  {"\\+", '+'},         // plus
  {"==", TK_EQ},         // equal
  {"-", '-'},
  {"\\*",'*'},
  {"\\/",'/'},
  {"!=",TK_NOTEQ},
  {"!",'!'},
  
  {"&&",TK_AND},
  {"\\|\\|",TK_OR},
  
  {"0[xX][0-9a-fA-F]+",TK_HEXA},
  {"[1-9][0-9]*|0",TK_DEC},
  {"[a-zA-Z_][a-zA-Z0-9_]*",TK_VAL},
  {"\\$[a-z]{2,3}",TK_REG},
  {"\\(",'('},
  {"\\)",')'},
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
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
	  case 256:
		break;
          default: 
		strncpy(tokens[nr_token].str, substr_start, substr_len);
 		tokens[nr_token].str[substr_len]='\0';
 		tokens[nr_token].type=rules[i].token_type;
		
		if(tokens[nr_token].type=='-'&&(nr_token == 0||(tokens[nr_token-1].type!=TK_DEC &&tokens[nr_token-1].type!=TK_REG &&tokens[nr_token-1].type!=TK_HEXA )))
 			tokens[nr_token].type=TK_NEG;		
 		if(tokens[nr_token].type=='*'&&(nr_token == 0|| (tokens[nr_token-1].type!=TK_DEC && tokens[nr_token-1].type!=TK_REG && tokens[nr_token-1].type!=TK_HEXA )))
			tokens[nr_token].type=TK_POINTER;

		nr_token++;
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

bool check_parentheses(int p,int q){
	 if(strcmp(tokens[p].str, "(") != 0 || strcmp(tokens[q].str, ")") != 0) return false;
	 int num=0;
	 for(int i=p;i<=q;i++){
 		if(tokens[i].type == '(') num++;
 		else if(tokens[i].type == ')') num--;
 		if(num==0&&i<q) return false; 
 		if(num<0) {
 			printf("parentheses Error!\n");
 			return false;
		 }
		if(num == 0 && i != q) return false;
 }
 		if(num!=0) {
 		printf("Parentheses Error!\n");
 		return false;
 }
 return true;
}

int dominant_op(int l, int r) {
	int i;
	int pos = l;
	int pri = 0;
	int b_num = 0;
	for (i = r; i >= l; i --) {
		if (tokens[i].type == ')') b_num++;
		if (tokens[i].type == '(') b_num--;
		if (b_num != 0) continue;
		switch (tokens[i].type) {
		case '+': { // pri = 4
			if (pri < 4) pos = i, pri = 4;
			break;
		}
		case '-': { // pri = 4
			if (pri < 4) pos = i, pri = 4;
			break;
		}
		case '*': { // pri = 3
			if (pri < 3) pos = i, pri = 3;
			break;
		}
		case '/': { // pri = 3
			if (pri < 3) pos = i, pri = 3;
			break;
		}
		case '!': { // pri = 2
			if (pri < 2) pos = i, pri = 2;
			break;
		}
		case TK_EQ: { // pri = 7
			if (pri < 7) pos = i, pri = 7;
			break;
		}
		case TK_NOTEQ: { // pri = 7
			if (pri < 7) pos = i, pri = 7;
			break;
		}
		case TK_AND: { // pri = 11
			if (pri < 11) pos = i, pri = 11;
			break;
		}
		case TK_OR: { // pri = 12
			if (pri < 12) pos = i, pri = 12;
			break;
		}
		case TK_NEG: { // pri = 2
			if (pri < 2) pos = i, pri = 2;
			break;
		}
		case TK_POINTER: { // pri = 2
			if (pri < 2) pos = i, pri = 2;
			break;
		}
		default: break;
		}
	}
	return pos;
}

uint32_t eval(int l, int r) {

	if (l > r) {

		printf("Expression Error!\n");

		return 0;

	}

	if (l == r) {

		uint32_t num = 0;

    if (tokens[l].type == TK_DEC) {

			sscanf(tokens[l].str, "%d", &num);

		} else if (tokens[l].type == TK_HEXA) {

			sscanf(tokens[l].str, "%x", &num);

		} else if (tokens[l].type == TK_REG) {

			if (strlen(tokens[l].str) == 4) { 

				int i;

				for (i = R_EAX; i <= R_EDI ; i ++) {
					char reg[4];
					for(int j=0;j<3;j++){
						reg[j]=tokens[l].str[j+1];
					}
					reg[3]='\0';
					if (strcmp(reg, regsl[i]) == 0) {
						return reg_l(i);
						break;

					}

				}

				if (i > R_EDI) {

					if (strcmp(tokens[l].str, "$eip") == 0) {

						return num = cpu.eip;

					} else {return 0;}

				} else return num = reg_l(i);

			}

			else if (strlen(tokens[l].str) == 3) {

				int i;

				for (i = R_AX; i <= R_DI; i ++) {
					char reg[3];
					for(int j=0;j<2;j++){
						reg[j]=tokens[l].str[j+1];
					}
					reg[2]='\0';
					if (strcmp(reg, regsw[i]) == 0) {
						return num = reg_w(i);

					}

				}

				for (i = R_AL; i <= R_BH; i ++) {
					char reg[3];
					for(int j=0;j<2;j++){
						reg[j]=tokens[l].str[j+1];
					}
					reg[2]='\0';
					if (strcmp(reg, regsb[i]) == 0) {
						return num = reg_b(i);

					}

				}


				return 0;

			}

			else {return 0;}

		} else {return 0;}

		return num;

	}

	uint32_t ans = 0;



	if (check_parentheses(l, r)) return eval(l + 1, r - 1);

	else {

		int pos = dominant_op(l, r);

		if (l == pos || tokens[pos].type == '!' || tokens[pos].type == TK_NEG || tokens[pos].type == TK_POINTER) {

			uint32_t r_ans = eval(pos + 1, r);

			switch (tokens[pos].type) {

			case TK_POINTER: ;return vaddr_read(r_ans, 4);

			case '!': return !r_ans;

			case TK_NEG: return -r_ans;

			default: {return 0;}

			}

		}

		uint32_t l_ans = eval(l, pos - 1), r_ans =  eval(pos + 1, r);

		switch (tokens[pos].type) {

		case '+': ans = l_ans + r_ans; break;

		case '-': ans = l_ans - r_ans; break;

		case '*': ans = l_ans * r_ans; break;

		case '/': if (r_ans == 0) { return 0;} else ans = l_ans / r_ans; break;

		case TK_EQ : ans = l_ans == r_ans; break;

		case TK_NOTEQ: ans = l_ans != r_ans; break;

		case TK_AND: ans = l_ans && r_ans; break;

		case TK_OR : ans = l_ans && r_ans; break;

		default: {return 0;}

		}

	}

	return ans;

}
		

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  *success = true;

  return eval(0, nr_token - 1);

}
