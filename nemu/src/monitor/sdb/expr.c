/***************************************************************************************
 * Copyright (c) 2014-2024 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include "common.h"
#include "debug.h"
#include <assert.h>
#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

// start from 256 to avoid collide with ascii
enum { TK_NOTYPE = 256, TK_REG, TK_VAR, TK_EQ, TK_NEQ, TK_AND, TK_OR, TK_PLUS, TK_MINUS, TK_MULTIPLY, TK_DIVIDE, TK_NUM, TK_PAREN_L, TK_PAREN_R };
enum { PREC_EQ, PREC_LESS, PREC_GREAT};

bool is_op(int type) {
    switch(type) {
        case TK_PLUS:
        case TK_MINUS:
        case TK_MULTIPLY:
        case TK_DIVIDE:
        case TK_EQ:
        case TK_NEQ:
        case TK_AND:
        case TK_OR:
            return true;
    }
    return false;
}
int get_prec(int type) {
    switch (type) {
        case TK_PLUS:
        case TK_MINUS:
            return 6;
        case TK_MULTIPLY:
        case TK_DIVIDE:
            return 5;
        case TK_EQ:
        case TK_NEQ:
            return 7;
        case TK_AND:
        case TK_OR:
            return 11;
    }
    Log("invalid type");
    return -1;
}
int prec_cmp(int t1, int t2) {
    int t1_prec = get_prec(t1);
    int t2_prec = get_prec(t2);
    if (t1_prec < t2_prec) {
        return PREC_GREAT;
    } else if (t1_prec > t2_prec) {
        return PREC_LESS;
    } else {
        return PREC_EQ;
    }
}

static struct rule {
    const char *regex;
    int token_type;
} rules[] = {

    /* TODO: Add more rules.
     * Pay attention to the precedence level of different rules.
     */

    {"(0[xX][0-9a-fA-F]+|0[bB][01]+|[0-9]+)", TK_NUM},
    {" +", TK_NOTYPE},    // spaces
    {"\\+", TK_PLUS},     // plus
    {"\\*", TK_MULTIPLY}, // multiply
    {"-", TK_MINUS},      // minus
    {"\\(", TK_PAREN_L},      // minus
    {"\\)", TK_PAREN_R},      // minus
    {"\\/", TK_DIVIDE},   // divide
    {"==", TK_EQ},       
    {"!=", TK_NEQ},       
    {"\\&\\&", TK_AND},       
    {"\\|\\|", TK_OR},       
    {"\\$[a-zA-Z0-9]+", TK_REG},       
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
    int i;
    char error_msg[128];
    int ret;

    for (i = 0; i < NR_REGEX; i++) {
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

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used)) = 0;

static bool make_token(char *e) {
    int position = 0;
    int i;
    regmatch_t pmatch;

    nr_token = 0;

    while (e[position] != '\0') {
        /* Try all rules one by one. */
        for (i = 0; i < NR_REGEX; i++) {
            if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
                char *substr_start = e + position;
                int substr_len = pmatch.rm_eo;

                Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex,
                    position, substr_len, substr_len, substr_start);

                position += substr_len;

                /* TODO: Now a new token is recognized with rules[i]. Add codes
                 * to record the token in the array `tokens'. For certain types
                 * of tokens, some extra actions should be performed.
                 */
                switch (rules[i].token_type) {
                    case TK_NOTYPE:  {
                            break;
                        }
                    default: {
                            Token *tok = malloc(sizeof(Token));
                            tok->type = rules[i].token_type;
                            strncpy(tok->str, substr_start, substr_len);
                            tok->str[substr_len] = '\0';
                            tokens[nr_token++] = *tok;
                            break;
                        }
                }

                break;
            }
        }

        if (i == NR_REGEX) {
            fprintf(stderr, "no match at position %d\n%s\n%*.s^\n", position, e, position, "");
            return false;
        }
    }

    return true;
}

int parse_number(const char *str) {
    // 检查是否是负号
    int negative = 0;
    if (*str == '-') {
        negative = 1;
        str++;
    }

    if (strncmp(str, "0x", 2) == 0 || strncmp(str, "0X", 2) == 0) {
        // 十六进制
        int val = (int)strtol(str, NULL, 16);
        return negative ? -val : val;
    } else if (strncmp(str, "0b", 2) == 0 || strncmp(str, "0B", 2) == 0) {
        // 二进制
        int val = (int)strtol(str, NULL, 2);
        return negative ? -val : val;
    } else {
        // 默认十进制
        int val = (int)strtol(str, NULL, 10);
        return negative ? -val : val;
    }
}

bool check_parentheses(int start, int end) {
    assert(end > start);
    return tokens[end].type == TK_PAREN_R && tokens[start].type == TK_PAREN_L;
}


word_t eval(int start, int end, bool *success) {
    if (start > end) {
        *success = false;
        return 0;
    } else if (start == end) {
        assert(tokens[end].type == TK_NUM || tokens[end].type == TK_REG);
        if (tokens[end].type == TK_REG) {
            const char *name = tokens[end].str;
            if (strcmp(name, "$pc") == 0) {
                *success = true;
                return cpu.pc;
            }
            int reg_idx = isa_reg_str2val(name, success);
            if (!success) {
                return 0;
            }
            *success = true;
            return cpu.gpr[reg_idx];
        }
        return parse_number(tokens[end].str);
    } else if (check_parentheses(start, end)) {
        return eval(start + 1, end - 1, success);
    } else {
        int main_op = 0;
        int op_loc = start;
        for (int i = start; i <= end; i++) {
            if (tokens[i].type == TK_PAREN_L) {
                while (i <= end && tokens[i].type != TK_PAREN_R) {
                    i += 1;
                }
                if (i > end || tokens[i].type != TK_PAREN_R) {
                    Log("ERROR: parentheses is not paired!");
                    return -1;
                }
                continue;
            }
            if (is_op(tokens[i].type)) {
                if (main_op == 0) {
                    main_op = tokens[i].type;
                    op_loc = i;
                } else {
                    switch (prec_cmp(main_op, tokens[i].type)) {
                        case PREC_GREAT:
                            {
                                main_op = tokens[i].type;
                                op_loc = i;
                                break;
                            }
                        case PREC_LESS:
                        case PREC_EQ:
                            break;
                    }
                }
            }
        }
        assert(op_loc > start && op_loc < end);
        int val1 = eval(start, op_loc - 1, success);
        int val2 = eval(op_loc + 1, end, success);
        if (!success) {
            return 0;
        }
        *success = true;
        switch (main_op) {
            case TK_PLUS: return val1 + val2;
            case TK_MINUS: return val1 - val2;
            case TK_MULTIPLY: return val1 * val2;
            case TK_DIVIDE: return val1 / val2;
            case TK_EQ: return val1 == val2;
            case TK_NEQ: return val1 != val2;
            case TK_AND: return val1 && val2;
            case TK_OR: return val1 || val2;
        }
    }
    return 0;
}

word_t expr(char *e, bool *success) {
    init_regex();
    if (!make_token(e)) {
        *success = false;
        return 0;
    }


    assert(nr_token > 0);
    // for (int i = 0; i < nr_token; i++) {
    //     printf("type: %d, str: %s\n", tokens[i].type, tokens[i].str);
    // }
    return eval(0, nr_token - 1, success);
}
