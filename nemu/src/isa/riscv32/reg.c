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

#include "local-include/reg.h"
#include <isa.h>
#include <stdio.h>
#include <string.h>

const char *regs[] = {"$0", "ra", "sp", "gp", "tp",  "t0",  "t1", "t2", "s0", "s1", "a0",
                      "a1", "a2", "a3", "a4", "a5",  "a6",  "a7", "s2", "s3", "s4", "s5",
                      "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

void isa_reg_display() {
    for (int i = 0; i < 32; i += 4) {
        printf("%s: 0x%08x\t%s: 0x%08x\t%s: 0x%08x\t%s: 0x%08x\t\n", reg_name(i), gpr(i),
               reg_name(i + 1), gpr(i + 1), reg_name(i + 2), gpr(i + 2), reg_name(i + 3),
               gpr(i + 3));
    }
}

word_t isa_reg_str2val(const char *s, bool *success) {
    *success = false;
    if (s[0] != '$') {
        return -1;
    }

    const char *name = s + 1;
    extern const char *regs[];
    for (int i = 0; i < MUXDEF(CONFIG_RVE, 16, 32); i++) {
        if (strcmp(regs[i], name) == 0 || (i == 0 && strcmp(s, "$0") == 0)) {
            *success = true;
            return i;
        }
    }
    return -1;
}
